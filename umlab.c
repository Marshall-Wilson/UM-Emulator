/*
 * umlab.c
 *
 * Functions to generate UM unit tests. Once complete, this module
 * should be augmented and then linked against umlabwrite.c to produce
 * a unit test writing program.
 *  
 * A unit test is a stream of UM instructions, represented as a Hanson
 * Seq_T of 32-bit words adhering to the UM's instruction format.  
 * 
 * Any additional functions and unit tests written for the lab go
 * here. 
 *  
 */


#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <seq.h>
#include <bitpack.h>
#include <math.h>


typedef uint32_t Um_instruction;
typedef enum Um_opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, ACTIVATE, INACTIVATE, OUT, IN, LOADP, LV
} Um_opcode;


/* Functions that return the two instruction types */

Um_instruction three_register(Um_opcode op, int ra, int rb, int rc);
Um_instruction loadval(unsigned ra, unsigned val);

Um_instruction three_register(Um_opcode op, int ra, int rb, int rc)
{
        Um_instruction instruction = 0;
        
        instruction = Bitpack_newu(instruction, 4, 28, op);
        instruction = Bitpack_newu(instruction, 3, 6, ra);
        instruction = Bitpack_newu(instruction, 3, 3, rb);
        instruction = Bitpack_newu(instruction, 3, 0, rc);

        return instruction; 
}


Um_instruction loadval(unsigned ra, unsigned val)
{
        Um_instruction instruction = 0; 

        instruction = Bitpack_newu(instruction, 4, 28, LV); 
        instruction = Bitpack_newu(instruction, 3, 25, ra);
        instruction = Bitpack_newu(instruction, 25, 0, val);

        return instruction; 
}


/* Wrapper functions for each of the instructions */
typedef enum Um_register { r0 = 0, r1, r2, r3, r4, r5, r6, r7 } Um_register;


static inline Um_instruction mov(Um_register a, Um_register b, Um_register c) 
{
        return three_register(CMOV, a, b, c);
}

static inline Um_instruction segload(Um_register a, Um_register b, 
                                                                Um_register c)
{
        return three_register(SLOAD, a, b, c);
}

static inline Um_instruction segstore(Um_register a, Um_register b, 
                                                                Um_register c)
{
        return three_register(SSTORE, a, b, c);
}

static inline Um_instruction add(Um_register a, Um_register b, Um_register c) 
{
        return three_register(ADD, a, b, c);
}

static inline Um_instruction mult(Um_register a, Um_register b, Um_register c)
{
        return three_register(MUL, a, b, c);
}

static inline Um_instruction div(Um_register a, Um_register b, Um_register c)
{
        return three_register(DIV, a, b, c);
}

static inline Um_instruction nand(Um_register a, Um_register b, Um_register c)
{
        return three_register(NAND, a, b, c);
}

static inline Um_instruction halt(void) 
{
        return three_register(HALT, 0, 0, 0);
}

static inline Um_instruction map(Um_register b, Um_register c)
{
        return three_register(ACTIVATE, 0, b, c);
}

static inline Um_instruction unmap(Um_register c)
{
        return three_register(INACTIVATE, 0, 0, c);
}

static inline Um_instruction output(Um_register c)
{
        return three_register(OUT, 0, 0, c);
}

static inline Um_instruction input(Um_register c)
{
        return three_register(IN, 0, 0, c);
}

static inline Um_instruction prog(Um_register b, Um_register c)
{
        return three_register(LOADP, 0, b, c);
}


/* Functions for working with streams */

static inline void append(Seq_T stream, Um_instruction inst)
{
        assert(sizeof(inst) <= sizeof(uintptr_t));
        Seq_addhi(stream, (void *)(uintptr_t)inst);
}

const uint32_t Um_word_width = 32;

void Um_write_sequence(FILE *output, Seq_T stream)
{
        assert(output != NULL && stream != NULL);
        int stream_length = Seq_length(stream);
        for (int i = 0; i < stream_length; i++) {
                Um_instruction inst = (uintptr_t)Seq_remlo(stream);
                for (int lsb = Um_word_width - 8; lsb >= 0; lsb -= 8) {
                        fputc(Bitpack_getu(inst, 8, lsb), output);
                }
        }
      
}


/* Unit tests for the UM */

void build_halt_test(Seq_T stream)
{
        append(stream, halt());
}

