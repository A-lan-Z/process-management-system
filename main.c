//
// Created by alanz on 4/13/23.
//

#include "main.h"
#include "utils.h"
#include "process_manager.h"

int main(int argc, char **argv) {
    Arguments args;
    Process *input_queue = NULL;
    parse_arg(argc, argv, &args);
    int num_processes = read_input(args.filename, &input_queue);
    Queue *ready_queue = init_queue(num_processes);
    enqueue(ready_queue, &input_queue[0]);

    printf("Filename: %s\n", args.filename);
    printf("Scheduler: %s\n", args.scheduler);
    printf("Memory Strategy: %s\n", args.memory_strategy);
    printf("Quantum: %d\n", args.quantum);

    // Print out the processes to check the correctness of read_input

    for (int i = 1; i < num_processes; i++) {
        insert_process_SJF(ready_queue, &input_queue[i]);
        printf("Process Name: %s\n", input_queue[i].process_name);
        printf("Arrival Time: %d\n", input_queue[i].arrival_time);
        printf("Service Time: %d\n", input_queue[i].service_time);
        printf("Memory Required: %d\n", input_queue[i].memory_required);
        printf("Remaining Time: %d\n", input_queue[i].remaining_time);
    }
    printf("\n\n\n\n");

    for (int i = 0; i < num_processes; i++) {
        printf("Process Name: %s\n", ready_queue->process_array[i]->process_name);
        printf("Arrival Time: %d\n", ready_queue->process_array[i]->arrival_time);
        printf("Service Time: %d\n", ready_queue->process_array[i]->service_time);
        printf("Memory Required: %d\n", ready_queue->process_array[i]->memory_required);
        printf("Remaining Time: %d\n", ready_queue->process_array[i]->remaining_time);
    }

    return 0;
}