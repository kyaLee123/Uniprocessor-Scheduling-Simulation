/*
 CPSC 457 Assignment 1 : Scheduling
 Group member 1: Quin Lane UCID: 30151264
 Group member 2: Kya Broderick  UCID: 30173946
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_LINE_LENGTH 256

typedef struct process {
    int pid;
    int arrival_time;
    int time_until_first_response;
    int actual_cpu_burst;
    int start_time;
    int finish_time;
    int remaining_time;
    int waiting_time;
    double predicted_remaining_time;
    int last_time_active;
    int active_time;
    double turnaround_time;


} Process;

typedef struct results {
    double avg_waiting_time;
    double avg_turn_around;
    double avg_response_time;
    int sequence[50];
    int response_times[51];
    int arrival_times[51];
    int bursts[51];
    int start_times[51];
    int finish_times[51];
    int process_wait[51];
    int process_turn_around[51];
    int process_response[51];
} Results;


typedef struct {
    //set queue size to 1000
    Process items[1000];
    //set components of queue
    int front, rear, size, capacity;
} Queue;
void initQueue(Queue *q){
    //initialize queue components

    q->front = 0;
    q->rear = -1;
    q->size = 0;
    //set cpacity to queue size
    q->capacity = 1000;
}
int isEmpty(Queue *q){
    return q->size == 0;
}

int isFull(Queue *q){
    return q->size == q->capacity;
}

void enqueue(Queue *q, Process process) {

    //if queue is full, we do not allow an enqueue and return
    if (isFull(q)) {
        printf("Error: Queue is full. Cannot enqueue process with PID %d\n", process.pid);
        return;
    }
    //otherwise, we enqueue (keeping in mine we are using a circular queue)
    q->rear = (q->rear + 1) % q->capacity;
    q->items[q->rear] = process;

    //note the increase in size of the queue
    q->size++;
}

Process dequeue(Queue *q) {
    //initialize the variable to be returned
    Process process;

    //if there is nothing in the queue, we return an error
    if (isEmpty(q)) {
        printf("Error: Queue is empty. Cannot dequeue\n");
        // Return an error Process
        Process error_process = {-1, -1, -1, -1};
        return error_process;
    }

    //else, we find the process to dequeue (keeping in mind it is a circular queue) and return it
    process = q->items[q->front];
    q->front = (q->front + 1) % q->capacity; // Wrap around
    q->size--;
    return process;
}

Process peek(Queue *q) {
    if (isEmpty(q)) {
        printf("Error: Queue is empty. Cannot peek\n");
        Process error_process = {-1, -1, -1, -1};
        return error_process;
    }
    return q->items[q->front];
}
// Function to print process information
void print_output(int pid, int arrival, int burst, int start, int finish, int wait, int turnaround, int first_response) {
    printf("\n%d\t%d\t%d\t%d\t%d\t%d\t%d\t\t%d\n", pid, arrival, burst, start, finish, wait, turnaround, first_response);
}

//function to print the results by pid
void printByPID(Process orderedProcesses[]){
    for (int i=1;i<51;i++){
        print_output(orderedProcesses[i].pid, orderedProcesses[i].arrival_time, orderedProcesses[i].actual_cpu_burst,
                     orderedProcesses[i].start_time, orderedProcesses[i].finish_time, orderedProcesses[i].waiting_time,
                     orderedProcesses[i].turnaround_time, orderedProcesses[i].time_until_first_response);
    }
}

void initialize_results(Results *res){
    for (int i=0; i<51; i++){
        res->arrival_times[i] = -1;
        res->avg_turn_around = 0;
        res->avg_response_time = 0;
        res->avg_waiting_time = 0;
        res->bursts[i] = 0;
        res->finish_times[i] = 0;
        res->process_wait[i] = 0;
        res->process_response[i] = 0;
        res->process_turn_around[i] = 0;
        res->start_times[i] = -1;

        if (i != 50){
            res->sequence[i] = 0;
        }

    }

}

//here, I create my ordered process that keeps track of information of the function by pid
void orderedProcessInitialize (Process orderedProcesses[]){
    for (int i=1;i<51;i++){
        orderedProcesses[i].pid = i;
        orderedProcesses[i].arrival_time = 100000;
        orderedProcesses[i].actual_cpu_burst = 0;
        orderedProcesses[i].waiting_time = 0;
        orderedProcesses[i].turnaround_time = 0;
        orderedProcesses[i].finish_time = 0;
        orderedProcesses[i].start_time = 100000;
        orderedProcesses[i].time_until_first_response = 100000;
    }
}

//here, I update information from the current process by pid
void updateOrderedProcess(Process orderedProcesses[], Process current_process){

    //ordered process position is the current pid
    int j = current_process.pid;

    //update summation variables
    orderedProcesses[j].waiting_time += current_process.waiting_time;
    orderedProcesses[j].turnaround_time += current_process.turnaround_time;
    orderedProcesses[j].actual_cpu_burst += current_process.actual_cpu_burst;

    //if arrival time of current process is smallest, we use this arrival time
    if (current_process.arrival_time < orderedProcesses[j].arrival_time){
        orderedProcesses[j].arrival_time = current_process.arrival_time;
    }

    //if start time of current process is smallest, we use this start time
    if (current_process.start_time < orderedProcesses[j].start_time){
        orderedProcesses[j].start_time = current_process.start_time;
    }

    //if finish time of current process is biggest, we use this finish time
    if (current_process.finish_time > orderedProcesses[j].finish_time){
        orderedProcesses[j].finish_time = current_process.finish_time;
    }

    orderedProcesses[j].turnaround_time = orderedProcesses[j].finish_time - orderedProcesses[j].arrival_time;
    //if response time of current process is smallest, we use this response time
    if (current_process.time_until_first_response < orderedProcesses[j].time_until_first_response){

        orderedProcesses[j].time_until_first_response = current_process.time_until_first_response;

    }
}
/* function to sort a queue by remaining burst length*/
void sortSJF(Queue *q) {
    //if there is nothing to sort
    if (isEmpty(q)) {
        return;
    }

    //I used a bubble sorting algorithm to sort the queue
    for (int i = 0; i < q->size - 1; i++) {
        bool swapped = false;

        for (int j = 0; j < q->size - 1 - i; j++) {
            // Calculate actual index, keeping in mind I used a circular queue
            int index1 = (q->front + j) % q->capacity;
            int index2 = (q->front + j + 1) % q->capacity;

            // Compare the actual_cpu_burst of processes next to eachother
            if (q->items[index1].actual_cpu_burst > q->items[index2].actual_cpu_burst) {
                // Swap if needed
                Process temp = q->items[index1];
                q->items[index1] = q->items[index2];
                q->items[index2] = temp;
                swapped = true;

            }
        }

        // If no two elements were swapped in the loop, we are sorted and ok to break out
        if (!swapped) {
            break;
        }
    }
}


