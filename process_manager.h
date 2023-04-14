//
// Created by alanz on 4/14/23.
//

#ifndef COMP30023_2023_PROJECT_1_PROC_MAN_H
#define COMP30023_2023_PROJECT_1_PROC_MAN_H

#include "utils.h"

// Insert a process into the ready queue in the order of SJF
void insert_process_SJF(Queue *ready_queue, Process *process);

// Simulate the Shortest Job First algorithm
void simulate_SJF(Process *input_queue, int num_processes);

#endif //COMP30023_2023_PROJECT_1_PROC_MAN_H
