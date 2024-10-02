**Uniprocessor Scheduling Algorithms Simulator**

**Overview**

This project implements a series of uniprocessor scheduling algorithms in C, designed to simulate and analyze the performance of various non-preemptive and preemptive scheduling strategies. The goal is to compare key performance metrics, such as average waiting time, turnaround time, and response time, for different scheduling approaches. The assignment also explores exponential averaging to predict CPU burst lengths in preemptive scheduling.

**Algorithms Implemented**

**Non-Preemptive Scheduling Algorithms**
-   First-Come-First-Served (FCFS):
    This algorithm schedules processes based on their arrival time. It is simple and easy to implement but can lead to the convoy effect, where short jobs wait behind long jobs.

- Shortest Job First (SJF):
    Also known as Shortest-Process-Next (SPN), this algorithm schedules the process with the shortest CPU burst next. It is optimal in terms of minimizing the average waiting time but requires knowledge of the burst length, which is simulated here.

- Non-Preemptive Priority Scheduling:
  Processes are assigned priorities based on their process ID, calculated as 1 / Process#. Processes with lower IDs have higher priority. This algorithm selects the highest priority process at each scheduling decision.

**Preemptive Scheduling Algorithms**
- Round Robin (RR):
  The time quantum (quanta) is a key parameter in this algorithm, where each process is given a fixed amount of CPU time before being preempted and placed back in the queue. The simulator runs the RR algorithm with various time quanta (from 1 to 100) to compare performance metrics.

- Shortest Remaining Time (SRT) with Exponential Averaging:
  This preemptive version of SJF uses exponential averaging to predict the length of the next CPU burst. The simulator tests different values of the alpha parameter (α) to explore its effect on performance.

**Input and Output**

Input Format: The input file contains 1000 lines, each representing a process with the following format:

<ProcessID, Arrival Time, Time Until First Response, CPU Burst Length>

**ProcessID**: An integer between 1 and 50.
**Arrival Time**: An integer between 0 and 5000.
**CPU Burst Length**: An integer between 1 and 200.
**Time Until First Response**: Used to calculate the average response time.

**Output Format**
For each scheduling algorithm, the output shows:

- The sequence of processes as they are scheduled.
- A table with details for each process, sorted by ProcessID, including:

ProcessID
Arrival Time
Waiting Time
Turnaround Time
Response Time

- Average waiting, turnaround and response times

**How to Run the Simulators**
Each scheduling algorithm can be run using the following commands:

**FCFS**

./scheduler FCFS < inputfile.txt

**SJF:**
./scheduler SJF < inputfile.txt

**Priority Scheduling:**

./scheduler Priority < inputfile.txt

**Round Robin (RR) with Time Quantum:**
./scheduler RR <Quantum> < inputfile.txt

**Shortest Remaining Time (SRT) with Exponential Averaging:**
./scheduler SRT <Alpha> < inputfile.txt


**C-based Simulations** The project is built entirely in C, leveraging low-level programming concepts and efficient memory management.
**Scheduling Algorithm Comparisons:** Side-by-side comparison of popular uniprocessor scheduling algorithms under different scenarios.
**Dynamic Graphing:** The project generates insightful graphs to visualize how different scheduling parameters affect system performance.
**Exponential Averaging** Explore the impact of exponential averaging on the predictability of CPU bursts.

**Conclusion**
This project showcases practical implementations of fundamental OS scheduling algorithms, focusing on performance analysis and parameter tuning. It serves as a foundation for understanding the complexities of process scheduling and real-world system performance.
