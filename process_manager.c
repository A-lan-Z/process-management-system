//
// Created by alanz on 4/14/23.
//

#include "process_manager.h"
#include "utils.h"
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
                if (strcmp(curr_process->process_name, process->process_name)) {
                    break;
                }
            }
        }
        ready_queue->process_array[i + 1] = ready_queue->process_array[i];
    }
    ready_queue->process_array[i + 1] = process;
    ready_queue->rear++;
}


// Simulate the Shortest Job First algorithm
void simulate_SJF(Process *input_queue, int num_processes) {
    Queue *ready_queue = init_queue(num_processes);
    int curr_time = 0;
    int completed_processes = 0;
    int last_arrival_index = 0;

    // Continue processing until all processes are completed
    while (completed_processes < num_processes) {
        // Add process that has arrived before current time to ready state
        Process *x = &input_queue[last_arrival_index];
        while (last_arrival_index < num_processes && input_queue[last_arrival_index].arrival_time <= curr_time) {
            printf("%d,READY,process_name=%s\n", curr_time, input_queue[last_arrival_index].process_name);
            insert_process_SJF(ready_queue, &input_queue[last_arrival_index]);
            last_arrival_index++;
        }
        if (ready_queue->front <= ready_queue->rear) {
            // There are remaining processes in ready queue, pop the shortest job
            Process *shortest = pop(ready_queue);
            // Simulate process completed, update process info
            printf("%d,RUNNING,process_name=%s,remaining_time=%d\n", curr_time, shortest->process_name, shortest->service_time);
            curr_time += shortest->service_time;
            shortest->completion_time = curr_time;
            shortest->remaining_time = 0;
            completed_processes++;
            printf("%d,FINISHED,process_name=%s,proc_remaining=%d\n", curr_time, shortest->process_name, num_processes - completed_processes);

        } else {
            // There are remaining processes but are not added to ready queue yet
            curr_time++;
        }
    }
    free(ready_queue->process_array);
}

