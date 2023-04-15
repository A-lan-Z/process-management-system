//
// Created by alanz on 4/13/23.
//

#include "main.h"
#include "utils.h"
#include "process_manager.h"

#define TOTAL_MEMORY_SIZE 2048

int main(int argc, char **argv) {

    /********************************************************************************/
    /**                              Input Handling                                **/
    /********************************************************************************/
    Arguments args;
    Process *processes = NULL;
    parse_arg(argc, argv, &args);
    int num_processes = read_input(args.filename, &processes);

    printf("Filename: %s\n", args.filename);
    printf("Scheduler: %s\n", args.scheduler);
    printf("Memory Strategy: %s\n", args.memory_strategy);
    printf("Quantum: %d\n", args.quantum);

    // Print out the processes to check the correctness of read_input

    for (int i = 0; i < num_processes; i++) {
        printf("Process Name: %s\n", processes[i].process_name);
        printf("Arrival Time: %d\n", processes[i].arrival_time);
        printf("Service Time: %d\n", processes[i].service_time);
        printf("Memory Required: %d\n", processes[i].memory_required);
        printf("Remaining Time: %d\n", processes[i].remaining_time);
    }
    printf("\n\n\n\n");

    /********************************************************************************/
    /**                               Simulate SJF                                 **/
    /********************************************************************************/
    int makespan = simulate_RR(processes, num_processes, args.quantum);
    performance_matrix(processes, num_processes, makespan);

    return 0;
}