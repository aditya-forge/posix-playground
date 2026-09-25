# CPU Scheduling — Highest Response Ratio Next (HRRN)

**Name:** Aditya Kumar
**Registration No:** AP24110010333
**Branch/Section:** BTech CSE - C

Implementation of the Highest Response Ratio Next (HRRN) CPU scheduling
algorithm for Programming Assignment 3.

## Files

| File | Description |
|---|---|
| `hrrn.c` | HRRN (Non-Preemptive) CPU Scheduling implementation |
| `processes_hrrn.txt` | Sample input (4 processes with staggered arrivals) |
| `CSE302_CPU_Scheduling - HRRN.pdf` | Lecture reference on HRRN scheduling |
| `OS_LAB_7.pdf` | Lab report for Week 07 |

## Compilation Instructions

The program is self-contained and compiles with `gcc`:

```bash
gcc hrrn.c -o hrrn
```

No external libraries are required beyond the C standard library.

## Execution Instructions

The program reads its input from `processes_hrrn.txt` by default, but an
alternate file path can be passed as the first command-line argument.

```bash
./hrrn                          # uses processes_hrrn.txt
./hrrn myfile.txt               # uses a custom input file
```

### Input File Format

```
<n>
<name> <arrival_time> <burst_time>   (repeated n times)
```

Example (`processes_hrrn.txt`):
```
4
P1 0 4
P2 1 6
P3 2 2
P4 3 5
```

## Algorithm Description

### Highest Response Ratio Next (HRRN), Non-Preemptive

HRRN is a non-preemptive scheduling algorithm that selects the process with
the highest response ratio at every scheduling decision point. Each ready
process's response ratio is computed as:

```
Response Ratio = (Waiting Time + Burst Time) / Burst Time
               = 1 + (Waiting Time / Burst Time)
```

A process that has waited a long time accumulates a growing waiting time,
which pushes its ratio up even if its burst is long — this "aging" effect
prevents starvation. A short burst time also inflates the ratio, so HRRN
naturally favors short jobs the way SJF does, but without permanently
starving long ones.

**Tie-breaking** when two ready processes have the same response ratio:
1. Prefer the smaller burst time.
2. If still tied, prefer the smaller arrival time.
3. If still tied, prefer the smaller PID.

### Pseudo-code

```
current_time = 0
while not all processes completed:
    candidates = processes that have arrived and are not yet completed
    if candidates is empty:
        current_time = next arrival time (jump forward)
        continue
    for each candidate:
        rr = (current_time - arrival_time + burst_time) / burst_time
    selected = candidate with highest rr
               (tie -> smallest burst_time, then smallest arrival_time, then smallest PID)
    response_time[selected] = current_time - arrival_time[selected]
    current_time += burst_time[selected]
    completion_time[selected] = current_time
    mark selected completed
```

### Common Metrics
- **Completion Time (CT):** the time at which a process finishes execution.
- **Turnaround Time (TAT):** CT minus Arrival Time.
- **Waiting Time (WT):** TAT minus Burst Time.
- **Response Time (RT):** the time from arrival until the process is first given the CPU. Since HRRN is non-preemptive, RT equals WT for every process.

## Complexity Analysis

Let **n** = number of processes.

| Aspect | Complexity | Notes |
|---|---|---|
| Time | O(n²) | At each of the n scheduling decisions, an O(n) scan computes the response ratio for every ready process and picks the highest. |
| Space | O(n) | Storage for n process records and the execution order array. |

Reading the input file and computing final metrics (turnaround, waiting,
response) is an additional O(n) pass that does not affect the overall
quadratic bound.

## Comparison with Other Algorithms

| Algorithm | Type | Starvation? | Key Property |
|---|---|---|---|
| FCFS | Non-preemptive | No | Simple but suffers from convoy effect |
| SJF | Non-preemptive | Yes | Optimal average waiting time but starves long jobs |
| SRTF | Preemptive | Yes | Preemptive version of SJF; lowest average wait |
| Priority | Both | Yes | Explicit priority values; can starve low-priority |
| Round Robin | Preemptive | No | Fair time-sharing; sensitive to quantum choice |
| **HRRN** | **Non-preemptive** | **No** | **Balances SJF-like short-job preference with aging to prevent starvation** |

HRRN combines the best aspects of SJF (favoring short bursts) with a
built-in aging mechanism (the response ratio grows over time for waiting
processes), making it starvation-free while still keeping average waiting
times low.

## Example Run

### `processes_hrrn.txt`
```
4
P1 0 4
P2 1 6
P3 2 2
P4 3 5
```

### HRRN Scheduling

```
$ gcc hrrn.c -o hrrn
$ ./hrrn

Gantt Chart :

| P1 | P3 | P2 | P4 |
0 4 6 12 17

PID   AT   BT   CT   TAT   WT   RT
1     0    4    4    4     0    0
2     1    6    12   11    5    5
3     2    2    6    4     2    2
4     3    5    17   14    9    9

Average Turnaround Time : 8.25
Average Waiting Time    : 4.00
Average Response Time   : 4.00
```

**Walkthrough:**

- **t = 0:** Only P1 has arrived → dispatch P1 (burst 4), completes at t = 4.
- **t = 4:** P2, P3, P4 have all arrived by now.
  - P2: RR = (3 + 6)/6 = 1.50
  - P3: RR = (2 + 2)/2 = **2.00** ← highest
  - P4: RR = (1 + 5)/5 = 1.20
  - Select P3, runs to completion at t = 6.
- **t = 6:** P2, P4 remaining.
  - P2: RR = (5 + 6)/6 = **1.83** ← highest
  - P4: RR = (3 + 5)/5 = 1.60
  - Select P2, runs to completion at t = 12.
- **t = 12:** Only P4 remaining → dispatch P4, completes at t = 17.

The aging effect is visible: P3 (shortest burst) runs first at t = 4, but
at t = 6, P2's longer wait bumps its ratio above P4's even though P4 has
the shorter burst. Without aging (plain SJF), P4 would have run before P2.
