#ifndef HEAP_H
#define HEAP_H

#include "common.h"
#include "ordarr.h"

#define HEAP_BASE_ADDR    0xC0000000
#define HEAP_INITIAL_SIZE 0x100000
#define HEAP_INDEX_SIZE   0x20000	// Length of ordarr heap index
#define HEAP_MAGIC_NUM    0x77777777	// MSB low; used for data
#define HEAP_MIN_SIZE     0x70000

typedef struct{
    uint32_t magic; // magic number; MSB set iff this is a hole
    uint32_t size;  // size of the block, including footer
} block_header_t;

typedef struct{
    uint32_t magic; // MSB low.
    block_header_t*header;
} block_footer_t;

// not using bitfields because padding makes it irrelevant here
typedef struct{
    ordarr_t index;
    uint32_t start_addr; // Start of allocated space.
    uint32_t end_addr;   // End of allocated space; may go up to max_addr.
    uint32_t max_addr;   // The maximum address the heap can be expanded to.
    bool supervisor;	 // Should additional requested pages be root-only?
    bool readonly;	 // Should additional requested pages be read-only?
} heap_t;

// from before dynamic allocation
uint32_t heap_allocate(uint32_t size, bool align, uint32_t phys);
uint32_t heap_getHeapAddress(void);

#endif
