/* um_operate.h 
 * by Megan Gelement (mgelem01) and Marshall Wilson (wwilso02)
 * last edited: 4/11/2021
 * 
 * Purpose: Contains the interface for creating and operating universal 
 *          machine instances 
 */

#ifndef UM_OPERATE_H
#define UM_OPERATE_H

#include <stdint.h>
#include <stdbool.h>
#include "um_mem.h"
#include <stdio.h>

typedef struct um_data_t* um_data_t;

/* creates a new, empty, heap-allocated UM instance */
um_data_t initialize_um();

/* reads program into a UM */
void read_um_program(FILE *program, um_data_t um, int num_words);

/* interprets the instruction pointed to by the program counter */
void read_instruction(um_data_t um);

/* returns whether the "halting" member is set to true */
bool is_halting(um_data_t um);

/* frees all heap allocated data associated with a um instance */
void free_um(um_data_t um);

#endif