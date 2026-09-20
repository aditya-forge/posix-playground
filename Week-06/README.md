# CPU Scheduling Algorithms — Priority & Round Robin

Implementation of Non-Preemptive Priority, Preemptive Priority, and Round Robin
CPU scheduling for Programming Assignment 2.

## Files

| File | Description |
|---|---|
| `priority_non_preemptive.c` | Non-Preemptive Priority Scheduling |
| `priority_preemptive.c` | Preemptive Priority Scheduling |
| `round_robin.c` | Round Robin Scheduling with configurable time quantum |
| `processes_priority.txt` | Sample input for both priority algorithms |
| `processes_rr.txt` | Sample input for Round Robin |

## Compilation Instructions

Each program is self-contained and compiles independently with `gcc`:

```bash
gcc priority_non_preemptive.c -o priority_non_preemptive
gcc priority_preemptive.c -o priority_preemptive
gcc round_robin.c -o round_robin
```

No external libraries are required beyond the C standard library.

## Execution Instructions

Each program reads its input file from the current directory by default, but
an alternate file path can be passed as the first command-line argument.

```bash
./priority_non_preemptive                       # uses processes_priority.txt
./priority_non_preemptive myfile.txt             # uses a custom input file

./priority_preemptive                            # uses processes_priority.txt
./priority_preemptive myfile.txt

./round_robin                                    # uses processes_rr.txt (quantum from file)
./round_robin processes_rr.txt 5                 # override the quantum to 5
```

### Input file formats

`processes_priority.txt`:
```
<n>
<name> <arrival_time> <burst_time> <priority>   (repeated n times)
```

`processes_rr.txt`:
```
<n>
<name> <arrival_time> <burst_time>   (repeated n times)
<time_quantum>
```

## Algorithm Descriptions & Pseudo-code

### 1. Non-Preemptive Priority Scheduling

Lower priority number = higher priority. At every decision point the scheduler
picks the highest-priority process among those that have already arrived and
runs it to completion. Ties break on arrival time, then process name.

```
current_time = 0
while not all processes completed:
    candidates = processes that have arrived and are not yet completed
    if candidates is empty:
        current_time++
        continue
    selected = candidate with smallest priority
               (tie -> smallest arrival_time, then name)
    response_time[selected] = current_time - arrival_time[selected]
    current_time += burst_time[selected]
    completion_time[selected] = current_time
    mark selected completed
```

### 2. Preemptive Priority Scheduling

The scheduler re-evaluates every process every time unit. If a newly arrived
(or already-waiting) process has strictly higher priority than the one
currently running, the running process is preempted and returns to the ready
set with its remaining burst time; it competes again on the next tick like
any other ready process. Equal priority does not cause preemption.

```
current_time = 0
while not all processes completed:
    candidates = processes with remaining_time > 0 that have arrived
    if candidates is empty:
        current_time++
        continue
    selected = candidate with smallest priority
               (tie -> smallest arrival_time, then name)
    if selected is running for the first time:
        response_time[selected] = current_time - arrival_time[selected]
    run selected for 1 time unit
    remaining_time[selected]--
    current_time++
    if remaining_time[selected] == 0:
        completion_time[selected] = current_time
        mark selected completed
```

### 3. Round Robin Scheduling

A circular ready queue holds every arrived, unfinished process. Each turn the
process at the front of the queue runs for at most one time quantum. If it
still has remaining burst time, it is re-enqueued — but only after any
processes that arrived during its turn are enqueued first.

```
current_time = 0
enqueue all processes with arrival_time == 0
while not all processes completed:
    if queue is empty:
        current_time++
        enqueue any process that has now arrived
        continue
    current = dequeue()
    if current is running for the first time:
        response_time[current] = current_time - arrival_time[current]
    run = min(quantum, remaining_time[current])
    remaining_time[current] -= run
    current_time += run
    enqueue any process that arrived during this turn (before re-adding current)
    if remaining_time[current] == 0:
        completion_time[current] = current_time
        mark current completed
    else:
        enqueue(current)
```

## Complexity Analysis

Let **n** = number of processes and **T** = total burst time (sum of all
burst times).

| Algorithm | Time Complexity | Space Complexity | Notes |
|---|---|---|---|
| Non-Preemptive Priority | O(n²) | O(n) | One O(n) scan to pick the next process runs at most n times (once per process dispatched). |
| Preemptive Priority | O(n · T) | O(n) | An O(n) scan to pick the highest-priority ready process happens on every simulated time unit, and the simulation runs for T time units in the worst case. |
| Round Robin | O(T) | O(n) | Each time unit performs O(1) queue operations (enqueue/dequeue), and enqueue checks are amortized O(1) per process per pass; the queue itself uses O(n) space. |

In all three cases, reading the input file and computing final metrics
(turnaround, waiting, response) is an additional O(n) pass.

The preemptive version is asymptotically more expensive than the
non-preemptive version because it re-evaluates priorities on every time
tick rather than only at dispatch points; a priority-queue (min-heap)
implementation would bring this down to O(T log n), which is a natural
optimization if T is large.

## Time Quantum Effect on Round Robin

Using `processes_rr.txt` (P1–P5, arrival times 0–4, bursts 10/5/3/8/6), the
program was run with several quantum values via
`./round_robin processes_rr.txt <quantum>`:

