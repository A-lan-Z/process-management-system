//
// Created by alanz on 4/14/23.
//

#include "process_manager.h"
#include "utils.h"
#include "memory.h"
#include <string.h>


// Insert a process into the ready queue in the order of SJF
void insert_process_SJF(Queue *ready_queue, Process *process) {
    // Insert the first process to the ready queue
    if (ready_queue->front == -1 && ready_queue->rear == -1) {
        ready_queue->front = 0;
        ready_queue->rear = 0;
        ready_queue->process_array[ready_queue->rear] = process;
        return;
    }
    int i;
    for (i = ready_queue->rear; i >= ready_queue->front; i--) {
        Process *curr_process = ready_queue->process_array[i];
        // Insert in ascending order of service time
        if (curr_process->service_time < process->service_time) {
            break;
        }
        // If equal service time, insert in ascending order of arrival time
        if (curr_process->service_time == process->service_time) {
            if (curr_process->arrival_time < process->arrival_time) {
                break;
            }
            // If equal arrival time, insert in ascending order of process name
            if (curr_process->arrival_time == process->arrival_time) {
                if (strcmp(curr_process->process_name, process->process_name) < 0) {
                    break;
                }
            }
        }
        ready_queue->process_array[i + 1] = ready_queue->process_array[i];
    }
    ready_queue->process_array[i + 1] = process;
    ready_queue->rear++;
}


/* Enqueue processes that have arrived to input queue */
void enqueue_input_queue(Process *processes, int num_processes, int curr_time, int *last_arrival_index, Queue *input_queue) {
    while (*last_arrival_index < num_processes && processes[*last_arrival_index].arrival_time <= curr_time) {
        enqueue(input_queue, &processes[*last_arrival_index]);
        (*last_arrival_index)++;
    }
}


/* Terminate process and deallocate memory when completed */
void terminate_process(Process **running_process, int curr_time, int last_arrival_index, int *completed_processes, MemoryBlock *memory_blocks, int is_best_fit) {
    if (*running_process != NULL && (*running_process)->remaining_time <= 0) {
        // Terminate and deallocate memory
        (*running_process)->completion_time = curr_time;
        (*completed_processes)++;
        printf("%d,FINISHED,process_name=%s,proc_remaining=%d\n", curr_time, (*running_process)->process_name, last_arrival_index - *completed_processes);

        // Deallocate memory
        if (is_best_fit) {
            dealloc(memory_blocks, (*running_process)->memory_start);
        }
        *running_process = NULL;
    }
}


/* Enqueue processes with successful memory allocation to ready queue */
void enqueue_ready_queue(Queue *input_queue, Queue *ready_queue, MemoryBlock **memory_blocks_ptr, int is_best_fit, int curr_time, int is_SJF) {
    // Best fit algorithm required, attempt to allocate memory to all process in input queue
    int input_queue_index = input_queue->front;
    while (input_queue_index <= input_queue->rear) {
        Process *candidate_process = input_queue->process_array[input_queue_index];
        int alloc_address = 0;

        // Add processes with memory allocated successfully to the ready queue
        if (is_best_fit) {
            alloc_address = best_fit_alloc(memory_blocks_ptr, candidate_process->memory_required);
            if (alloc_address == -1) {
                input_queue_index++;
                continue;
            }
            printf("%d,READY,process_name=%s,assigned_at=%d\n", curr_time, candidate_process->process_name, alloc_address);
        }
        candidate_process->memory_start = alloc_address;
        if (is_SJF) {
            insert_process_SJF(ready_queue, candidate_process);
        } else {
            enqueue(ready_queue, candidate_process);
        }
        dequeue(input_queue, input_queue_index);
    }
}


/* Simulate the Shortest Job First algorithm and return makespan */
int simulate_SJF(Process *processes, int num_processes, int quantum, MemoryBlock **memory_blocks_ptr, int is_best_fit) {
    int curr_time = 0;
    int completed_processes = 0;
    int last_arrival_index = 0;
    Queue *input_queue = init_queue(num_processes);
    Queue *ready_queue = init_queue(num_processes);
    Process *running_process = NULL;

    // Continue processing until all processes are completed
    while (completed_processes < num_processes) {

        // Perform the following tasks only at the start of each cycle
        if (!(curr_time % quantum)) {
            terminate_process(&running_process, curr_time, last_arrival_index, &completed_processes, *memory_blocks_ptr, is_best_fit);
            enqueue_input_queue(processes, num_processes, curr_time, &last_arrival_index, input_queue);
            enqueue_ready_queue(input_queue, ready_queue, memory_blocks_ptr, is_best_fit, curr_time, 1);

            // Start a new READY process if there are no running process
            if (ready_queue->front <= ready_queue->rear && running_process == NULL) {
                running_process = pop(ready_queue);
                printf("%d,RUNNING,process_name=%s,remaining_time=%d\n", curr_time, running_process->process_name, running_process->remaining_time);
            }
        }

        // Resume the previously running process
        if (running_process != NULL) {
            running_process->remaining_time--;
        }
        curr_time++;
    }

    // Free all used memory
    free(input_queue->process_array);
    free(ready_queue->process_array);
    free(input_queue);
    free(ready_queue);
    return curr_time - 1;
}


/* Simulate the Round Robin algorithm and return makespan */
int simulate_RR(Process *processes, int num_processes, int quantum, MemoryBlock **memory_blocks_ptr, int is_best_fit) {
    int curr_time = 0;
    int completed_processes = 0;
    int last_arrival_index = 0;
    Queue *input_queue = init_queue(num_processes);
    Queue *ready_queue = init_queue(num_processes);
    Process *running_process = NULL;

    // Continue processing until all processes are completed
    while (completed_processes < num_processes) {

        // Perform the following tasks only at the start of each cycle
        if (!(curr_time % quantum)) {
            terminate_process(&running_process, curr_time, last_arrival_index, &completed_processes, *memory_blocks_ptr,
                              is_best_fit);
            enqueue_input_queue(processes, num_processes, curr_time, &last_arrival_index, input_queue);
            enqueue_ready_queue(input_queue, ready_queue, memory_blocks_ptr, is_best_fit, curr_time, 0);

            // If the process requires more CPU time and there are other READY processes, the process
            // is suspended and enters the READY state again to await more CPU time.
            if (ready_queue->front <= ready_queue->rear && running_process != NULL) {
                enqueue(ready_queue, running_process);
                running_process = NULL;
            }

            // Start a new READY process
            if (ready_queue->front <= ready_queue->rear && running_process == NULL) {
                running_process = pop(ready_queue);
                printf("%d,RUNNING,process_name=%s,remaining_time=%d\n", curr_time, running_process->process_name,
                       running_process->remaining_time);
            }
        }

        // Resume the previously running process
        if (running_process != NULL) {
            running_process->remaining_time--;
        }
        curr_time++;
    }

    // Free all used memory
    free(input_queue->process_array);
    free(ready_queue->process_array);
    free(input_queue);
    free(ready_queue);
    return curr_time - 1;
}
