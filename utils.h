//
// Created by alanz on 4/13/23.
//

#ifndef COMP30023_2023_PROJECT_1_UTILS_H
#define COMP30023_2023_PROJECT_1_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#define MAX_PROCESS_NAME_LEN 8

/* Data structure for processes */
typedef struct Process {
    char process_name[MAX_PROCESS_NAME_LEN];
    int arrival_time;
    int service_time;
    int remaining_time;
    int completion_time;
    int memory_required;
    int memory_start;
    int memory_end;
} Process;

/* Queue for storing the ready queue */
typedef struct Queue {
    Process **process_array;
    int front;
    int rear;
} Queue;

/* Data structure to simulate memory blocks */
typedef struct MemoryBlock {
    int block_id;
    int start_address;
    int end_address;
    int size;
    int process_id;
} MemoryBlock;

/* Data structure to simulate memory system */
typedef struct Memory {
    int size;
    int remaining_size;
    MemoryBlock **block_array;
} Memory;

/* Data structure to store command line arguments */
typedef struct Arguments{
    char *filename;
    char *scheduler;
    char *memory_strategy;
    int quantum;
} Arguments;

/* Custom round-half-up function */
double round_half_up(double num, int precision);

/* Initialise a queue and allocate memory */
Queue *init_queue(int num_processes);

/* Enqueue a process to the end of the queue */
void enqueue(Queue *queue, Process *process);

/* Dequeue the first process from the input queue */
Process *pop(Queue *ready_queue);

/* Helper function to read input into correct format */
int read_input(const char *file_path, Process **processes_ptr);

/* Helper function to parse command line arguments */
void parse_arg(int argc, char **argv, Arguments *args);

/* Helper function to print performance matrix */
void performance_matrix(Process *processes, int num_processes, int makespan);

#endif //COMP30023_2023_PROJECT_1_UTILS_H
