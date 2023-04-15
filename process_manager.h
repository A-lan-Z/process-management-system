//
// Created by alanz on 4/14/23.
//

#ifndef COMP30023_2023_PROJECT_1_PROC_MAN_H
#define COMP30023_2023_PROJECT_1_PROC_MAN_H

#include "utils.h"
#include "memory.h"


// Insert a process into the ready queue in the order of SJF
void insert_process_SJF(Queue *ready_queue, Process *process);

// Simulate the Shortest Job First algorithm and return makespan
int simulate_SJF(Process *processes, int num_processes, int quantum, MemoryBlock **memory_blocks_ptr, int is_best_fit);

// Simulate the Round Robin algorithm and return makespan
int simulate_RR(Process *processes, int num_processes, int quantum, MemoryBlock **memory_blocks_ptr, int is_best_fit);

#endif //COMP30023_2023_PROJECT_1_PROC_MAN_H