/* function to sort a queue by remaining burst length*/
Results sjf(Process *processes, int num_processes) {
    Results res; 
    //initilize results
    initialize_results(&res);

    Process orderedProcesses[51];
    orderedProcessInitialize(orderedProcesses);

    //init ready queue
    Queue readyQueue;
    initQueue(&readyQueue);

    //init counters
    double currTime = 0;
    int completed = 0;
    int i = 0;
    int sequence_index = 0;


    //while we have not finished fully executing every single process
    while (completed < num_processes) {

        //we enqueue all processes that have arrived by the current time
        while (i < num_processes && processes[i].arrival_time <= currTime) {
            enqueue(&readyQueue, processes[i]);
            //every time we enqueue something, we have to re-sort the queue
            sortSJF(&readyQueue);
            i++;
        }

        //if the queue is not empty we can begin executing the processes
        if (!isEmpty(&readyQueue)) {

            //dequeue the next process to execute
            Process currProcess = dequeue(&readyQueue);

            //since we are in a non-preemptive schedueling, process will be executing fully, so start time is the time of execution
            currProcess.start_time = currTime;

            if (res.start_times[currProcess.pid] == -1){
                    //first time a process with this PID is ran
                    res.start_times[currProcess.pid] = currProcess.start_time;
                    res.sequence[sequence_index] = currProcess.pid;
                    sequence_index ++;
                }


            //update waiting time
            currProcess.waiting_time = currTime - currProcess.arrival_time;

            //updat response time
            currProcess.time_until_first_response = currTime + currProcess.time_until_first_response - currProcess.arrival_time;

            //update the current time
            currTime = currTime + currProcess.actual_cpu_burst;

            //update time we finish at
            currProcess.finish_time = currTime;

            //update turnaround time
            currProcess.turnaround_time = currProcess.finish_time - currProcess.arrival_time;

            //update the processes by PID
            updateOrderedProcess(orderedProcesses, currProcess);

            //update the number of completed processes
            completed++;

        } else {
            //if queue is empty, we just increment the current time
            currTime++;
        }
    }

    //initiate variables
    double turnarounds = 0;
    double waitings = 0;
    double responses = 0;

    //go through ordered process list
    for (int i=0; i<51; i++){

        //update the variables
        turnarounds += orderedProcesses[i].turnaround_time;
        waitings += orderedProcesses[i].waiting_time ; //????????????????????????????????????????????????do we need to divide or no????????????????????????????????????????????????
        responses += orderedProcesses[i].time_until_first_response;
        //update results
        res.finish_times[i] = orderedProcesses[i].finish_time;
        res.process_turn_around[i] = orderedProcesses[i].turnaround_time;
        res.process_response[i] = orderedProcesses[i].time_until_first_response;
        res.arrival_times[i] = orderedProcesses[i].arrival_time;
        res.process_wait[i] = orderedProcesses[i].waiting_time;
        res.bursts[i] = orderedProcesses[i].actual_cpu_burst;
    }


    res.avg_waiting_time = waitings / 50;
    res.avg_turn_around = turnarounds / 50;
    res.avg_response_time = responses / 50;

    return res;

}


