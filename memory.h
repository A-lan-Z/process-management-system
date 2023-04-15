//
// Created by alanz on 4/15/23.
//

#ifndef COMP30023_2023_PROJECT_1_MEMORY_H
#define COMP30023_2023_PROJECT_1_MEMORY_H

#include <stdio.h>
#include <stdlib.h>

/* Data structure to simulate a doubly linked list of memory blocks */
typedef struct MemoryBlock {
    int start_address;
    int size;
    int is_free;
    struct MemoryBlock* prev;
    struct MemoryBlock* next;
} MemoryBlock;

/* Initialise memory system */
MemoryBlock *init_memory(int tot_mem_size);

/* Allocate memory of required size with best fit algorithm */
int best_fit_alloc(MemoryBlock *memory_blocks, int memory_required);

/* Helper function to deallocate the block with input start address */
void dealloc(MemoryBlock *memory_blocks, int start_address);

#endif //COMP30023_2023_PROJECT_1_MEMORY_H
