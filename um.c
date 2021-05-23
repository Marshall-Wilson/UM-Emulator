/*
 * um.c 
 * 
 * Written by: Megan Gelement (mgelem01) and Marshall Wilson (wwilso02)
 * Date: April 11, 2021
 * 
 * Purpose: Main driver for UM program. 
 */


#include <stdio.h>
#include <stdlib.h>
#include <bitpack.h>
#include "um_operate.h"
#include <sys/stat.h>
#include "open_or_die.h"


int main(int argc, char *argv[])
{
    if (argc >= 3 || argc == 1) {
        fprintf(stderr, "USAGE: ./um program_filename.um\n");
        exit(EXIT_FAILURE);
    }

    /* get program file data */
    struct stat sb;
    if (stat(argv[1], &sb) == -1) {
        fprintf(stderr, "Stat Error\n");
        exit(EXIT_FAILURE);
    }

    int num_words = sb.st_size / 4;

    FILE *fp = open_or_die(argv[1]);

    um_data_t UM = initialize_um();

    read_um_program(fp, UM, num_words);

    while (!is_halting(UM)) {
        read_instruction(UM);
    }

    free_um(UM);
    fclose(fp);

    return EXIT_SUCCESS;
}