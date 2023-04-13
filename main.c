//
// Created by alanz on 4/13/23.
//

#include "main.h"
#include "utils.h"

int main(int argc, char **argv) {
    Arguments args;
    Process *input_queue = NULL;
    parse_arg(argc, argv, &args);
    read_input(args.filename, &input_queue);

    printf("Filename: %s\n", args.filename);
    printf("Scheduler: %s\n", args.scheduler);
    printf("Memory Strategy: %s\n", args.memory_strategy);
    printf("Quantum: %d\n", args.quantum);

    // Print out the processes to check the correctness of read_input

    for (int i = 0; i < 3; i++) {
        printf("Process Name: %s\n", input_queue[i].process_name);
        printf("Arrival Time: %d\n", input_queue[i].arrival_time);
        printf("Service Time: %d\n", input_queue[i].service_time);
        printf("Memory Required: %d\n", input_queue[i].memory_required);
        printf("Remaining Time: %d\n", input_queue[i].remaining_time);
    }


    return 0;
}