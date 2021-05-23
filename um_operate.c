/* um_operate.c 
 * by Megan Gelement (mgelem01) and Marshall Wilson (wwilso02)
 * last edited: 4/11/2021
 * 
 * Purpose: Contains the implementation of functions for creating and 
 *      interacting with universal machine instances.
 */


#include "um_operate.h"
#include <math.h>
#include <mem.h>
#include <bitpack.h> 
#include <assert.h>

/* struct um_data_t 
 * Purpose:     stores the data for a UM instance
 * Members:     uint32_t regs[8]: array holding the 8 registers for the machine
 *              uint32_t program_counter: holds the word index of the next 
 *                  instruction to be read in segment 0
 *              um_mem_t memory: the memory storage for the UM instance 
 *              bool halting: true iff the halt instruction has been executed
 */
struct um_data_t {
    uint32_t    regs[8];
    uint32_t    program_counter;
    um_mem_t    memory;
    bool        halting;
};


/* instruction functions 0-13 */
void mov(um_data_t um, uint32_t inst);
void seg_load(um_data_t um, uint32_t inst);
void seg_store(um_data_t um, uint32_t inst);
void add(um_data_t um, uint32_t inst);
void mult(um_data_t um, uint32_t inst);
void div(um_data_t um, uint32_t inst);
void nand(um_data_t um, uint32_t inst);
void halt(um_data_t um, uint32_t inst);
void map_seg(um_data_t um, uint32_t inst);
void unmap_seg(um_data_t um, uint32_t inst);
void output(um_data_t um, uint32_t inst);
void input(um_data_t um, uint32_t inst);
void load_prog(um_data_t um, uint32_t inst);
void load_val(um_data_t um, uint32_t inst);

/* Array of instruction function pointers */
void (*instructions[14])() = { mov, seg_load, seg_store, add, mult,
                                     div, nand, halt, map_seg, unmap_seg, 
                                     output, input, load_prog, load_val };
/* helper functions */
uint32_t read_word(FILE *fp);
void get_abc(uint32_t inst, uint32_t *regs);


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
|                    um_data_t Functions                     *|
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* initialize_um
 * Purpose:     Initializes a new, empty UM instance 
 * Parameters:  None
 * Returns:     um_data_t: the newly initialized, heap-allocated um_data_t
 * Notes:       None 
 */
um_data_t initialize_um()
{
    um_data_t um = ALLOC(sizeof(struct um_data_t));

    /* all registers are initialized to 0 */
    for (int i = 0; i < 8; i++) {
        um->regs[i] = 0;
    }

    /* initialize program counter */
    um->program_counter = 0;
    
    /* initialize memory struct */
    um->memory = um_mem_new();

    um->halting = false;

    return um;
}


/* is_halting
 * Purpose:     Get "halting" boolean from um_data_t struct
 * Parameters:  um_data_t um: UM struct from which to retrieve boolean
 * Returns:     bool: "halting" boolean from given UM struct
 * Notes:       Does not modify UM
 */
bool is_halting(um_data_t um)
{
    return um->halting;
}


/* free_um
 * Purpose:     Frees data associated with a given UM instance
 * Parameters:  um_data_t um: UM struct to free
 * Returns:     None.
 * Notes:       Relies on um_mem_free to free the memory associated with the 
 *                  UM
 */
void free_um(um_data_t um)
{
    assert(um != NULL);
    um_mem_free(um->memory);
    FREE(um);
}


/* read_um_program
 * Purpose:     Read a program into a UM.
 * Parameters:  FILE *program: File pointer to program to read
 *              um_data_t um:  UM into which to read program
 *              int num_words: number of words in provided program
 * Returns:     None
 * Notes:       Assumes "program" file is open.
 */
void read_um_program(FILE *program, um_data_t um, int num_words)
{
    map_segment(um->memory, num_words);

    uint32_t word;
    for (int i = 0; i < num_words; i++) {
        word = read_word(program);
        set_seg_value(um->memory, 0, i, word);
    }
}


/* read_word
 * Purpose:     Reads and bitpacks a word in big-endian order from a file
 * Parameters:  FILE *fp: File pointer to open file from which to read a word
 * Returns:     uint32_t: Bitpacked word read in
 * Notes:       Assumes "fp" file is open.
 */
uint32_t read_word(FILE *fp)
{
    uint32_t word;
    uint32_t c;

    for (int i = 3; i >= 0; i--) {
        c = getc(fp); 
        word = Bitpack_newu(word, 8, 8 * i, c);
    }

    return word; 
}


