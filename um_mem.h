/*
 * um_mem.h
 * 
 * Written by: Megan Gelement (mgelem01) and Marshall Wilson (wwilso02)
 * Date: April 11, 2021
 * 
 * Purpose: Interface of UM memory management module. 
 * 
 */

#ifndef UM_MEM_H
#define UM_MEM_H

#include <seq.h>
#include <stdint.h>
#include <uarray.h>

typedef struct um_mem_t* um_mem_t;

/* allocates space for a new, empty um_mem_t */
um_mem_t um_mem_new();

/* frees all heap-allocated space associated with a um_mem_t */
void um_mem_free(um_mem_t memory);


/* creates a new segment in memory with the provided number of words */
unsigned map_segment(um_mem_t memory, unsigned length);

/* unmaps given segment */
void unmap_segment(um_mem_t memory, uint32_t seg_id);


/* sets the segment at the given id to be a provided uarray_t */
void set_segment(um_mem_t memory, uint32_t seg_id, UArray_T segment);

/* gets copy of given segment */
UArray_T get_segment_copy(um_mem_t memory, uint32_t seg_id);


/* sets the word in memory with the provided ids to the provided word */
void set_seg_value(um_mem_t memory, uint32_t seg_id, uint32_t word_id, 
                                                            uint32_t new_val);

/* returns the word stored at the provided indices in memory */                                                            
uint32_t get_seg_value(um_mem_t memory, uint32_t seg_id, uint32_t word_id);


#endif