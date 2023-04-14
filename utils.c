//
// Created by alanz on 4/13/23.
//

#include "utils.h"

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
int read_input(const char *file_path, Process **Processes_ptr) {
    // Check validity of input file
    FILE *file = fopen(file_path, "r");
    if (!file) {
        perror("Error opening input file");
        exit(EXIT_FAILURE);
    }

    // Allocate memory for input queue
    *Processes_ptr = NULL;
    int num_processes = 0;

    // Read process data from the input file into the input queue
    while (!feof(file)) {
        Process p;
        if (fscanf(file, "%d%s%d%d", &p.arrival_time, p.process_name, &p.service_time, &p.memory_required) == 4) {
            p.remaining_time = p.service_time;
            *Processes_ptr = realloc(*Processes_ptr, (num_processes + 1) * sizeof(Process));
            if (!(*Processes_ptr)) {
                perror("Error allocating memory for input queue");
                exit(EXIT_FAILURE);
            }
            (*Processes_ptr)[num_processes++] = p;
        }
    }

    fclose(file);
    return num_processes;
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

/* Helper function to print performance matrix */
void performance_matrix(Process *processes, int num_processes, int makespan) {
    double total_turnaround_time = 0;
    double total_overhead = 0;
    double max_overhead = 0;

    for (int i = 0; i < num_processes; i++) {
        int turnaround_time = processes[i].completion_time - processes[i].arrival_time;
        double overhead = (double)turnaround_time / processes[i].service_time;
        total_turnaround_time += turnaround_time;
        total_overhead += overhead;

        if (overhead > max_overhead) {
            max_overhead = overhead;
        }
    }
    int avg_turnaround_time = (int)((total_turnaround_time + num_processes - 1) / num_processes);
    double avg_overhead = total_overhead / num_processes;

    printf("Turnaround time %d\n", avg_turnaround_time);
    printf("Time overhead %.2f %.2f\n", max_overhead, avg_overhead);
    printf("Makespan %d\n", makespan);
}