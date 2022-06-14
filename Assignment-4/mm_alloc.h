/*
 * mm_alloc.h
 *
 * Exports a clone of the interface documented in "man 3 malloc".
 */

#pragma once

#ifndef _malloc_H_
#define _malloc_H_

#include <stdlib.h>

void* mm_malloc(size_t size);
void* mm_realloc(void* ptr, size_t size);
void mm_free(void* ptr);


extern struct mem_block HeadPtr;

/* mem block struct */
typedef struct mem_block {
    size_t size; 
    struct mem_block *next; 
    struct mem_block *prev; 
    int empty_status; 
    void *b;       
 }mem_block;




#endif