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


/* Find and return the best fit block in the entire memory */
MemoryBlock *find_best_fit(MemoryBlock *memory_blocks, int memory_required) {
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
    return best_fit_block;
}


/* Helper function to update the adjacent memory block pointers */
void update_adj_blocks(MemoryBlock *memory_block) {
    if (memory_block->prev) {
        memory_block->prev->next = memory_block;
    }
    if (memory_block->next) {
        memory_block->next->prev = memory_block;
    }
}


/* Allocate memory of required size with best fit algorithm */
int best_fit_alloc(MemoryBlock **memory_blocks, int memory_required) {
    MemoryBlock *best_fit_block = find_best_fit(*memory_blocks, memory_required);

    // Return fail if no suitable memory block found
    if (!best_fit_block) {return -1;}

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
    update_adj_blocks(allocated_block);

    // Update the head of list if required
    if (!allocated_block->prev) {
        *memory_blocks = allocated_block;
    }

    return allocated_block->start_address;
}


/* Helper function to combine adjacent free blocks */
void combine_adj_blocks(MemoryBlock **block_ptr_A, MemoryBlock **block_ptr_B) {
    MemoryBlock *block_A = *block_ptr_A;
    MemoryBlock *block_B = *block_ptr_B;

    block_A->size += block_B->size;
    block_A->next = block_B->next;

    if (block_B->next) {
        block_B->next->prev = block_A;
    }
    free(block_B);
    *block_ptr_B = NULL;
}


/* Helper function to deallocate the block with input start address */
void dealloc(MemoryBlock *memory_blocks, int start_address) {
    MemoryBlock *curr_block = memory_blocks;

    // Iterate through the entire memory to find the block to be deallocated
    while (curr_block) {
        if (curr_block->start_address == start_address) {
            curr_block->is_free = 1;

            // Combine with adjacent free blocks if necessary
            if (curr_block->prev && curr_block->prev->is_free) {
                MemoryBlock *prev_block = curr_block->prev;
                combine_adj_blocks(&(curr_block->prev), &curr_block);
                curr_block = prev_block;
            }
            if (curr_block->next && curr_block->next->is_free) {
                combine_adj_blocks(&curr_block, &(curr_block->next));
            }

            // Exist function once memory is deallocated
            return;
        }
        curr_block = curr_block->next;
    }
}