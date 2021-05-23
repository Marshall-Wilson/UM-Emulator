/*
 * um_mem.c
 * 
 * Written by: Megan Gelement (mgelem01) and Marshall Wilson (wwilso02)
 * Date: April 11, 2021
 * 
 * Purpose: Implementation of UM memory management module. 
 */
 

 #include "um_mem.h"
 #include <seq.h>
 #include <uarray.h> 
 #include <stdint.h>
 #include <stdlib.h>
 #include <mem.h>
 #include <stdio.h>
 #include <assert.h>

void fill_seg(UArray_T seg);


/* struct um_mem_t
 * Purpose:     Holds important data for the memory managment of a um instance
 * Members:     Seq_T segment_list: a sequence which holds the UM's segments
 *              Seq_T avail_ids: a sequence of currently available segment IDs
 *                  The last seq element is next ID after the highest ID that 
 *                  has already been mapped
 */
struct um_mem_t {
    Seq_T segment_list;
    Seq_T avail_ids;
};


/* um_mem_new
 * Purpose:     Creates and returns an empty heap-allocated um_mem_t 
 * Parameters:  None
 * Returns:     um_mem_t the newly allocated struct 
 * Notes:       Client is responsible for calling um_mem_free to free memory
 */
um_mem_t um_mem_new()
{
    um_mem_t new_mem = ALLOC(sizeof(struct um_mem_t));
    new_mem->segment_list = Seq_new(100);
    new_mem->avail_ids = Seq_new(100);
    return new_mem;
}


/* map_segment
 * Purpose:     Creates a new segment of the desired length in memory, 
 *                  initializes its values to 0, and returns its ID
 * Parameters:  um_mem_t memory: the memory where the segment should be added
 *              int length: the number of 32-bit words the segment should hold
 * Returns:     unsigned: the ID of the newly created segment
 */
unsigned map_segment(um_mem_t memory, unsigned length) 
{
    assert(memory != NULL);

    unsigned index;

    /* first segment creation */
    if (Seq_length(memory->segment_list) == 0) {
        Seq_addhi(memory->segment_list, UArray_new(length, sizeof(uint32_t)));
        Seq_addhi(memory->avail_ids, (void *)(uintptr_t)1);
        index = 0;
    
    /* no recycled ids */
    } else if(Seq_length(memory->avail_ids) == 1) {
        Seq_addhi(memory->segment_list, UArray_new(length, sizeof(uint32_t)));
        index = (unsigned)(uintptr_t)Seq_remlo(memory->avail_ids);
        Seq_addhi(memory->avail_ids, (void *)(uintptr_t)(index + 1));

    /* using previously mapped segment */
    } else {
        index = (unsigned)(uintptr_t)Seq_remlo(memory->avail_ids);
        Seq_put(memory->segment_list, index, UArray_new(length, sizeof(uint32_t)));
    }

    fill_seg(Seq_get(memory->segment_list, index));

    return index;
}


/* set_seg_value
 * Purpose:     Sets the word indicated by a segment and word ID to a value
 * Parameters:  um_mem_t memory: The um memory where the word should be stored
 *              uint32_t seg_id: the ID of the segment where the word should
 *                                  be stored
 *              uint32_t word_id: the ID of the word within the specified seg
 *              uint32_t new_val: the value to be stored in that word
 * Returns:     None
 * Notes:       It is a URE to provide a seg_id that does not identify a 
 *                  currently mapped segment or to provide a word_id that 
 *                  is beyond the length of the specified segment. 
 *              It is a CRE for memory to be NULL
 */
void set_seg_value(um_mem_t memory, uint32_t seg_id, uint32_t word_id, 
                   uint32_t new_val)
{
    assert(memory != NULL);
    *(uint32_t *)UArray_at(Seq_get(memory->segment_list, seg_id), word_id) = 
    new_val;
}


/* get_seg_value
 * Purpose:     Retrieves value stored in a given segment.
 * Parameters:  um_mem_t memory:  Struct containing all memory management/
 *                                location data for the UM
 *              uint32_t seg_id:  Segment ID in which the desired word (value)
 *                                is located
 *              uint32_t word_id: Offset of word to retrieve
 * Returns:     uint32_t:         value at specified location [seg_id][word_id]
 * Notes:       Does not modify segment value. 
 *              It is a URE to provide a seg_id that does not identify a 
 *                  currently-mapped segment, or to provide a word_id that is 
 *                  beyond the length of the specified segment.
 *              It is a CRE for memory to be NULL.
 */
