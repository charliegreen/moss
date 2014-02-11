#ifndef PAGING_H
#define PAGING_H

#include "common.h"

// Note that we have to specify int here because the compiler spews about
// uint32_t being a GCC extension and errors if there is no type
typedef struct{
    int present  : 1;   // Page present in memory
    int rw       : 1;   // Read-only if clear, read/write if set
    int user     : 1;   // Supervisor level only if clear
    int accessed : 1;   // Has page been accessed since last refresh?
    int dirty    : 1;   // Has page been written to since last refresh?
    int unused   : 7;   // Amalgamation of unused and reserved bits
    int frame    : 20;  // Frame address (shifted right 12 bits)
} page_t;

typedef struct{
    page_t pages[1024];
} page_table_t;

typedef struct{
    page_table_t*tables[1024]; // array of page table pointers

    // Array of pointers to the pagetables above, but gives their *physical*
    // location, for loading into the CR3 register.
    uint32_t tablesPhysical[1024];

    //uint32_t physicalAddr;  // The physical address of tablesPhysical.
} page_directory_t;

void paging_initialize(void);
page_t*paging_getPage(uint32_t address, bool make, page_directory_t*dir);
void paging_switchPageDirectory(page_directory_t *dir);

void paging_allocFrame(page_t*page, bool kernel, bool writable);
void paging_freeFrame(page_t*page);

#endif
