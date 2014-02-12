#include "common.h"
#include "console.h"
#include "statusbar.h"
#include "heap.h"

size_t strlen(const char*str){
    size_t ret = 0;
    while(str[ret] != 0)
	ret++;
    return ret;
}

// strlen, but checks for color escape sequences
size_t strlenesc(const char*str){
    size_t ret = 0;
    for(size_t i=0; str[i]!=0; i++)
	// depending on lazy evaluation to verify we have room
	if(str[i] == '' && str[i+1]!=0 && str[i+2]!=0){
	    uint8_t fg = ctox(str[i+1]);
	    uint8_t bg = ctox(str[i+2]);
	    if(fg<16 && bg<16){
		i+=2;
	    }else
		ret+=3;
	}else
	    ret++;
    return ret;
}

void strncpy(char*s1, const char*s2, size_t n){
    for(size_t i=0; i<n; i++)
	s1[i] = s2[i];
}

void memset(void*address, uint8_t byte, size_t len){
    uint32_t base = (uint32_t)address;
    for(uint32_t addr = base; addr < base+len; addr++)
	*(uint8_t*)addr = byte;
}

uint8_t ctox(char c){
    if(c >= '0' && c <= '9')
	return c-'0';
    else if(c >= 'A' && c <= 'F')
	return c-'A'+10;
    else if(c >= 'a' && c <= 'f')
	return c-'a'+10;
    else
	return 0xFF;
}

void halt(void){
    statusbar_setRunningFlag(BLOCKING);
    asm("hlt");
    statusbar_setRunningFlag(RUNNING);
}

void hang(void){
    console_printInfo("40Forced halt\n");
    statusbar_setRunningFlag(HALTED);
    asm("jmp label_hang");
}

void outb(uint16_t port, uint8_t value){
    asm volatile ("outb %1, %0" : : "dN" (port), "a" (value));
}

uint8_t inb(uint16_t port){
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a" (ret) : "dN" (port));
    return ret;
}

uint16_t inw(uint16_t port){
    uint16_t ret;
    asm volatile("inw %1, %0" : "=a" (ret) : "dN" (port));
    return ret;
}

uint8_t byteBCDToSanity(uint8_t bcd){
    return (bcd/16)*10 + (bcd & 0xF);
}

uint32_t kmalloc  (uint32_t size){ return heap_allocate(size, false, 0); }
uint32_t kamalloc (uint32_t size){ return heap_allocate(size, true,  0); }
uint32_t kpmalloc (uint32_t size, uint32_t phys){
    return heap_allocate(size, false, phys);
}
uint32_t kapmalloc(uint32_t size, uint32_t phys){
    return heap_allocate(size, true,  phys);
}

// Prints CR0 in a readable fashion. For further information, see the AMD64 Architecture
// Programmer's Manual Volume 2: System Programming entry on CR0, beginning on page 42.
// You can find this entry on page 90 of this PDF:
// http://developer.amd.com/wordpress/media/2012/10/24593_APM_v21.pdf
void printCR0(void){
    uint32_t cr0 = __getCR0();
    console_print("[CR0:");
    if(cr0 & 1<<31) console_print(" PG"); // paging enabled
    if(cr0 & 1<<30) console_print(" CD"); // cache disabled
    if(cr0 & 1<<29) console_print(" NW"); // not writethrough
    if(cr0 & 1<<18) console_print(" AM"); // alignment mask
    if(cr0 & 1<<16) console_print(" WP"); // write protect
    if(cr0 & 1<< 5) console_print(" NE"); // numeric error
    if(cr0 & 1<< 4) console_print(" ET"); // extension type
    if(cr0 & 1<< 3) console_print(" TS"); // task switched
    if(cr0 & 1<< 2) console_print(" EM"); // emulation
    if(cr0 & 1<< 1) console_print(" MP"); // monitor coprocessor
    if(cr0 & 1<< 0) console_print(" PE"); // protection enabled
    console_print("]");
}