uint32_t get_seg_value(um_mem_t memory, uint32_t seg_id, uint32_t word_id)
{
    assert(memory != NULL);
    return *(uint32_t *)UArray_at(Seq_get(memory->segment_list, seg_id), 
                                          word_id);
}


/* unmap_segment
 * Purpose:     Frees memory associated with a given segment and adds its id 
 *              to the list of available ids for eventual re-mapping
 * Parameters:  um_mem_t memory: Struct containing all memory management data
 *                               for the UM
 *              uint32_t seg_id: ID of segment to unmap
 * Returns:     None
 * Notes:       Cannot unmap Segment 0. 
 *              It is a CRE for memory to be NULL. 
 */
void unmap_segment(um_mem_t memory, uint32_t seg_id)
{
    assert(memory != NULL);

    /* free memory associated with the given segment */
    UArray_T seg = (UArray_T)Seq_get(memory->segment_list, seg_id);
    UArray_free(&seg);
    Seq_put(memory->segment_list, seg_id, NULL);

    /* Add freed segment id to list of available ids */
    Seq_addlo(memory->avail_ids, (void *)(uintptr_t)seg_id);
}


/* um_mem_free
 * Purpose:    Frees all memory associated with UM memory management
 * Parameters: um_mem_t memory: Struct containing UM memory data 
 * Returns:    None
 * Notes:      It is a CRE for memory to be NULL. 
 */
void um_mem_free(um_mem_t memory)
{
    assert(memory != NULL);
    
    int length = Seq_length(memory->segment_list);
    
    for (int i = 0; i < length; i++) {
        
        UArray_T seg = (UArray_T)Seq_remlo(memory->segment_list);
        
        /* Previously-unmapped segments should not be freed again */
        if (seg != NULL){
            UArray_free(&seg);
        }
    }

    Seq_free(&(memory->segment_list));
    Seq_free(&(memory->avail_ids));
    FREE(memory);
}


/* fill_seg
 * Purpose:    Initializes new segments to 0
 * Parameters: UArray_T seg: Current segment to initialize
 * Returns:    None
 * Notes:      Helper function for map_segment
 *             It is a CRE for seg to be NULL. 
 */
void fill_seg(UArray_T seg)
{
    assert(seg != NULL);

    uint32_t *elmt;
    for (int i = 0; i < UArray_length(seg); i++){
        elmt = UArray_at(seg, i);
        *elmt = 0;
    }
}


/* get_segment_copy
 * Purpose:     Copies the segment at a given segment id
 * Parameters:  um_mem_t memory: struct containing UM memory data
 *              uint32_t seg_id: ID of segment to be copied
 * Returns:     UArray_T:        copy segment at seg_id
 * Notes:       Does not modify existing segments.        
 */
UArray_T get_segment_copy(um_mem_t memory, uint32_t seg_id)
{
    UArray_T source_seg = Seq_get(memory->segment_list, seg_id);
    int length = UArray_length(source_seg);
    UArray_T dest_seg = UArray_new(length, sizeof(uint32_t));

    uint32_t *elem_ptr;

    for (int i = 0; i < length; i++) {
        elem_ptr = UArray_at(dest_seg, i);
        *elem_ptr = *(uint32_t *)(UArray_at(source_seg, i)); 
    }

    return dest_seg;
}


/* set_segment
 * Purpose:     Frees the UArray stored at a currently mapped segment and 
 *                  replaces it with a provided UArray 
 * Parameters:  um_mem_t memory: the memory containing the segment to be 
 *                  changed
 *              uint32_t seg_id: the index of the segment to be changed 
 *              UArray_T segment: the UArray to be inserted at that index 
 * Returns:     None
 * Notes:       It is a URE for seg_id to identify an unmapped segment    
 */
void set_segment(um_mem_t memory, uint32_t seg_id, UArray_T segment)
{
    UArray_T seg = (UArray_T)Seq_get(memory->segment_list, seg_id);
    UArray_free(&seg);
    Seq_put(memory->segment_list, seg_id, segment);
}
