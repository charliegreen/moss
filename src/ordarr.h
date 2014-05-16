#ifndef ORDARR_H
#define ORDARR_H

/* This is an interface for an ordered array that can store any 4-byte thing
 * (in particular, anything that can be cast to void*)
 */

#include "common.h"

typedef void* type_t;

// Less-than predicate. This indicates how to order items.
typedef bool (*ltpredicate_t)(type_t,type_t);

typedef struct{
    type_t*array;
    uint32_t size;
    uint32_t maxSize;
    ltpredicate_t lt;
} ordarr_t;

bool ordarr_defaultLTPredicate(type_t a, type_t b);

// Create an ordered array, and put it at addr. addr=NULL will allocate one.
ordarr_t ordarr_create(void*addr, uint32_t maxSize, ltpredicate_t lt);

void ordarr_destroy(ordarr_t*oa);		// Destroy oa
void ordarr_insert(ordarr_t*oa, type_t item);	// Add item to oa
void ordarr_remove(ordarr_t*oa, uint32_t i);	// Remove item at index i
type_t ordarr_lookup(ordarr_t*oa, uint32_t i);	// Get item at index i

#endif