void build_verbose_halt_test(Seq_T stream)
{
        append(stream, halt());
        append(stream, loadval(r1, 'B'));
        append(stream, output(r1));
        append(stream, loadval(r1, 'a'));
        append(stream, output(r1));
        append(stream, loadval(r1, 'd'));
        append(stream, output(r1));
        append(stream, loadval(r1, '!'));
        append(stream, output(r1));
        append(stream, loadval(r1, '\n'));
        append(stream, output(r1));
}

void build_add_test(Seq_T stream)
{
        append(stream, add(r1, r2, r3));
        append(stream, halt());
}

void build_print6_test(Seq_T stream)
{
        append(stream, loadval(r1, 48));
        append(stream, loadval(r2, 6));
        append(stream, add(r3, r1, r2));
        append(stream, output(r3));
        append(stream, halt());
}

void build_load_print_test(Seq_T stream)
{
        append(stream, loadval(0, 48));
        append(stream, output(0));
        append(stream, loadval(r1, 49));
        append(stream, output(r1));       
        append(stream, loadval(r2, 50));
        append(stream, output(r2));
        append(stream, loadval(r3, 51));
        append(stream, output(r3));
        append(stream, loadval(r4, 52));
        append(stream, output(r4));
        append(stream, loadval(r5, 53));
        append(stream, output(r5));
        append(stream, loadval(r6, 54));
        append(stream, output(r6));
        append(stream, loadval(r7, 55));
        append(stream, output(r7));
        append(stream, halt());
}


void build_hello_test(Seq_T stream)
{
        append(stream, loadval(0, 72));
        append(stream, output(0));
        append(stream, loadval(0, 101));
        append(stream, output(0));
        append(stream, loadval(0, 108));
        append(stream, output(0));
        append(stream, loadval(0, 108));
        append(stream, output(0));
        append(stream, loadval(0, 111));
        append(stream, output(0));
        append(stream, loadval(0, 32));
        append(stream, output(0));
        append(stream, loadval(0, 87));
        append(stream, output(0));
        append(stream, loadval(0, 111));
        append(stream, output(0));
        append(stream, loadval(0, 114));
        append(stream, output(0));
        append(stream, loadval(0, 108));
        append(stream, output(0));
        append(stream, loadval(0, 100));
        append(stream, output(0));
        append(stream, loadval(0, 33));
        append(stream, output(0));
        append(stream, loadval(0, 10));
        append(stream, output(0));
        append(stream, halt());
}


void build_mult_test(Seq_T stream)
{
        // 2 * 3 = 6
        append(stream, loadval(0, 48));
        append(stream, loadval(r1, 2));
        append(stream, loadval(r2, 3));
        append(stream, mult(r3, r1, r2));
        append(stream, add(r3, r3, 0));
        append(stream, output(r3));

        // exceed limit by 64 (@ character)
        append(stream, loadval(r1, pow(2, 24)));
        append(stream, loadval(r2, pow(2, 14)));
        append(stream, mult(0, r1, r2));
        append(stream, loadval(r2, 1));
        append(stream, add(0, 0, r2));
        append(stream, loadval(r4, 64));
        append(stream, mult(0, 0, r4));
        append(stream, output(0));

        // exceed by 65 (A)
        append(stream, loadval(r1, 67649));
        append(stream, loadval(r2, 63489));
        append(stream, mult(0, r1, r2));
        append(stream, output(0));
        append(stream, halt());

}

void build_add_limit_test(Seq_T stream)
{
        append(stream, loadval(r1, pow(2, 24)));
        append(stream, loadval(r2, pow(2, 8)));
        append(stream, mult(0, r1, r2));
        append(stream, loadval(r3, 65));
        append(stream, add(r1, r3, 0));
        append(stream, output(r1));
        append(stream, halt());
}


void build_div_test(Seq_T stream)
{
        /* Numbers evenly divide */
        append(stream, loadval(r2, 570));
        append(stream, loadval(r3, 10));
        append(stream, div(r1, r2, r3));
        append(stream, output(r1));
        
        /* Numbers don't evenly divide  */
        append(stream, loadval(r2, 573));
        append(stream, loadval(r3, 10));
        append(stream, div(r1, r2, r3));
        append(stream, output(r1));

        /* Div by 1 */
        append(stream, loadval(r1, 1));
        append(stream, loadval(r2, 54)); //6
        append(stream, div(r1, r2, r1));
        append(stream, output(r1));

        /* Div 0 by others */
        append(stream, loadval(r1, 0));
        append(stream, loadval(r2, 48));
        append(stream, loadval(r3, 49893));
        append(stream, div(r1, r1, r3));
        append(stream, add(r1, r1, r2));
        append(stream, output(r1));


        append(stream, halt());
}


