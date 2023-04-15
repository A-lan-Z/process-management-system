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


// Simulate the Shortest Job First algorithm and return makespan
int simulate_SJF(Process *processes, int num_processes, int quantum) {
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
            // Identify whether the currently running process (if any) has completed.
            if (running_process != NULL && running_process->remaining_time <= 0) {
                // Terminate and deallocate memory
                running_process->completion_time = curr_time;
                completed_processes++;
                printf("%d,FINISHED,process_name=%s,proc_remaining=%d\n", curr_time, running_process->process_name, last_arrival_index - completed_processes);
                running_process = NULL;
            }

            // Add all arrived process to input queue
            while (last_arrival_index < num_processes && processes[last_arrival_index].arrival_time <= curr_time) {
                enqueue(input_queue, &processes[last_arrival_index]);
                // Enqueue all processes in input queue to ready queue in ascending service time
                insert_process_SJF(ready_queue, &processes[last_arrival_index]);
                last_arrival_index++;
            }

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
    free(ready_queue->process_array);
    return curr_time - 1;
}


// Simulate the Round Robin algorithm and return makespan
int simulate_RR(Process *processes, int num_processes, int quantum) {
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
            // Identify whether the currently running process (if any) has completed.
            if (running_process != NULL && running_process->remaining_time <= 0) {
                // Terminate and deallocate memory
                running_process->completion_time = curr_time;
                completed_processes++;
                printf("%d,FINISHED,process_name=%s,proc_remaining=%d\n", curr_time, running_process->process_name, last_arrival_index - completed_processes);
                running_process = NULL;
            }

            // Add all arrived process to input queue and ready queue
            while (last_arrival_index < num_processes && processes[last_arrival_index].arrival_time <= curr_time) {
                enqueue(input_queue, &processes[last_arrival_index]);
                enqueue(ready_queue, &processes[last_arrival_index]);
                last_arrival_index++;
            }

            // If the process requires more CPU time and there are other READY processes, the process
            // is suspended and enters the READY state again to await more CPU time.
            if (ready_queue->front <= ready_queue->rear && running_process != NULL) {
                enqueue(ready_queue, running_process);
                running_process = NULL;
            }

            // Start a new READY process
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
    free(ready_queue->process_array);
    return curr_time - 1;
}
