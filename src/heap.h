#ifndef HEAP_H
#define HEAP_H

#include "common.h"

// Note that we make the assumption that nothing will ever need to be freed.

uint32_t heap_allocate(uint32_t size, bool align, uint32_t phys);
uint32_t heap_getHeapAddress(void);

#endif
