//
// Created by alanz on 4/13/23.
//

#include "utils.h"
#include <stdio.h>
#include <stdlib.h>


/* Initialise a queue and allocate memory */
Queue *init_queue(int num_processes) {
    Queue *queue = malloc(sizeof(Queue));
    queue->process_array = malloc(num_processes * sizeof(Process *));
    if (!queue || !queue->process_array) {
        perror("Error allocating memory for queue");
        exit(EXIT_FAILURE);
    }
    queue->front = -1;
    queue->rear = -1;
    return queue;
}

/* Enqueue a process to the end of the queue */
void enqueue(Queue *queue, Process *process) {
    queue->rear++;
    queue->process_array[queue->rear] = process;

    // Update front when queue was empty
    if (queue->front == -1) {
        queue->front = 0;
    }
}

/* Dequeue the first process from the input queue */
Process *pop(Queue *ready_queue) {
    if (ready_queue->front > ready_queue->rear) {
        return NULL;
    }
    Process *p = ready_queue->process_array[ready_queue->front];
    ready_queue->front++;
    return p;
}

/* Helper function to read input into correct format */
int read_input(const char *file_path, Process **input_queue_ptr) {
    // Check validity of input file
    FILE *file = fopen(file_path, "r");
    if (!file) {
        perror("Error opening input file");
        exit(EXIT_FAILURE);
    }

    // Allocate memory for input queue
    *input_queue_ptr = NULL;
    int input_queue_size = 0;

    // Read process data from the input file into the input queue
    while (!feof(file)) {
        Process p;
        if (fscanf(file, "%d%s%d%d", &p.arrival_time, p.process_name, &p.service_time, &p.memory_required) == 4) {
            p.remaining_time = p.service_time;
            *input_queue_ptr = realloc(*input_queue_ptr, (input_queue_size + 1) * sizeof(Process));
            if (!(*input_queue_ptr)) {
                perror("Error allocating memory for input queue");
                exit(EXIT_FAILURE);
            }
            (*input_queue_ptr)[input_queue_size++] = p;
        }
    }

    fclose(file);
    return input_queue_size;
}

/* Helper function to parse command line arguments */
void parse_arg(int argc, char **argv, Arguments *args) {
    int opt;
    while ((opt = getopt(argc, argv, "f:s:m:q:")) != -1) {
        switch (opt) {
            case 'f':
                args->filename = optarg;
                break;
            case 's':
                args->scheduler = optarg;
                break;
            case 'm':
                args->memory_strategy = optarg;
                break;
            case 'q':
                args->quantum = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s -f <filename> -s (SJF | RR) -m (infinite | best-fit) -q (1 | 2 | 3)\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    if (args->filename == NULL || args->scheduler == NULL || args->memory_strategy == NULL || args->quantum == 0) {
        fprintf(stderr, "Usage: %s -f <filename> -s (SJF | RR) -m (infinite | best-fit) -q (1 | 2 | 3)\n", argv[0]);
        exit(EXIT_FAILURE);
    }
}
