//
// Created by alanz on 4/14/23.
//

#include "process_manager.h"
#include "utils.h"
#include "memory.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdint.h>
#include <netinet/in.h>


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


/* Helper function to execute real processes */
int create_real_process(Process *process, int curr_time, int *pipe_in, int *pipe_out) {
    pid_t pid;

    if ((pid = fork()) == 0) {
        // In child process; handle input output
        close(pipe_in[1]);
        close(pipe_out[0]);
        dup2(pipe_in[0], STDIN_FILENO);
        dup2(pipe_out[1], STDOUT_FILENO);

        // Execute an instance of process
        execl("./process", "process", process->process_name, NULL);

        perror("excel");
        exit(EXIT_FAILURE);

    } else if (pid > 0) {
        // In parent process; handle input output
        close(pipe_in[0]);
        close(pipe_out[1]);

        // Send simulation time to process
        uint32_t big_endian_time = htonl(curr_time);
        write(pipe_in[1], &big_endian_time, sizeof(big_endian_time));

        // Read and verify last byte from process
        uint8_t last_byte;
        read(pipe_out[0], &last_byte, sizeof(last_byte));
        if (last_byte != ((big_endian_time >> 24) & 0xFF)) {
            fprintf(stderr, "Verification for last byte failed for process %s\n", process->process_name);
        }
        return pid;
    } else {
        perror("fork");
        exit(EXIT_FAILURE);
    }
}


/* Helper function to suspend processes */
void suspend_real_process(Process *process, int curr_time, int *pipe_in) {
    // Send simulation time of suspend
    uint32_t  big_endian_time = htonl(curr_time);
    write(pipe_in[1], &big_endian_time, sizeof(big_endian_time));

    // Send SIGTSTP signal to process, and wait for process to enter a stopped state
    kill(process->pid, SIGTSTP);
    int wstatus;
    waitpid(process->pid, &wstatus, WUNTRACED);
    while (!WIFSTOPPED(wstatus)) {
        waitpid(process->pid, &wstatus, WUNTRACED);
    }
}


/* Helper function to resume processes */
void resume_real_process(Process *process, int curr_time, int *pipe_in, int *pipe_out) {
    // Send simulation time of resume
    uint32_t  big_endian_time = htonl(curr_time);
    write(pipe_in[1], &big_endian_time, sizeof(big_endian_time));

    // Send SIGCONT signal to process, and verify last byte
    kill(process->pid, SIGCONT);
    uint8_t last_byte;
    read(pipe_out[0], &last_byte, sizeof(last_byte));
    if (last_byte != ((big_endian_time >> 24) & 0xFF)) {
        fprintf(stderr, "Verification for last byte failed for process %s\n", process->process_name);
    }
}


/* Helper function to terminate processes and read verification byte string*/
void terminate_real_process(Process *process, int curr_time, int *pipe_in, int *pipe_out, char *byte_string) {
    // Send simulation time of terminate
    uint32_t  big_endian_time = htonl(curr_time);
    write(pipe_in[1], &big_endian_time, sizeof(big_endian_time));

    // Send SIGTERM signal to process, and read 64 byte string
    kill(process->pid, SIGTERM);
    read(pipe_out[0], byte_string, 64);
}


