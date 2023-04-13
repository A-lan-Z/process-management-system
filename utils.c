//
// Created by alanz on 4/13/23.
//

#include "utils.h"
#include <stdio.h>
#include <stdlib.h>


/* Helper function to read input into correct format */
void read_input(const char *file_path, Process **input_queue_ptr) {
    // Check validity of input file
    FILE *file = fopen(file_path, "r");
    if (!file) {
        perror("Error opening input file");
        exit(EXIT_FAILURE);
    }

    // Count the number of processes and allocate memory
    int lines = 1;
    char ch;
    while (!feof(file)) {
        ch = fgetc(file);
        if (ch == '\n') {
            lines++;
        }
    }

    *input_queue_ptr = malloc(lines * sizeof(Process));
    if (!(*input_queue_ptr)) {
        perror("Error allocating memory for input queue");
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_SET);

    // Read process data from the input file into the input queue
    for (int i = 0; i < lines; i++) {
        Process *p = &((*input_queue_ptr)[i]);
        fscanf(file, "%d%s%d%d", &p->arrival_time, p->process_name, &p->service_time, &p->memory_required);
        p->remaining_time = p->service_time;
    }
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