void build_nand_test(Seq_T stream)
{
        append(stream, loadval(r2, 65));
        append(stream, loadval(r3, 65));
        append(stream, nand(r1, r2, r3));
        append(stream, nand(r1, r1, r1));
        append(stream, output(r1));


        append(stream, loadval(r2, 126));
        append(stream, loadval(r3, 67));
        append(stream, nand(r1, r2, r3)); // = ~66
        append(stream, nand(r1, r1, r1)); // = ~(~66 & ~66) = 66
        append(stream, output(r1));

        append(stream, halt());
}


void build_load_prog_test(Seq_T stream)
{
        append(stream, loadval(r1, 3));
        append(stream, prog(0, r1));
        append(stream, halt());
        append(stream, loadval(r1, pow(2, 24)));
        append(stream, loadval(r2, pow(2, 4)));
        append(stream, loadval(r3, 10));
        append(stream, loadval(r4, r5));
        append(stream, loadval(r5, 65));
        append(stream, loadval(r6, 7));
        append(stream, map(r7, 2)); // map new segment
        append(stream, loadval(0, 0));
        append(stream, mult(r3, r3, r1)); //10 * 2^24
        append(stream, mult(r3, r3, r2)); //10 * 2^24 * 2^4
        append(stream, add(r3, r3, r4)); //10 * 2^24 * 2^4 + 5
        append(stream, segstore(r7, 0, r3));
        append(stream, loadval(0, 1));
        append(stream, mult(r6, r6, r1)); //7 * 2^24
        append(stream, mult(r6, r6, r2)); //7 * 2^24 * 2^4
        append(stream, segstore(r7, 0, r6));
        append(stream, loadval(0, 0));
        append(stream, prog(r7, 0));
}


void build_mov_test(Seq_T stream)
{
        append(stream, loadval(r1, 65));
        append(stream, loadval(r2, 66));
        append(stream, loadval(r3, 0));
        append(stream, mov(r1, r2, r3));
        append(stream, output(r1)); // output should be A
        append(stream, loadval(r4, 7));
        append(stream, mov(r1, r2, r4));
        append(stream, output(r1)); // output should be B
        append(stream, halt());       
}

void build_div_0_test(Seq_T stream)
{
        append(stream, loadval(r1, 200));
        append(stream, div(0, r1, r2));
        append(stream, output(0));
        append(stream, halt());
}

void build_map_unmap_test(Seq_T stream)
{
        append(stream, loadval(0, 20));
        append(stream, loadval(r1, 48));
        append(stream, loadval(r2, 32));

        append(stream, map(r3, 0));
        append(stream, add(r6, r3, r1));
        append(stream, output(r6));
        append(stream, output(r2));

        append(stream, map(r4, 0));
        append(stream, add(r6, r4, r1));
        append(stream, output(r6));
        append(stream, output(r2));
        
        append(stream, map(r5, 0));
        append(stream, add(r6, r5, r1));
        append(stream, output(r6));
        append(stream, output(r2));
        
        append(stream, unmap(r3));
        append(stream, unmap(r4));
        append(stream, map(r3, 0));
        append(stream, add(r6, r3, r1));
        append(stream, output(r6));
        append(stream, output(r2));
        
        append(stream, map(r4, 0));
        append(stream, add(r6, r4, r1));
        append(stream, output(r6));
        append(stream, output(r2));
        
        append(stream, map(r5, 0));
        append(stream, add(r6, r5, r1));
        append(stream, output(r6));
        append(stream, halt());
}

void build_unmap_fail(Seq_T stream)
{
        append(stream, map(r1, 0));
        append(stream, unmap(r1));
        append(stream, loadval(r2, 48));
        append(stream, add(r3, r2, r1));
        append(stream, output(r3));
        append(stream, unmap(r1));
}

void build_input_test(Seq_T stream)
{
        append(stream, input(r1));  
        append(stream, output(r1));
        append(stream, halt());
}


