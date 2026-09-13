# CPU Scheduling Algorithms - FCFS, SJF, SRTF

**Name:** Aditya Kumar
**Registration No:** AP24110010333
**Branch/Section:** BTech CSE - C

A C program that simulates FCFS (First Come First Serve), SJF (Shortest Job First, non-preemptive), and SRTF (Shortest Remaining Time First, preemptive) CPU scheduling algorithms. The program reads process information (name, arrival time, burst time) from an input file and, for each algorithm, works out the Completion Time, Turnaround Time, Waiting Time, Response Time, their averages, and a Gantt chart.

---

## 1. Compilation Instructions

The program is written in standard C (stdio.h, stdlib.h, string.h, stdbool.h) and can be compiled with gcc on Linux, macOS, or WSL/MinGW on Windows.

```bash
gcc scheduler.c -o scheduler
```

This produces an executable named `scheduler` (or `scheduler.exe` on Windows).

## 2. Execution Instructions

Run the compiled executable with the input file name as a command line argument:

```bash
./scheduler processes1.txt
```

If no argument (or more than one) is supplied, the program prints a usage message and exits:

```
Usage: ./scheduler processes1.txt
```

### Input File Format

```
Number_of_Processes
Process_Name Arrival_Time Burst_Time
Process_Name Arrival_Time Burst_Time
...
```

Example (`processes1.txt`):

```
3
P1 0 10
P2 0 5
P3 0 8
```

The program then prints, one after another, the results for FCFS, SJF, and SRTF, each showing a results table, the four average metrics, and a Gantt chart.

## 3. Algorithm Description

### FCFS (First Come First Serve), Non-preemptive
Processes are sorted by arrival time and executed strictly in that order. A process runs to completion once the CPU is allocated to it; if the CPU is idle before the next process arrives, time is advanced to that arrival. It's simple to implement but can suffer from the convoy effect, where a long process delays several shorter ones behind it.

### SJF (Shortest Job First), Non-preemptive
Among all processes that have arrived and are not yet completed, the one with the smallest burst time is picked next. Once selected, it runs to completion without preemption. If the CPU is idle (no process has arrived yet), time jumps forward to the next arrival. This algorithm gives a lower average waiting time among non-preemptive strategies, but it needs burst times to be known in advance, and it can end up starving longer processes.

### SRTF (Shortest Remaining Time First), Preemptive
This is the preemptive version of SJF. At every unit of time (effectively, at every arrival), the scheduler re-checks all arrived but incomplete processes and runs the one with the smallest remaining time. If a newly arrived process has a shorter remaining time than the one currently executing, the CPU is preempted and switched over to the new process. Ties are broken in favour of continuing the process that is already running. This generally gives the lowest average waiting and turnaround time of the three, but it comes with the overhead of more frequent context switches.

### Common Metrics
- Completion Time (CT): the time at which a process finishes execution.
- Turnaround Time (TAT): CT minus Arrival Time.
- Waiting Time (WT): TAT minus Burst Time.
- Response Time (RT): the time from arrival until the process is first given the CPU. This only differs from waiting time when a process is preempted and resumed later, as happens in SRTF.

## 4. Complexity Analysis

Let n be the number of processes.

| Algorithm | Time Complexity | Space Complexity | Notes |
|---|---|---|---|
| FCFS | O(n^2) for the bubble sort by arrival time (O(n log n) is achievable with a better sort); O(n) for the scheduling pass itself | O(n) | A single pass through the sorted list |
| SJF | O(n^2), since for each of the n selections an O(n) scan is done to find the shortest job among the processes that have arrived | O(n) | Needs a small boolean array to track which processes are already done |
| SRTF | O(n^2) in the worst case, since the simulation advances time unit by unit and, at each unit, scans all n processes to find the one with the shortest remaining time | O(n), plus a bit more for storing Gantt chart segments | The most expensive of the three because of how often it checks for preemption; could be brought down to O(n log n) with a min-heap |

calculate_metrics() and display_results() are both O(n) and run once per algorithm, so they don't change the overall complexity above.

## 5. Example Run and Output

**Input (`processes1.txt`):**
```
3
P1 0 10
P2 0 5
P3 0 8
```

**Command:**
```bash
aditya-forge@LAPTOP-184D8EB:~$ gcc scheduler.c -o scheduler
aditya-forge@LAPTOP-184D8EB:~$ ./scheduler processes1.txt
```

**Output (FCFS):**
```
==============================================================
FCFS SCHEDULING RESULTS
==============================================================
Process | Arrival | Burst | Completion | Turnaround | Waiting | Response
-----------------------------------------------------------------------
P1      |       0 |    10 |         10 |         10 |       0 |        0
P2      |       0 |     5 |         15 |         15 |      10 |       10
P3      |       0 |     8 |         23 |         23 |      15 |       15
-----------------------------------------------------------------------
Average Turnaround Time : 16.00
Average Waiting Time    : 8.33
Average Response Time   : 8.33

FCFS Gantt Chart:

| P1 | P2 | P3 |
0   10   15   23
```

**Output (SRTF):**
```
==============================================================
SRTF SCHEDULING RESULTS
==============================================================
Process | Arrival | Burst | Completion | Turnaround | Waiting | Response
-----------------------------------------------------------------------
P1      |       0 |    10 |         23 |         23 |      13 |       13
P2      |       0 |     5 |          5 |          5 |       0 |        0
P3      |       0 |     8 |         13 |         13 |       5 |        5
-----------------------------------------------------------------------
Average Turnaround Time : 13.67
Average Waiting Time    : 6.00
Average Response Time   : 6.00

SRTF Gantt Chart:

| P2 | P3 | P1 |
0   5    13   23
```

**Output (SJF):**
```
==============================================================
SJF SCHEDULING RESULTS
==============================================================
Process | Arrival | Burst | Completion | Turnaround | Waiting | Response
-----------------------------------------------------------------------
P1      |       0 |    10 |         23 |         23 |      13 |       13
P2      |       0 |     5 |          5 |          5 |       0 |        0
P3      |       0 |     8 |         13 |         13 |       5 |        5
-----------------------------------------------------------------------
Average Turnaround Time : 13.67
Average Waiting Time    : 6.00
Average Response Time   : 6.00

SJF Gantt Chart:

| P2 | P3 | P1 |
0   5    13   23
```

Two more test cases (`processes2.txt`, with staggered arrivals, and `processes3.txt`, with six processes) were also run to check correctness across FCFS, SJF, and SRTF. Results for both are included in the lab report.

## 6. Files in this Submission

| File | Description |
|---|---|
| `scheduler.c` | C source code implementing FCFS, SJF, and SRTF |
| `processes1.txt` | Sample input, 3 processes, all arriving at time 0 |
| `processes2.txt` | Sample input, 4 processes with staggered arrival times |
| `processes3.txt` | Sample input, 6 processes (complex scenario) |
| `README.md` | This file |
