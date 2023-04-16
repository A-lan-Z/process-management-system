//
// Created by alanz on 4/13/23.
//

#include <string.h>
#include "driver.h"
#include "utils.h"
#include "process_manager.h"
#include "memory.h"

#define TOTAL_MEMORY_SIZE 2048

int main(int argc, char **argv) {

    /********************************************************************************/
    /**                              Input Handling                                **/
    /********************************************************************************/
    Arguments args;
    Process *processes = NULL;
    MemoryBlock *memory_blocks = init_memory(TOTAL_MEMORY_SIZE);
    parse_arg(argc, argv, &args);
    int num_processes = read_input(args.filename, &processes);

    printf("Filename: %s\n", args.filename);
    printf("Scheduler: %s\n", args.scheduler);
    printf("Memory Strategy: %s\n", args.memory_strategy);
    printf("Quantum: %d\n", args.quantum);

    // Print out the processes to check the correctness of read_input

    for (int i = 0; i < 3; i++) {
        printf("Process Name: %s\n", processes[i].process_name);
        printf("Arrival Time: %d\n", processes[i].arrival_time);
        printf("Service Time: %d\n", processes[i].service_time);
        printf("Memory Required: %d\n", processes[i].memory_required);
        printf("Remaining Time: %d\n\n\n", processes[i].remaining_time);
    }


    /********************************************************************************/
    /**                               Simulate SJF                                 **/
    /********************************************************************************/
    int is_best_fit = strcmp(args.memory_strategy, "best-fit") ? 0 : 1;
    int is_SJF = strcmp(args.scheduler, "SJF") ? 0 : 1;
    int makespan;
    if (is_SJF) {
        makespan = simulate_SJF(processes, num_processes, args.quantum, &memory_blocks, is_best_fit);
    } else {
        makespan = simulate_RR(processes, num_processes, args.quantum, &memory_blocks, is_best_fit);
    }
    performance_matrix(processes, num_processes, makespan);

    // Free all used memory
    free(memory_blocks);
    free(processes);

    return 0;
}