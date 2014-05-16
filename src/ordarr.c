#include "ordarr.h"
#include "common.h"

bool ordarr_defaultLTPredicate(type_t a, type_t b){    return a < b;	}

ordarr_t ordarr_create(void*addr, uint32_t maxSize, ltpredicate_t lt){
    ordarr_t ret;
    ret.array = addr==NULL ?
	(type_t*)kmalloc(maxSize*sizeof(type_t)) :
	(type_t*)addr;
    memset(ret.array, 0, maxSize*sizeof(type_t));
    ret.size = 0;
    ret.maxSize = maxSize;
    ret.lt = lt;
    return ret;
}

void ordarr_destroy(__UNUSED ordarr_t*oa){
    //TODO
}

void ordarr_insert(ordarr_t*oa, type_t item){
    ASSERT(oa->lt);
    uint32_t i;
    for(i=0; i<oa->size && oa->lt(oa->array[i], item); i++)
	;
    if(i==oa->size)		// add item at end of array
	oa->array[oa->size++] = item;
    else{
	type_t tmp = oa->array[i];
	oa->array[i] = item;
	while(i < oa->size){
	    i++;
	    type_t tmp2 = oa->array[i];
	    oa->array[i] = tmp;
	    tmp = tmp2;
	}
	oa->size++;
    }
}

void ordarr_remove(ordarr_t*oa, uint32_t i){
    for(; i<oa->size; i++)
	oa->array[i] = oa->array[i+1];
    oa->size--;
}

type_t ordarr_lookup(ordarr_t*oa, uint32_t i){
    ASSERT(i<oa->size);
    return oa->array[i];
}