// Round Robin Scheduling Function
Results round_robin(Process *processes, int num_processes, int time_quantum) {
    Results res; 
    //initilize results
    initialize_results(&res);

    //initialize queue
    Queue readyQueue;
    initQueue(&readyQueue);


    //initialize ordered process list
    Process orderedProcesses[51];
    orderedProcessInitialize(orderedProcesses);


    //initialize counters
    int current_time = 0;
    int completed = 0;
    int i = 0;
    int sequence_index = 0;


    //initiaze last process
    Process lastProcess;
    lastProcess.remaining_time = 0;


    while (completed < num_processes) {


        // Enqueue all processes that have arrived by current_time
        while (i < num_processes && processes[i].arrival_time <= current_time){
            processes[i].remaining_time = processes[i].actual_cpu_burst;
            processes[i].finish_time = 0;
            //enqueue arrived proccesses
            enqueue(&readyQueue, processes[i]);
            i++;
        }


        //if the last process did not finish, we enqueue it
        if (lastProcess.remaining_time > 0){
            enqueue(&readyQueue, lastProcess);}


        //if the queue is empty, we increment the current time
        if (isEmpty(&readyQueue)) {
            current_time++;


        } else {


            // Dequeue the next process
            Process current_process = dequeue(&readyQueue);


            //if remaining time is equal to the actual cpu burst, we update the start time, as the process is executing for the first time
            if (current_process.remaining_time == current_process.actual_cpu_burst) {
                current_process.start_time = current_time;
                if (res.start_times[current_process.pid] == -1){

                    //first time a process with this PID is ran
                    res.start_times[current_process.pid] = current_process.start_time;
                    res.sequence[sequence_index] = current_process.pid;
                    sequence_index ++;
                }
                current_process.waiting_time = current_time - current_process.arrival_time;
            }
            else {


                //update waiting time to be time from last execution
                current_process.waiting_time += current_time - current_process.finish_time;
            }


            //if response time is bigger than quantum, we minus from the process what response time has now been completed
            if (current_process.time_until_first_response > time_quantum) {
                current_process.time_until_first_response = current_process.time_until_first_response - time_quantum;
            }


                //otherwise, the response time is finishing in this execution. We run it as response time + current process - arrival
            else {
                current_process.time_until_first_response = current_time - current_process.arrival_time + current_process.time_until_first_response;
            }


            //if the remaining time of the process is greater than the time quantum, we need to prepare to re-execute
            if (current_process.remaining_time > time_quantum) {


                // Execute for time_quantum
                current_time += time_quantum;
                current_process.remaining_time -= time_quantum;
                current_process.finish_time = current_time;


                //otherwise, we are completing our process execution this iteration
            } else {


                // Execute the remaining burst
                current_time += current_process.remaining_time;
                current_process.finish_time = current_time;
                current_process.turnaround_time = current_process.finish_time - current_process.arrival_time;
                current_process.remaining_time = 0; // Mark process as completed
                completed++;


                //update the list of processes by pid
                updateOrderedProcess(orderedProcesses, current_process);


            }


            //mark the current process the last process
            lastProcess = current_process;


        }


    }

    double responses = 0;
    double turnarounds = 0;
    double waitings = 0;


    //gather all data needed for averaging
    for (int j=0; j<51; j++){
        turnarounds += orderedProcesses[j].turnaround_time;
        waitings += orderedProcesses[j].waiting_time;
        responses += orderedProcesses[j].time_until_first_response;

        res.finish_times[j] = orderedProcesses[j].finish_time;
        res.process_turn_around[j] = orderedProcesses[j].turnaround_time;
        res.process_response[j] = orderedProcesses[j].time_until_first_response;
        res.arrival_times[j] = orderedProcesses[j].arrival_time;
        //res.start_times[j] = orderedProcesses[j].start_time;
        res.process_wait[j] = orderedProcesses[j].waiting_time;
        res.bursts[j] = orderedProcesses[j].actual_cpu_burst;
    }

    res.avg_waiting_time = waitings / 50;
    res.avg_turn_around = turnarounds / 50;
    res.avg_response_time = responses / 50;

    return res;
}