/* Simulate the Shortest Job First algorithm and return makespan */
int simulate_SJF(Process *processes, int num_processes, int quantum, MemoryBlock **memory_blocks_ptr, int is_best_fit) {
    int curr_time = 0;
    int completed_processes = 0;
    int last_arrival_index = 0;
    Queue *input_queue = init_queue(num_processes);
    Queue *ready_queue = init_queue(num_processes);
    Process *running_process = NULL;

    // Initialise pipes for all processes
    for (int i = 0; i < num_processes; i++) {
        pipe(processes[i].pipe_in);
        pipe(processes[i].pipe_out);
    }

    // Continue processing until all processes are completed
    while (completed_processes < num_processes) {

        // Perform the following tasks only at the start of each cycle
        if (!(curr_time % quantum)) {
            if (running_process != NULL && running_process->remaining_time <= 0) {
                char byte_string[65] = {0};
                char *temp_name = running_process->process_name;

                // Terminate real process and then simulated process
                terminate_real_process(running_process, curr_time, running_process->pipe_in,
                                       running_process->pipe_out,
                                       byte_string);
                terminate_process(&running_process, curr_time, last_arrival_index, &completed_processes,
                                  *memory_blocks_ptr,
                                  is_best_fit);
                printf("%d,FINISHED-PROCESS,process_name=%s,sha=%s\n", curr_time, temp_name, byte_string);
            }

            // Enqueue process to input queue and ready queue
            enqueue_input_queue(processes, num_processes, curr_time, &last_arrival_index, input_queue);
            enqueue_ready_queue(input_queue, ready_queue, memory_blocks_ptr, is_best_fit, curr_time, 1);

            // Start a new READY process if there are no running process or resume process already running
            if (ready_queue->front <= ready_queue->rear && running_process == NULL) {
                running_process = pop(ready_queue);
                running_process->pid = create_real_process(running_process, curr_time, running_process->pipe_in, running_process->pipe_out);
                printf("%d,RUNNING,process_name=%s,remaining_time=%d\n", curr_time, running_process->process_name, running_process->remaining_time);
            } else if (running_process != NULL) {
                resume_real_process(running_process, curr_time, running_process->pipe_in, running_process->pipe_out);
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

    // Initialise pipes for all processes
    for (int i = 0; i < num_processes; i++) {
        pipe(processes[i].pipe_in);
        pipe(processes[i].pipe_out);
    }

    // Continue processing until all processes are completed
    while (completed_processes < num_processes) {

        // Perform the following tasks only at the start of each cycle
        if (!(curr_time % quantum)) {
            if (running_process != NULL && running_process->remaining_time <= 0) {
                char byte_string[65] = {0};
                char *temp_name = running_process->process_name;

                // Terminate real process and then simulated process
                terminate_real_process(running_process, curr_time, running_process->pipe_in,
                                       running_process->pipe_out,
                                       byte_string);
                terminate_process(&running_process, curr_time, last_arrival_index, &completed_processes, *memory_blocks_ptr,
                                  is_best_fit);
                printf("%d,FINISHED-PROCESS,process_name=%s,sha=%s\n", curr_time, temp_name, byte_string);
            }

            // Enqueue process to input queue and ready queue
            enqueue_input_queue(processes, num_processes, curr_time, &last_arrival_index, input_queue);
            enqueue_ready_queue(input_queue, ready_queue, memory_blocks_ptr, is_best_fit, curr_time, 0);

            // If the process requires more CPU time and there are other READY processes, the process
            // is suspended and enters the READY state again to await more CPU time.
            if (ready_queue->front <= ready_queue->rear && running_process != NULL) {
                suspend_real_process(running_process, curr_time, running_process->pipe_in);
                enqueue(ready_queue, running_process);
                running_process = NULL;
            }

            // Start a new READY process or resume a suspended process
            if (ready_queue->front <= ready_queue->rear && running_process == NULL) {
                // Process the next process in ready queue
                running_process = pop(ready_queue);
                if (!running_process->has_created) {
                    // Create a new real process
                    running_process->pid = create_real_process(running_process, curr_time, running_process->pipe_in,
                                                               running_process->pipe_out);
                    running_process->has_created = 1;
                } else {
                    // Resume the real process
                    resume_real_process(running_process, curr_time, running_process->pipe_in,
                                        running_process->pipe_out);
                }
                printf("%d,RUNNING,process_name=%s,remaining_time=%d\n", curr_time, running_process->process_name,
                       running_process->remaining_time);
            } else if (running_process != NULL) {
                // There are only one process in the ready queue
                resume_real_process(running_process, curr_time, running_process->pipe_in,
                                    running_process->pipe_out);
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
