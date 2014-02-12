#include "paging.h"
#include "common.h"
#include "desctab.h"
#include "console.h"

#define BITMAP_IDX(x) (x/32)	// index into our bitmap, _frames
#define BITMAP_OFF(x) (x%32)	// index into each uint32_t in our bitmap

//================================
// fields
extern uint32_t _heapAddress; // from heap.c
uint32_t*_frames;	      // a bitmap of frames
uint32_t _numFrames;	      // the number of frames

page_directory_t*_currentDirectory;

//================================
// local prototypes
static void setFrame(uint32_t frameAddr, bool used);
static bool getFrame(uint32_t frameAddr);
static uint32_t getFreeFrame(void);
static bool pageFaultHandler(registers_t regs);

//================================
// local functions
static void setFrame(uint32_t frameAddr, bool used){
    uint32_t frameNum = frameAddr/PAGING_PAGE_SIZE;
    uint32_t frameIdx = BITMAP_IDX(frameNum);
    uint16_t frameOff = BITMAP_OFF(frameNum);
    if(used)
	_frames[frameIdx] |=   1<<frameOff;
    else
	_frames[frameIdx] &= ~(1<<frameOff);
}

__UNUSED
static bool getFrame(uint32_t frameAddr){
    uint32_t frameNum = frameAddr/PAGING_PAGE_SIZE;
    uint32_t frameIdx = BITMAP_IDX(frameNum);
    uint16_t frameOff = BITMAP_OFF(frameNum);
    return _frames[frameIdx] & (1<<frameOff);
}

static uint32_t getFreeFrame(void){
    for(uint32_t i=0; i<BITMAP_IDX(_numFrames); i++)
	if(~_frames[i]) // if all bits are high, we don't bother looking
	    for(uint8_t j=0; j<32; j++)
		if(!(_frames[i] & (1<<j)))
		    return i*32+j;
    return ~0;
}

static bool pageFaultHandler(registers_t regs){
    // The faulting address is kept in CR2. Nifty!
    uint32_t badAddr = __getCR2();

    // From http://wiki.osdev.org/Page_fault:
    // P   : When set, the fault was caused by a protection violation.
    //       When not set, it was caused by a non-present page.
    // W/R : When set, write access caused the fault; otherwise read access.
    // U/S : When set, the fault occurred in user mode; otherwise in supervisor (kernel) mode.
    // RSVD: When set, one or more page directory entries contain reserved bits which are set to 1.
    //       This only applies when the PSE or PAE flags in CR4 are set to 1.
    // I/D : When set, the fault was caused by an instruction fetch.
    //       This only applies when the No-Execute bit is supported and enabled.
    
    bool p    = regs.err_code & 0x1;
    bool wr   = regs.err_code & 0x2;
    bool user = regs.err_code & 0x4;
    bool rsvd = regs.err_code & 0x8;
    bool id   = regs.err_code & 0x10;

    console_printErr("Page fault! [");
    if(!p) console_print("missing; ");
    else   console_print("present; ");

    if(wr) console_print("failed on write; ");
    else   console_print("failed on read; ");

    if(user) console_print("failed in user-mode; ");
    else     console_print("failed in kernel-mode; ");

    if(rsvd) console_print("trampled RSVD; ");
    else     console_print("safe RSVD; ");

    if(id)   console_print("failed on instruction fetch");

    console_print("]; Failure at 0x");
    console_printNum(badAddr, 16);
    console_print("\n");
    hang();
    return false;
}

//================================
// global functions
void paging_initialize(void){
    // The size of physical memory. We assume it's 16MB.
    const uint32_t kLastPage = 0x1000000;

    _numFrames = kLastPage/PAGING_PAGE_SIZE;
    _frames = (uint32_t*)kmalloc(BITMAP_IDX(_numFrames));
    memset(_frames, 0, BITMAP_IDX(_numFrames));
    
    // Create a page directory
    page_directory_t*kdir = (page_directory_t*)kamalloc(sizeof(page_directory_t));
    memset(kdir, 0, sizeof(page_directory_t));
    _currentDirectory = kdir;
    
    // Here we identity-map kernel memory so that virtual memory is the same
    // as physical memory. This makes our lives far easier.
    for(uint32_t i=0; i < _heapAddress; i+=PAGING_PAGE_SIZE){
	paging_allocFrame(paging_getPage(i,true,kdir),false,false); // readable but non-writable to userspace
	/*
	if(i/PAGING_PAGE_SIZE % 64 == 0 && i){
	    console_print("    64 pages [last 0x");
	    console_printNum(i/PAGING_PAGE_SIZE,16);
	    console_print("] now alloc'd\n");
	}
	*/
    }

    //console_print("  initialized kernel identity-mapping!\n");
    
    desctab_registerIntHandler(14, pageFaultHandler);
    paging_switchPageDirectory(kdir);
}

void paging_switchPageDirectory(page_directory_t *dir){
    _currentDirectory = dir;
    
    uint32_t cr3 = (uint32_t)(&dir->tablesPhysical);
    asm volatile("mov %0, %%cr3":: "r"(cr3));
    
    uint32_t cr0;
    asm volatile("mov %%cr0, %0": "=r"(cr0));
    cr0 |= 0x80000000; // Enable paging!
    asm volatile("mov %0, %%cr0":: "r"(cr0));
}

page_t*paging_getPage(uint32_t address, bool make, page_directory_t*dir){
    uint32_t page_index = address/PAGING_PAGE_SIZE;
    //address /= 0x1000; // Turn the address into an index.
    
    // Find the page table containing this address.
    uint32_t table_idx = page_index/1024; // 1024 entries in page_directory_t->tables
    
    if(dir->tables[table_idx]) // If this table is already assigned
	return &dir->tables[table_idx]->pages[page_index%1024];
    else if(make){
	uint32_t phys;
	dir->tables[table_idx] = (page_table_t*)kapmalloc(sizeof(page_table_t), (uint32_t)&phys);
	memset(dir->tables[table_idx], 0, sizeof(page_table_t));
	dir->tablesPhysical[table_idx] = phys | 0x7; // present, read-write, user-accessible

	/*
	console_print("  allocated page table for 0x");
	console_printNum(address, 16);
	console_print(" at 0x");
	console_printNum(phys, 16);
	console_print("\n");
	*/
	return &dir->tables[table_idx]->pages[page_index%1024];
    }else
	return 0;
}

void paging_allocFrame(page_t*page, bool kernel, bool writable){
    if(page->frame)	   // if nonzero, the page is already alloc'd.
	return;
    uint32_t frameIndex = getFreeFrame();
    if(frameIndex==0xffffffff)
	HCF("Could not allocate frame; no frames are available!");
    setFrame(frameIndex*PAGING_PAGE_SIZE, true); // CLAIM'D
    page->present = 1;
    page->rw      = writable?1:0;
    page->user    = kernel?0:1;
    page->frame   = frameIndex;
}

void paging_freeFrame(page_t*page){
    uint32_t frame = page->frame;
    if(!frame)
	return;			// frame was never allocated
    setFrame(frame,false);
    page->frame = 0;
}