/* read_instruction
 * Purpose:     Reads the instruction indicated by the program counter, 
 *                  increments the program counter, and then executes the 
 *                  instruction that was just read 
 * Parameters:  um_data_t um: the um instance where the instruction is 
 * Returns:     None
 * Notes:       it is a URE to call this when the program counter is not set 
 *                  to a valid index in segment 0 (the instruction segment)
 */
void read_instruction(um_data_t um)
{
    uint32_t instruction = get_seg_value(um->memory, 0, um->program_counter);
    uint32_t opcode = Bitpack_getu(instruction, 4, 28);

    um->program_counter++;

    instructions[opcode](um, instruction);
}


/* get_abc
 * Purpose:     Gets registers A, B, and C from a provided instruction
 * Parameters:  uint32_t instruction: instruction from which to retrieve 
 *                                      registers
 *              uint32_t *abc:        struct of uint32_ts in which to store
 *                                      register values
 * Returns:     None
 * Notes:       TODO?
 */
void get_abc(uint32_t inst, uint32_t *abc)
{
    abc[0] = Bitpack_getu(inst, 3, 6);
    abc[1] = Bitpack_getu(inst, 3, 3);
    abc[2] = Bitpack_getu(inst, 3, 0);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
|                    Instruction Functions                   *|
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* mov
 * Purpose:     Conditional move instruction; if $r[C] != 0, $r[A] = $r[B]
 * Parameters:  um_data_t um: the UM where the data is stored
 *              uint32_t inst: instruction holding register indices
 * Returns:     None
 * Notes:       None
 */
void mov(um_data_t um, uint32_t inst)
{
    uint32_t abc[3];
    get_abc(inst, abc);

    if (um->regs[abc[2]] != 0) {
        um->regs[abc[0]] = um->regs[abc[1]];
    }
}


/* seg_load
 * Purpose:     Loads the value at $m[$r[B]][$r[C]] into $r[A]
 * Parameters:  um_data_t um: the UM where the data is stored
 *              uint32_t inst: the instruction holding the register indices
 * Returns:     None 
 * Notes:       It is a URE for $m[$r[B]][$r[C]] to not indicate a valid word 
 *                 in a mapped segment
 */
void seg_load(um_data_t um, uint32_t inst)
{
    uint32_t abc[3];
    get_abc(inst, abc);
    um->regs[abc[0]] = get_seg_value(um->memory, 
                                        um->regs[abc[1]], um->regs[abc[2]]);
}


/* seg_store
 * Purpose:     Loads the value at $r[C] into $m[$r[A]][$r[B]]
 * Parameters:  um_data_t um: the UM where the data is stored
 *              uint32_t inst: the instruction holding the register indices
 * Returns:     None 
 * Notes:       It is a URE for $m[$r[A]][$r[B]] to not indicate a valid word 
 *                 in a mapped segment
 */
void seg_store(um_data_t um, uint32_t inst)
{
    uint32_t abc[3];
    get_abc(inst, abc);
    set_seg_value(um->memory, um->regs[abc[0]], 
                  um->regs[abc[1]], um->regs[abc[2]]);
}


/* halt
 * Purpose:     Sets the 'halting' boolean to true 
 * Parameters:  um_data_t um: the UM where the data is stored
 *              uint32_t inst: the instruction which called halt
 * Returns:     None 
 * Notes:       None 
 */
void halt(um_data_t um, uint32_t inst)
{
    um->halting = true;
    (void) inst;
}


/* output
 * Purpose:     Outputs value in $r[C]
 * Parameters:  um_data_t um: the UM where the data is stored
 *              uint32_t inst: the instruction holding the register index
 * Returns:     None
 * Notes:      It is a URE for the value in $r[C] to be less than 0 or greater
 *                  than 255 
 */
void output(um_data_t um, uint32_t inst)
{
    uint32_t abc[3];
    get_abc(inst, abc);
    putc(um->regs[abc[2]], stdout);
}


/* input
 * Purpose:     Loads a single char from input into $r[C]
 * Parameters:  um_data_t um: the UM where the data is stored
 *              uint32_t inst: the instruction holding the register indices
 * Returns:     None 
 * Notes:       It is a URE if input is not in the range 0 to 255
 *              If the end of input has been signalled, $r[C] is loaded with 
 *                  a 32-bit word of all 1's
 */
void input(um_data_t um, uint32_t inst)
{
    uint32_t abc[3];
    get_abc(inst, abc);
    um->regs[abc[2]] = getc(stdin);

    if (feof(stdin) != 0) {
        um->regs[abc[2]] = ~(0); 
    }
}


/* map_seg
 * Purpose:     Creates a new segment with a number of words equal to the 
 *                  value at the given instruction's $r[C]; initializes each 
 *                  word in the new segment to 0; sets $r[B]'s segment ID to an
 *                  available value and maps the new segment to $m[$r[B]].
 * Parameters:  um_data_t um:  the UM where the data is stored
 *              uint32_t inst: the instruction holding the register indices
 * Returns:     None
 * Notes:       None
 */
void map_seg(um_data_t um, uint32_t inst)
{
    uint32_t abc[3];
    get_abc(inst, abc);
    um->regs[abc[1]] = map_segment(um->memory, um->regs[abc[2]]);
}


/* unmap_seg
 * Purpose:     Unmapps segment $m[$r[C]] 
 * Parameters:  um_data_t um: the UM where the data is stored
 *              uint32_t inst: the instruction holding the register indices
 * Returns:     None 
 * Notes:       It is a URE for $m[$r[C]] to indicate an unmapped segment
 */
void unmap_seg(um_data_t um, uint32_t inst)
{
    uint32_t abc[3];
    get_abc(inst, abc);
    unmap_segment(um->memory, um->regs[abc[2]]);
}


/* load_prog
 * Purpose:     Duplicates segment $m[$r[B]] and replaces $m[0] with the 
                    duplicate. Sets the program counter to $r[C]
 * Parameters:  um_data_t um: the UM where the data is stored
 *              uint32_t inst: the instruction holding the register indices
 * Returns:     None 
 * Notes:       It is a URE for $m[$r[B]] to point to an unmapped segment 
 */
void load_prog(um_data_t um, uint32_t inst)
{
    uint32_t abc[3];
    get_abc(inst, abc);
    
    um->program_counter = um->regs[abc[2]];

    if (um->regs[abc[1]] == 0) {
        return;
    }
    
    UArray_T seg_copy = get_segment_copy(um->memory, um->regs[abc[1]]);
    set_segment(um->memory, 0, seg_copy);

}


/* load_val
 * Purpose:     Sets $r[A] to the value stored in bits 0-24 of the instruction
 * Parameters:  um_data_t um: the UM where the data is stored
 *              uint32_t inst: the instruction holding the register indices
 * Returns:     None 
 * Notes:       None 
 */
void load_val(um_data_t um, uint32_t inst)
{
    uint32_t a = Bitpack_getu(inst, 3, 25);
    uint32_t val = Bitpack_getu(inst, 25, 0);
    um->regs[a] = val;
}


/* add
 * Purpose:     Adds the values at $r[B] and $r[C] mod 2^32 and stores the 
                    result in $r[A].
 * Parameters:  um_data_t um:  UM where the data is stored
 *              uint32_t inst: the instruction holding the register indices
 * Returns:     None
 * Notes:       If the result of the addition > 2^32, the result stored in 
 *                  $r[A] is modded by 2^32
 */
void add(um_data_t um, uint32_t inst)
{
    uint32_t abc[3];
    get_abc(inst, abc);

    um->regs[abc[0]] = (um->regs[abc[1]] + um->regs[abc[2]]);
}


/* mult
 * Purpose:     sets $r[A] to ($r[B] * $r[C]) mod 2^32
 * Parameters:  um_data_t um: the UM where the data is stored
 *              uint32_t inst: the instruction holding the register indices
 * Returns:     None 
 * Notes:       None 
 */
void mult(um_data_t um, uint32_t inst)
{
    uint32_t abc[3];
    get_abc(inst, abc);

    um->regs[abc[0]] = (um->regs[abc[1]] * um->regs[abc[2]]);
}


/* div
 * Purpose:     Sets $r[A] = $r[B]/$r[C] (rounded down to the nearest int)
 * Parameters:  um_data_t um: the UM where the data is stored
 *              uint32_t inst: the instruction holding the register indices
 * Returns:     None
 * Notes:       It is a URE for $r[C] to equal 0
 */
void div(um_data_t um, uint32_t inst)
{
    uint32_t abc[3];
    get_abc(inst, abc);

    um->regs[abc[0]] = (um->regs[abc[1]] / um->regs[abc[2]]);
}


/*
 * Purpose:     Performs a bitwise nand operation on $r[B] and $r[C]; stores
                    the result in $r[A].
 * Parameters:  um_data_t um: the UM where the data is stored
 *              uint32_t inst: the instruction holding the register indices
 * Returns:     None
 * Notes:       None
 */
void nand(um_data_t um, uint32_t inst)
{
    uint32_t abc[3];
    get_abc(inst, abc);

    um->regs[abc[0]] = ~(um->regs[abc[1]] & um->regs[abc[2]]);
}

