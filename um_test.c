#include <seq.h>
#include "um_mem.h"
#include <stdio.h>
#include <uarray.h>
#include "um_operate.h"
#include <sys/stat.h>

void test_initialize_um();
void test_map_segment();
void test_get_set_seg_value();
void test_unmap_segment();
void test_read_program();
void test_read_instruction();

int main()
{
    /* These tests require the um_mem_t struct to be accessible
    test_map_segment();
    test_unmap_segment();
    test_get_set_seg_value();
    */

    test_initialize_um();
    test_read_program();
    test_read_instruction();

    return 0;
}

void test_initialize_um()
{
    um_data_t um = initialize_um();

    printf("Register values: ");

    for (int i = 0; i < 8; i++){
        printf("%d ", um->registers[i]);
    }
    printf("\n");

    printf("Halt: %d\n", um->halting);

    printf("Program counter: %d\n", um->program_counter);


}

void test_read_program()
{
    printf("Testing Read Program\n");
    um_data_t UM = initialize_um();
    FILE *fp = fopen("add.um", "r");
    struct stat st;
    stat("add.um", &st);
    read_um_program(fp, UM, st.st_size / 4);

    printf("Words are: ");
    for (int i = 0; i < st.st_size / 4; i++) {
        printf("%d ", get_seg_value(UM->memory, 0, i));
    }
    printf("\n");
}


void test_read_instruction()
{
    printf("Testing Read Instruction\n");
    um_data_t UM = initialize_um();
    FILE *fp = fopen("print-six.um", "r");
    struct stat st;
    stat("print-six.um", &st);
    read_um_program(fp, UM, st.st_size / 4);

    printf("Instructions are: \n");
    for (int i = 0; i < st.st_size / 4; i++) {
        read_instruction(UM);
    }
}

/* These tests require the um_mem_t struct to be accessible
void test_map_segment() 
{
    printf("Testing Map Segment: \n");
    um_mem_t test_mem = um_mem_new();
    int indices[3];
    indices[0] = map_segment(test_mem, 20);
    indices[1] = map_segment(test_mem, 0);
    indices[2] = map_segment(test_mem, 3);

    printf("Indices are: %u, %u, %u\n", indices[0], indices[1], indices[2]);
    int sizes[3];
    sizes[0] = UArray_length(Seq_get(test_mem->segment_list, 0));
    sizes[1] = UArray_length(Seq_get(test_mem->segment_list, 1));
    sizes[2] = UArray_length(Seq_get(test_mem->segment_list, 2));
    printf("Sizes are: %u, %u, %u\n", sizes[0], sizes[1], sizes[2]);

    um_mem_free(test_mem);
}


void test_unmap_segment()
{
    printf("Testing Unmap Segment: \n");

    um_mem_t test_mem = um_mem_new();
    int ids[8];
    ids[0] = map_segment(test_mem, 10);
    ids[1] = map_segment(test_mem, 2);
    ids[2] = map_segment(test_mem, 15);
    ids[3] = map_segment(test_mem, 166);
    ids[4] = map_segment(test_mem, 21);
    unmap_segment(test_mem, 2);
    unmap_segment(test_mem, 3);
    ids[5] = map_segment(test_mem, 300);
    ids[6] = map_segment(test_mem, 31);
    ids[7] = map_segment(test_mem, 33);

    printf("IDs are: ");
    for (int i = 0; i < 8; i++){
        printf("%u, ", ids[i]);
    }
    printf("\n");

    
    for (int j = 1; j < 6; j++) {
        unmap_segment(test_mem, j);
    } 

    um_mem_free(test_mem);
}


void test_get_set_seg_value()
{
    printf("Testing Set/Get Seg Value: \n");
    um_mem_t test_mem = um_mem_new();
    int seg_0 = map_segment(test_mem, 5);
    int indices[4];
    indices[0] = map_segment(test_mem, 5);
    indices[1] = map_segment(test_mem, 5);
    indices[2] = map_segment(test_mem, 5);
    indices[3] = map_segment(test_mem, 5);


    set_seg_value(test_mem, 1, 0, 32);
    set_seg_value(test_mem, 1, 1, 77);
    set_seg_value(test_mem, 2, 0, 8);
    set_seg_value(test_mem, 2, 1, 25);

    int values[5];
    values[0] = get_seg_value(test_mem, 1, 0);
    values[1] = get_seg_value(test_mem, 1, 1);
    values[2] = get_seg_value(test_mem, 2, 0);
    values[3] = get_seg_value(test_mem, 2, 1);
    values[4] = get_seg_value(test_mem, 4, 3);
    
    printf("Values: %u, %u, %u, %u, %u\n", values[0], values[1], values[2], values[3], values[4]);
    
    um_mem_free(test_mem);    

    (void)seg_0; (void)indices;
}
*/