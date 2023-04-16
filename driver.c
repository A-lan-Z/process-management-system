//
// Created by alanz on 4/13/23.
//

#include <string.h>
#include "driver.h"
#include "process_manager.h"

#define IMPLEMENTS_REAL_PROCESS
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

    /********************************************************************************/
    /**                        Simulate Process Scheduling                         **/
    /********************************************************************************/
    int is_best_fit = strcmp(args.memory_strategy, "best-fit") == 0;
    int is_SJF = strcmp(args.scheduler, "SJF") == 0;
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