//this function will sort the queue by 1 / process ID
void sortPriority(Queue *q) {
    if (isEmpty(q)) {
        return; // Nothing to sort if the queue is empty
    }

    // Bubble Sort Algorithm
    for (int i = 0; i < q->size - 1; i++) {
        bool swapped = false;

        for (int j = 0; j < q->size - 1 - i; j++) {

            // Calculate indexes because of circular queue
            int index1 = (q->front + j) % q->capacity;
            int index2 = (q->front + j + 1) % q->capacity;

            // compare PID (1/pid is the same as going the bigger PID is favored)
            if ( q->items[index1].pid  >  q->items[index2].pid ) {
                // Swap the processes if the next one has higher PID
                Process temp = q->items[index1];
                q->items[index1] = q->items[index2];
                q->items[index2] = temp;
                swapped = true;

            }
        }

        // If no two elements were swapped, we are sorted
        if (!swapped) {
            break;
        }
    }
}

//this function sorts processes by non preemptive priority (1/pid)
Results nonPreemptivePriority(Process *processes, int num_processes) {

    Results res; 
    //initilize results
    initialize_results(&res);

    Queue readyQueue;
    initQueue(&readyQueue);

    //initiaze list of processes by pid
    Process orderedProcesses[51];
    orderedProcessInitialize(orderedProcesses);

    int current_time = 0;
    int i = 0;
    int completed = 0;
    int sequence_index = 0;

    while (completed < num_processes) {

        //While processes arrive
        while (i < num_processes && processes[i].arrival_time <= current_time) {
            enqueue(&readyQueue, processes[i]);
            sortPriority(&readyQueue);
            i++;
        }

        //if the queue is empty, we continue
        if (isEmpty(&readyQueue)) {
            current_time++;
        } else {
            //dequeue the latest process
            Process currProcess = dequeue(&readyQueue);

            //update vars
            currProcess.start_time = current_time;

            if (res.start_times[currProcess.pid] == -1){
                    //first time a process with this PID is ran
                    res.start_times[currProcess.pid] = currProcess.start_time;
                    res.sequence[sequence_index] = currProcess.pid;
                    sequence_index ++;
                }
            //update variables
            currProcess.waiting_time = currProcess.start_time - currProcess.arrival_time;
            currProcess.finish_time = currProcess.start_time + currProcess.actual_cpu_burst;
            currProcess.time_until_first_response = current_time + currProcess.time_until_first_response - currProcess.arrival_time;

            currProcess.turnaround_time = currProcess.finish_time - currProcess.arrival_time;
            current_time = currProcess.actual_cpu_burst + current_time;

            //update by pid
            updateOrderedProcess(orderedProcesses, currProcess);
            //update the completed count
            completed++;
        }
    }
    //initiate variables for averaging
    double turnaroundFinal = 0;
    double waitingFinal = 0;
    double firstResponses = 0;

    for (int j = 0; j < 51; j++) {
        //update variables per pid
        turnaroundFinal += orderedProcesses[j].turnaround_time;
        waitingFinal += orderedProcesses[j].waiting_time;
        firstResponses += orderedProcesses[j].time_until_first_response;

        res.finish_times[j] = orderedProcesses[j].finish_time;
        res.process_turn_around[j] = orderedProcesses[j].turnaround_time;
        res.process_response[j] = orderedProcesses[j].time_until_first_response;
        res.arrival_times[j] = orderedProcesses[j].arrival_time;
        res.process_wait[j] = orderedProcesses[j].waiting_time;
        res.bursts[j] = orderedProcesses[j].actual_cpu_burst;
    }

    res.avg_waiting_time = waitingFinal / 50;
    res.avg_turn_around = turnaroundFinal / 50;
    res.avg_response_time = firstResponses / 50;

    return res;
}

