//
// Created by alanz on 4/15/23.
//

#include "memory.h"

/* Initialise memory system */
MemoryBlock *init_memory(int tot_mem_size) {
    MemoryBlock *memory_blocks = (MemoryBlock *)malloc(sizeof(MemoryBlock));
    memory_blocks->start_address = 0;
    memory_blocks->size = tot_mem_size;
    memory_blocks->is_free = 1;
    memory_blocks->prev = NULL;
    memory_blocks->next = NULL;
    return memory_blocks;
}


/* Allocate memory of required size with best fit algorithm */
int best_fit_alloc(MemoryBlock *memory_blocks, int memory_required) {
    MemoryBlock *curr_block = memory_blocks;
    MemoryBlock *best_fit_block = NULL;

    // Iterate through the entire memory to find the best fit block
    while (curr_block) {
        if (curr_block->is_free && curr_block->size >= memory_required) {
            if (!best_fit_block || curr_block->size < best_fit_block->size) {
                best_fit_block = curr_block;
            }
        }
        curr_block = curr_block->next;
    }

    // Return fail if no suitable memory block found
    if (!best_fit_block) {
        return -1;
    }

    // Partition the best fit block to allocated block and remaining block
    MemoryBlock *allocated_block = (MemoryBlock *) malloc(sizeof(MemoryBlock));
    allocated_block->start_address = best_fit_block->start_address;
    allocated_block->size = memory_required;
    allocated_block->is_free = 0;
    allocated_block->prev = best_fit_block->prev;
    // Update best fit block to be the remaining block
    best_fit_block->start_address += memory_required;
    best_fit_block->size -= memory_required;

    // Remove the remaining block if it has size 0
    if (best_fit_block->size) {
        allocated_block->next = best_fit_block;
    } else {
        allocated_block->next = best_fit_block->next;
        free(best_fit_block);
    }

    // Update the previous and next block's pointers
    if (allocated_block->prev) {
        allocated_block->prev->next = allocated_block;
    }
    if (allocated_block->next) {
        allocated_block->next->prev = allocated_block;
    }

    return allocated_block->start_address;
}


/* Helper function to deallocate the block with input start address */
void dealloc(MemoryBlock *memory_blocks, int start_address) {
    MemoryBlock *curr_block = memory_blocks;

    // Iterate through the entire memory to find the block to be deallocated
    while (curr_block) {
        if (curr_block->start_address == start_address) {
            curr_block->is_free = 1;

            // Combine with previous free block if necessary
            if (curr_block->prev && curr_block->prev->is_free) {
                MemoryBlock *prev_block = curr_block->prev;
                prev_block->size += curr_block->size;
                prev_block->next = curr_block->next;

                if (curr_block->next) {
                    curr_block->next->prev = prev_block;
                }

                free(curr_block);
                curr_block = prev_block;
            }

            // Combine with next free block is necessary
            if (curr_block->next && curr_block->next->is_free) {
                MemoryBlock *next_block = curr_block->next;
                curr_block->size += next_block->size;
                curr_block->next = next_block->next;

                if (next_block->next) {
                    next_block->next->prev = curr_block;
                }
                free(next_block);
            }

            // Exist function once memory is deallocated
            return;
        }
        curr_block = curr_block->next;
    }
}