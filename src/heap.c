#include "heap.h"
#include "common.h"

// _heapAddress should be wherever our kernel ends in physical memory
extern uint32_t _kernelEnd;		 // defined in linker.ld
uint32_t _heapAddress = (uint32_t)&_kernelEnd; //0x01000000;

uint32_t heap_getHeapAddress(void){ return _heapAddress; }

uint32_t heap_allocate(uint32_t size, bool align, uint32_t phys){
    if(align && (_heapAddress & 0xFFF)){ // if unaligned, align the memory
	_heapAddress += 0x1000; // each page is 0x1000 bytes
	_heapAddress &= 0xfffff000;
    }
    if(phys)
	*(uint32_t*)phys = _heapAddress;

    uint32_t addr = _heapAddress;
    _heapAddress += size;
    return addr;
}