//Algorithm to sort given queue using bubble sort based on predicted remaining time of the processes
void sortQueueSRT(Queue *q) {
    if (isEmpty(q)) return;

    int size = q->size;  // Get the number of elements in the queue
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - 1 - i; j++) {
            int current_index = (q->front + j) % q->capacity;
            int next_index = (q->front + j + 1) % q->capacity;

            if (q->items[current_index].predicted_remaining_time > q->items[next_index].predicted_remaining_time) {
                // Swap the processes
                Process temp = q->items[current_index];
                q->items[current_index] = q->items[next_index];
                q->items[next_index] = temp;
            }
        }
    }
}

/**
 * @param  int list[]  List containing values to be averaged
 * @param  int size  Size of the list
 * @return the average of the given elements in the list
 * Function calculates the average based on the given list and size
 */
double calc_avg(int list[], int size){

    double sum = 0.0;

    for (int i = 1; i < size+1; i++){
        sum += list[i];
        
    }
    return sum / size;
}


/**
 * @param  Queue *q  Represents Ready Queue
 * @param  int pid  PID to be used to match processes in the ready queue
 * @param  double new_predicted_remaining_time  new predicted remaining time value to use to update in the ready queue
 * @return None
 * Function goes through the queue and updates the predicted remaining time of any processes with the matching PID to the new given predicted remaining time
 */

void updatePredictedRemainingTime(Queue *q, int pid, double new_predicted_remaining_time) {
    for (int i = q->front; i <= q->rear; i++) {
        if (q->items[i].pid == pid && q->items[i].start_time == -1) {
            // Update the predicted remaining time based on the completed process
            q->items[i].predicted_remaining_time = new_predicted_remaining_time;
        }
    }
}




/** FCFS 
 * @param   Process processes[]   List of processes to be run.
 * @param   int size   size of the given list of processes.
 * @return  Results of running the given process list.
 *
 * FCFS algorithm looks at given list of processes to be run, and determines which processes to run in which order.
 * FCFS will execute processes based on their arrival time in the ready queue, and run each process to completion without interuption
 * Return results based on processes executed once they have all been run to completion.
 */


Results FCFS(Process processes[], int size){

    Results res; 
    //initilize results
    initialize_results(&res);

    //Initialize arrays to keep track of process info, max size of 50 since their are 50 PIDs
    int waiting_times[51] = {0};
    int turn_around_times[51] = {0};
    int response_times[51] = {0};
    int first_arrival[51]; //used for calcualtion in turnaround
    int last_completion[51] = {0}; //used for calcualtion in turnaround
    Process first_process[51];

    for (int i=0; i <51; i++){
        //initialize arrays
        first_arrival[i] = -1;
    }

    int sequence_index = 0;


    //set variables used when looping
    int current_time = 0; //Simulation starts at time 0
    int process_index = 0; //to keep track of next process to arrive
    int completed_processes = 0; //Track number of completed processes
    Process current_process;

    int in_progress = 0; //to keep track of if a process is currently running. set first to False
    Queue ready_queue;
    initQueue(&ready_queue);

    //Keep looping until all of the processes have been completed
    while (completed_processes < size){

        //check for new processes arriving at current_time and add them to ready queue
        for( int i = process_index; i < size; i++){
            if (current_time == processes[i].arrival_time){
                //add this process to the queue
                enqueue(&ready_queue, processes[i]);

                if (first_arrival[processes[i].pid] == -1){
                    //first time a process with this PID arrived
                    res.arrival_times[processes[i].pid] = processes[i].arrival_time;
                    first_arrival[processes[i].pid] = processes[i].arrival_time;
                    
                }

            } else if (current_time < processes[i].arrival_time){
                //current time exceeds the processes which are sorted by arrival time, so there is no reason to keep looping
                process_index = i;
                break;
            }

        }
        //Nothing is in progress, and theres at least one process in the ready queue
        if (!in_progress && !isEmpty(&ready_queue)){
            current_process = dequeue(&ready_queue); //Dequeue the process at front of queue to be current process
            if (current_process.start_time == -1){
                //this is the first time this process is running
                current_process.start_time = current_time;
                
                if (res.start_times[current_process.pid] == -1){
                    //first time a process with this PID is ran
                    res.start_times[current_process.pid] = current_process.start_time;
                    first_process[current_process.pid] = current_process;
                    res.sequence[sequence_index] = current_process.pid;
                    sequence_index ++;
                }

            }
            //update that processes waiting time
            current_process.waiting_time = current_time - current_process.arrival_time;
            current_process.remaining_time = current_process.actual_cpu_burst;

            waiting_times[current_process.pid] += current_process.waiting_time; //update total waiting time for the current PID
            //update in progress
            in_progress = 1; //True
        }
        if (in_progress){
            //Process in progress decrease the remaining time
            current_process.remaining_time -= 1; //decrease remaing time by 1

            //PROCESS finished running
            if (current_process.remaining_time == 0){
                current_process.finish_time = current_time;
                res.bursts[current_process.pid] += current_process.actual_cpu_burst; //track total burst of process
                
                //update the completion time of each process to be used in calculation of turnaround time
                last_completion[current_process.pid] = current_time;

                if (response_times[current_process.pid] == 0){
                    //if this is the first process with this PID that has been completed
                    int response_time = current_process.time_until_first_response + current_process.waiting_time;
                    response_times[current_process.pid] = response_time;
                    //first_arrival[current_process.pid] = current_process.arrival_time;
                    

                }

                completed_processes +=1;
                in_progress = 0; //reset in progress to False since process has finished.
            }
        
        }
        //increment current time by 1
        current_time += 1;
    }

    //calculate turnaround time for each PID
    for (int i=1; i < 51; i++){
        turn_around_times[i] = last_completion[i] - first_arrival[i];
        res.finish_times[i] = last_completion[i]; //store latest finish time
        res.process_turn_around[i] = turn_around_times[i];
        res.process_wait[i] = waiting_times[i];
        res.process_response[i] = response_times[i];
    }

    res.avg_waiting_time = calc_avg(waiting_times, 50);
    res.avg_response_time = calc_avg(response_times, 50);
    res.avg_turn_around = calc_avg(turn_around_times, 50);
    return res;
}