void build_segloadstore_test(Seq_T stream)
{
        append(stream, loadval(0, 72));
        append(stream, map(r1, 0));
        append(stream, loadval(r2, 0));
        append(stream, loadval(r3, 1));
        append(stream, segstore(r1, r2, 0));
        append(stream, loadval(0, 101));
        append(stream, add(r2, r2, r3));
        append(stream, segstore(r1, r2, 0));
        append(stream, loadval(0, 108));
        append(stream, add(r2, r2, r3));
        append(stream, segstore(r1, r2, 0));
        append(stream, loadval(0, 108));
        append(stream, add(r2, r2, r3));
        append(stream, segstore(r1, r2, 0));
        append(stream, loadval(0, 111));
        append(stream, add(r2, r2, r3));
        append(stream, segstore(r1, r2, 0));
        append(stream, loadval(0, 32));
        append(stream, add(r2, r2, r3));
        append(stream, segstore(r1, r2, 0));
        append(stream, loadval(0, 87));
        append(stream, add(r2, r2, r3));
        append(stream, segstore(r1, r2, 0));
        append(stream, loadval(0, 111));
        append(stream, add(r2, r2, r3));
        append(stream, segstore(r1, r2, 0));
        append(stream, loadval(0, 114));
        append(stream, add(r2, r2, r3));
        append(stream, segstore(r1, r2, 0));
        append(stream, loadval(0, 108));
        append(stream, add(r2, r2, r3));
        append(stream, segstore(r1, r2, 0));
        append(stream, loadval(0, 100));
        append(stream, add(r2, r2, r3));
        append(stream, segstore(r1, r2, 0));
        append(stream, loadval(0, 33));
        append(stream, add(r2, r2, r3));
        append(stream, segstore(r1, r2, 0));
        append(stream, loadval(0, 10));
        append(stream, add(r2, r2, r3));
        append(stream, segstore(r1, r2, 0));

        append(stream, loadval(r2, 0));
        append(stream, segload(0, r1, r2));
        append(stream, output(0));
        append(stream, add(r2, r2, r3));
        append(stream, segload(0, r1, r2));
        append(stream, output(0));
        append(stream, add(r2, r2, r3));
        append(stream, segload(0, r1, r2));
        append(stream, output(0));
        append(stream, add(r2, r2, r3));
        append(stream, segload(0, r1, r2));
        append(stream, output(0));
        append(stream, add(r2, r2, r3));
        append(stream, segload(0, r1, r2));
        append(stream, output(0));
        append(stream, add(r2, r2, r3));
        append(stream, segload(0, r1, r2));
        append(stream, output(0));
        append(stream, add(r2, r2, r3));
        append(stream, segload(0, r1, r2));
        append(stream, output(0));
        append(stream, add(r2, r2, r3));
        append(stream, segload(0, r1, r2));
        append(stream, output(0));
        append(stream, add(r2, r2, r3));
        append(stream, segload(0, r1, r2));
        append(stream, output(0));
        append(stream, add(r2, r2, r3));
        append(stream, segload(0, r1, r2));
        append(stream, output(0));
        append(stream, add(r2, r2, r3));
        append(stream, segload(0, r1, r2));
        append(stream, output(0));
        append(stream, add(r2, r2, r3));
        append(stream, segload(0, r1, r2));
        append(stream, output(0));
        append(stream, add(r2, r2, r3));
        append(stream, segload(0, r1, r2));
        append(stream, output(0));
        append(stream, add(r2, r2, r3));
        append(stream, halt());
}


void build_50m_loop(Seq_T stream)
{
        //setup
        append(stream, loadval(r1, 1)); //r1 is counter
        append(stream, loadval(r2, 2)); //r2 is incrementer
        
        append(stream, add(r1, r1, r2)); //increment counter by 2
        append(stream, loadval(r3, 5000000));
        append(stream, add(r1, r1, r2));
        append(stream, loadval(r5, 10));
        append(stream, add(r1, r1, r2));
        append(stream, mult(r3, r3, r5));
        append(stream, add(r1, r1, r2));
        append(stream, div(r4, r1, r3));
        append(stream, add(r1, r1, r2));
        append(stream, loadval(r5, 2));
        append(stream, add(r1, r1, r2));
        append(stream, loadval(r6, 18));
        append(stream, add(r1, r1, r2));
        append(stream, mov(r5, r6, r4));
        append(stream, add(r1, r1, r2));
        append(stream, prog(0, r5));
        append(stream, loadval(r7, 33));
        append(stream, output(r7)); //!
        append(stream, halt());
        //normal loop jumps to instruction index 3 but once we've reached the target, jumps to index (wherever halt is)




        

}