| Quantum | Avg Turnaround | Avg Waiting | Avg Response | Context Switches |
|---|---|---|---|---|
| 1 | 22.80 | 16.40 | 1.20 | 31 |
| 2 | 23.40 | 17.00 | 2.80 | 16 |
| 3 | 22.20 | 15.80 | 4.60 | 11 |
| 4 | 23.00 | 16.60 | 5.60 | 9 |
| 5 | 20.80 | 14.40 | 7.20 | 7 |
| 10 | 18.20 | 11.80 | 11.80 | 4 |
| 100 | 18.20 | 11.80 | 11.80 | 4 |

**Observations**

- **Very small quantum (1):** Response time is excellent (processes get a
  taste of the CPU almost immediately), but context switches spike (31),
  which would carry real overhead on actual hardware even though this
  simulation assumes zero switching cost.
- **Small-to-moderate quantum (2–4):** Context switches drop quickly, but
  turnaround and waiting times don't necessarily improve monotonically —
  quantum 2 is actually a bit worse than quantum 1 and 3 here, showing that
  RR performance is sensitive to how burst times align with the queue
  rotation, not just the raw quantum size.
- **Larger quantum (5–10):** Turnaround and waiting times drop close to
  their best values as fewer processes need multiple rounds, but response
  time rises steadily since a process may now wait behind a full quantum
  of every other process before it is first scheduled.
- **Very large quantum (100 ≥ max burst time):** Round Robin degenerates
  to FCFS — every process finishes in a single quantum, so results at
  quantum 10 and 100 are identical (4 context switches, one per process
  boundary).

**Takeaway:** a small quantum favors responsiveness at the cost of
overhead; a large quantum favors throughput-style metrics at the cost of
responsiveness. A quantum in the range of a typical burst time (here,
around 5) tends to balance both reasonably well for this workload.

## Example Run

### `processes_priority.txt`
```
6
P1 0 10 3
P2 1 5 1
P3 2 3 5
P4 3 8 2
P5 4 6 4
P6 5 4 1
```

### Non-Preemptive Priority

```
$ gcc priority_non_preemptive.c -o priority_non_preemptive
$ ./priority_non_preemptive
==========================================
NON-PREEMPTIVE PRIORITY SCHEDULING RESULTS
==========================================
Process | Arrival | Burst | Priority | Completion | Turnaround | Waiting | Response
--------|---------|-------|----------|------------|------------|---------|---------
P1      | 0       | 10    | 3        | 10         | 10         | 0       | 0
P2      | 1       | 5     | 1        | 15         | 14         | 9       | 9
P3      | 2       | 3     | 5        | 36         | 34         | 31      | 31
P4      | 3       | 8     | 2        | 27         | 24         | 16      | 16
P5      | 4       | 6     | 4        | 33         | 29         | 23      | 23
P6      | 5       | 4     | 1        | 19         | 14         | 10      | 10

Average Turnaround Time : 20.83
Average Waiting Time    : 14.83
Average Response Time   : 14.83
Total Context Switches  : 5

Gantt Chart for Non-Preemptive Priority:
| P1 | P2 | P6 | P4 | P5 | P3 |
0      10   15   19   27   33   36

Context Switches : 5
```

### Preemptive Priority

```
$ gcc priority_preemptive.c -o priority_preemptive
$ ./priority_preemptive
==========================================
PREEMPTIVE PRIORITY SCHEDULING RESULTS
==========================================
Process | Arrival | Burst | Priority | Completion | Turnaround | Waiting | Response
--------|---------|-------|----------|------------|------------|---------|---------
P1      | 0       | 10    | 3        | 27         | 27         | 17      | 0
P2      | 1       | 5     | 1        | 6          | 5          | 0       | 0
P3      | 2       | 3     | 5        | 36         | 34         | 31      | 31
P4      | 3       | 8     | 2        | 18         | 15         | 7       | 7
P5      | 4       | 6     | 4        | 33         | 29         | 23      | 23
P6      | 5       | 4     | 1        | 10         | 5          | 1       | 1

Average Turnaround Time : 19.17
Average Waiting Time    : 13.17
Average Response Time   : 10.33
Total Context Switches  : 6

Gantt Chart for Preemptive Priority:
| P1 | P2 | P6 | P4 | P1 | P5 | P3 |
0       1    6   10   18   27   33   36
    ^    ^    ^    ^    ^    ^
```

### `processes_rr.txt`
```
5
P1 0 10
P2 1 5
P3 2 3
P4 3 8
P5 4 6
3
```

### Round Robin (Quantum = 3)

```
$ gcc round_robin.c -o round_robin
$ ./round_robin
==========================================
ROUND ROBIN SCHEDULING RESULTS (Quantum = 3)
==========================================
Process | Arrival | Burst | Completion | Turnaround | Waiting | Response
--------|---------|-------|------------|------------|---------|---------
P1      | 0       | 10    | 32         | 32         | 22      | 0
P2      | 1       | 5     | 20         | 19         | 14      | 2
P3      | 2       | 3     | 9          | 7          | 4       | 4
P4      | 3       | 8     | 31         | 28         | 20      | 6
P5      | 4       | 6     | 29         | 25         | 19      | 11

Average Turnaround Time : 22.20
Average Waiting Time    : 15.80
Average Response Time   : 4.60
Total Context Switches  : 11

Gantt Chart for Round Robin:
| P1 | P2 | P3 | P4 | P1 | P5 | P2 | P4 | P1 | P5 | P4 | P1 |
0       3    6    9   12   15   18   20   23   26   29   31   32
```