/** Shortest Time Remaining (SRT) using exponential averaging
 * @param   Process processes[]   List of processes to be run.
 * @param   double a   value of 'a' to be used in the exponential averaging calculation.
 * @param   int size   amount of processes in the given list of processes.
 * @return  Results of running the given process list.
 *
 * SRT algorithm looks at given list of processes and determines the order in which the processes should be executed.
 * Order is determined by Shortest Remaing Time based on exponential averaging to predict future burst times of processses.
 * After all processes in the given list have been run to completion, function calculates averages and returns the results.
 */
Results SRT(Process processes[], double a, int size){
    Results res;
    initialize_results(&res);


    //Initialize arrays to keep track of process metrics, each to be accessed by there process IDs (there are 50 PIDs)
    int waiting_times[51] = {0}; //50 processes initialized to zero;
    int turn_around_times[51] = {0}; 
    int response_times[51] = {0}; 
    int first_arrival[51]; //used for calcualtion in turnaround time
    int last_completion[51] = {0}; //used for calcualtion in turnaround time
    double predicted_bursts[51]; //keep track of predicted bursts for each process PID, defaulted to t0 = 10.

    //used to set the base predicted burst value for all PIDs to 10
    for (int i = 0; i < 51; i++) {
        predicted_bursts[i] = 10;
        processes[i].predicted_remaining_time = 10;
        first_arrival[i] = -1;
    }

    //set variables to be used during main execution loop of processes
    int current_time = 0; //Simulation starts at time 0
    int process_index = 0; //to keep track of next process to arrive
    int completed_processes = 0; //Track number of completed processes   
    int in_progress = 0; //to keep track of if a process is currently running. set first to False
    int sequence_index = 0;

    Process current_process; 
    Queue ready_queue;
    initQueue(&ready_queue);


    //Keep looping until all of the processes have been completed
    while (completed_processes < size){

        //NEW PROCESS ARRIVAL: check for new processes arriving at current_time and add them to ready queue
        for( int i = process_index; i < size; i++){
            if (current_time == processes[i].arrival_time){
                processes[i].predicted_remaining_time = predicted_bursts[processes[i].pid]; //set the predicted bursts
                
                enqueue(&ready_queue, processes[i]); //add this process to the queue

                if (first_arrival[processes[i].pid] == -1){
                    //first time a process with this PID arrived
                    res.arrival_times[processes[i].pid] = processes[i].arrival_time;
                    first_arrival[processes[i].pid] = processes[i].arrival_time;
                    
                }
                sortQueueSRT(&ready_queue); //after enqueing, resort the queue

            } else if (current_time < processes[i].arrival_time){
                //current time exceeds the arrival time of current process, and since process list is sorted by arrvival time, no need to keep searching
                process_index = i;
                break;
            }
        }

        //Nothing is in progress and the ready queue is not empty.
        if (!in_progress && !isEmpty(&ready_queue)){
            current_process = dequeue(&ready_queue); //Dequeue the process at front of queue to be current process
            
            if (current_process.active_time == 0){
                //This means this is the first time this process is being run
                current_process.waiting_time = current_time - current_process.arrival_time;
                waiting_times[current_process.pid] += current_process.waiting_time; //update the total waiting time for that process ID
                current_process.start_time = current_time;

                if (res.start_times[current_process.pid] == -1){
                    //first time a process with this PID is ran
                    res.start_times[current_process.pid] = current_process.start_time;
                    res.sequence[sequence_index] = current_process.pid;
                    sequence_index ++;
                }
                
                    
            } else {
                //This means this process is returning to being run after waiting again in the ready queue
                int extra_wait = current_time - current_process.last_time_active; //Calculates the extra time spent back in the ready queue
                current_process.waiting_time += extra_wait; //add this extra time to the current waiting time
                waiting_times[current_process.pid] += extra_wait; //only adjust for the extra wait in the total waiting time for the process
            } 
            in_progress = 1; //update in progress tracker
        }


        //Handles in progress process
        if (in_progress){
            
            //PRE-EMPTION based on predicted remaining time compared to the process in the front of the queue
            if (!isEmpty(&ready_queue) && (current_process.predicted_remaining_time > peek(&ready_queue).predicted_remaining_time)){

                current_process.last_time_active = current_time;
                enqueue(&ready_queue, current_process); //add the currently running process back into queue
                sortQueueSRT(&ready_queue);

                current_process = dequeue(&ready_queue); //change current process to the process with shorter remaining time (predicted)
                current_process.remaining_time = current_process.actual_cpu_burst;
                in_progress = 1;

                //Initial Wait time calculation

                if (current_process.active_time == 0){
                    //This means this is the first time this process is being run
                    current_process.waiting_time = current_time - current_process.arrival_time;
                    waiting_times[current_process.pid] += current_process.waiting_time; //update the total waiting time for that process ID
                    current_process.start_time = current_time;
                    if (res.start_times[current_process.pid] == -1){
                    //first time a process with this PID is ran
                    res.start_times[current_process.pid] = current_process.start_time;
                    res.sequence[sequence_index] = current_process.pid;
                    sequence_index ++;
                    }   

                } else {
                    //This means this process is returning to being run after waiting again in the ready queue
                    int extra_wait = current_time - current_process.last_time_active; //Calculates the extra time spent back in the ready queue
                    current_process.waiting_time += extra_wait; //add this extra time to the current waiting time
                    waiting_times[current_process.pid] += extra_wait; //only adjust for the extra wait in the total waiting time for the process
                } 
            }

            //update process remaining and active time for this current time 
            current_process.active_time += 1;
            current_process.remaining_time -= 1; //decrease remaing time by 1
    
            current_process.predicted_remaining_time -= 1;

            //PROCESS finished running
            if (current_process.remaining_time == 0){
                current_process.finish_time = current_time + 1; //adjusted +1 since time hasnt been incremented **********************************************
                res.bursts[current_process.pid] += current_process.actual_cpu_burst; //track total burst of process

                //Calculate next burst prediction of completed process PID using exponential averaging 
                predicted_bursts[current_process.pid] = (a * current_process.actual_cpu_burst) + ((1-a) * predicted_bursts[current_process.pid]);
                updatePredictedRemainingTime(&ready_queue, current_process.pid, predicted_bursts[current_process.pid]);
 
                if (current_time+1 > last_completion[current_process.pid]) {
                    last_completion[current_process.pid] = current_time+1;
                }

                //last_completion[current_process.pid] = current_time + 1;  //track completion time for that PID

                if (response_times[current_process.pid] == 0){
                //if this is the first time a process with this PID that has been executed, track its response time and arrival time to be used later
                    int response_time = current_process.time_until_first_response + current_process.waiting_time;
                    response_times[current_process.pid] = response_time;
                
                }

                completed_processes +=1;
                in_progress = 0; //reset in progress to False since process has finished.
            }
        }
        current_time += 1; //increment time of simulation
    }
    
    //calculate turnaround time for each PID
    for (int i=1; i < 51; i++){
        turn_around_times[i] = last_completion[i] - first_arrival[i];
        res.finish_times[i] = last_completion[i]; //store latest finish time
        res.process_turn_around[i] = turn_around_times[i];
        res.process_wait[i] = waiting_times[i];
        res.process_response[i] = response_times[i];
    }

    res.avg_waiting_time = calc_avg(waiting_times, 50);
    res.avg_response_time = calc_avg(response_times, 50);
    res.avg_turn_around = calc_avg(turn_around_times, 50);
    return res;
}



void printOutput(Results results, int size){
    //print sequence
    printf("seq = [");
    for (int i =0; i < size-1; i++){
        printf("%d, ", results.sequence[i]);
    }
    printf("%d]\n", results.sequence[size-1]);


    //print header
    printf("+----+---------+-------+-------+--------+------+------------+--------------+\n");
    printf("| Id | Arrival | Burst | Start | Finish | Wait | Turnaround | Response Time |\n");
    printf("+----+---------+-------+-------+--------+------+------------+--------------+\n");

    //print proces info for each process
    for (int i = 1; i < 51; i++) {
        printf("| %2d | %7d | %5d | %5d | %6d | %4d | %10d | %12d |\n", 
        i, 
        results.arrival_times[i], 
        results.bursts[i], 
        results.start_times[i], 
        results.finish_times[i], 
        results.process_wait[i], 
        results.process_turn_around[i], 
        results.process_response[i] );
    }
    //Footer
    printf("+----+---------+-------+-------+--------+------+------------+--------------+\n");
    printf("Average waiting time: %.2f ms\n", results.avg_waiting_time);
    printf("Average turnaround time: %.2f ms\n", results.avg_turn_around);
    printf("Average response time: %.2f ms\n", results.avg_response_time);

}


    void read_file_stdin(Process processes[]) {

        //get name of file from stdin

        // Read lines of the file, one at a time
        char buffer[MAX_LINE_LENGTH];
        int linenum = 0;
        int i = 0;

        // Handle header (skip)
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            perror("Error reading Header");
            exit(1);
        }

        while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            linenum += 1;

            // Extract data items from this line using sscanf()
            if (sscanf(buffer, "%d,%d,%d,%d",
                       &processes[i].pid,
                       &processes[i].arrival_time,
                       &processes[i].time_until_first_response,
                       &processes[i].actual_cpu_burst) != 4) {
                perror("Error reading in line");
            }

            processes[i].waiting_time = 0;
            processes[i].turnaround_time = 0;
            processes[i].finish_time = 0;
            processes[i].start_time = -1; // Indicates not started yet
            processes[i].remaining_time = processes[i].actual_cpu_burst; // Initialize remaining time to actual CPU burst
            i += 1; // Increment array index
        }
    }

    
    int main(int argc, char *argv[]) {
        // Define list of Processes that will hold the data for each process
        Process processes[1000]; // Size 1000 because the csv file has 1000 entries
        
        read_file_stdin(processes); //initialize processes
     
        //RUN ALGORITHMS
            if (argc < 2){
            //not enough arguments
            printf("Wrong format for cmd line inputs");
            exit(1);
        }

        //Find associated algorithm
        if (strcmp(argv[1], "FCFS") == 0){
            //RUN FCFS
            Results res = FCFS(processes, 1000);
            printOutput(res, 50);
            

        } else if (strcmp(argv[1], "SJF") == 0){
            Results res = sjf(processes,1000);
            printOutput(res, 50);

        } else if (strcmp(argv[1], "Priority") == 0) {
            //Call Priority
            Results res = nonPreemptivePriority(processes, 1000);
            printOutput(res, 50);

        } else if (strcmp(argv[1], "RR") == 0) {
            //also have to check if they added the quanta as an argument
            if (argc < 3){
                exit(1);
            }
            //Call RR
            int quantum = atoi(argv[2]);      
            Results res = round_robin(processes, 1000, quantum);
            printOutput(res, 50);

        } else if (strcmp(argv[1], "SRT") == 0) {
            //also have to check if they added the alpha as an argument
            if (argc < 3){
                exit(1);
            } //Call SRT

            Results res = SRT(processes, 0.5, 1000);
            printOutput(res, 50);

        } else {
            //None of the algorithm names were used
            fprintf(stderr, "Unknown algorithm\n");
        }
        

        return 0;
}











