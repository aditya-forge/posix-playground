# SRM University AP — Operating Systems

**Name:** Aditya Kumar  
**Registration No:** AP24110010333  
**Branch/Section:** BTech CSE - C  
**Institution:** SRM University AP  
**Course:** Operating Systems (CSE302)  
**Environment:** Ubuntu Linux / WSL (`aditya-forge@LAPTOP-184D82EB`)  
**Compiler:** GCC (`-Wall -Wextra -lm`)

---

## Overview

In previous lab modules, we explored fundamental CPU scheduling algorithms under idealized theoretical assumptions:
- **Week 05**: Classical non-preemptive First-Come-First-Served (FCFS), Shortest Job First (SJF), and basic Shortest Remaining Time First (SRTF) with zero context-switch overhead.
- **Week 06**: Static-priority (preemptive and non-preemptive) and fixed-quantum Round Robin (RR).
- **Week 07**: Highest Response Ratio Next (HRRN), balancing burst duration against waiting duration to mitigate starvation.

In **Week 10**, we bridge the gap between textbook models and realistic operating system kernels and real-time environments. We implement and analyze four advanced CPU scheduling designs:

1. **SRTF with Context Switch Overhead ($C$)**: Incorporates realistic context-switch latency penalties and models preemption dampening—a running process is preempted only if the remaining execution difference strictly offsets the switching cost.
2. **Round Robin with Dynamic Time Quantum**: Eliminates static time-quantum tuning bottlenecks by adaptively adjusting the quantum cycle-by-cycle using the median of ready queue bursts in the initial cycle and $\lceil \frac{\text{Average Remaining Burst}}{2} \rceil$ in successive cycles.
3. **HRRN with Priority Weighting & Anti-Starvation Aging**: Synthesizes priority-driven execution with aging heuristics through an Adjusted Response Ratio ($\text{ARR}$) and a hard starvation ceiling that dynamically overrides priority bias to prevent indefinite postponement.
4. **Rate Monotonic Real-Time Scheduling (RMS)**: Models fixed-priority periodic real-time tasks, validating schedulability using the Liu-Layland utilization bound ($U \le n(2^{1/n}-1)$), exact fixed-point Response-Time Analysis (RTA), and a discrete timeline simulator running over the task set hyperperiod ($H = \text{LCM}(T_1, \dots, T_n)$).

---

## Included Files

| File | Description |
|---|---|
| `srtf_cs.c` | Assignment 1: Shortest Remaining Time First simulator with context switch overhead $C$ and dampening preemption |
| `rr_dynamic.c` | Assignment 2: Round Robin scheduler with dynamic cycle-by-cycle quantum adaptation |
| `hrrn_priority.c` | Assignment 3: Priority-weighted HRRN with automated anti-starvation threshold overrides |
| `rms.c` | Assignment 4: Rate Monotonic real-time scheduler with Liu-Layland test, exact RTA, and hyperperiod simulation |
| `Makefile` | Build automation compiling all four targets with strict flags (`-Wall -Wextra -lm`) |
| `in1_a.txt`, `in1_b.txt`, `in1_c.txt` | Input test cases for Assignment 1 (varying context-switch overhead $C \in \{2, 1, 5\}$) |
| `in2_a.txt`, `in2_b.txt`, `in2_c.txt` | Input test cases for Assignment 2 (staggered arrivals, zero-time batch arrival, intermittent idle) |
| `in3_a.txt`, `in3_b.txt` | Input test cases for Assignment 3 (normal priority weighting vs. low-priority starvation trigger) |
| `in4_a.txt`, `in4_b.txt`, `in4_c.txt` | Input test cases for Assignment 4 (provably schedulable, overloaded, and bound-inconclusive schedulable) |
| `output_assignment1_srtf_cs.log` | Complete terminal execution logs across all test cases for Assignment 1 |
| `output_assignment2_rr_dynamic.log` | Complete terminal execution logs across all test cases for Assignment 2 |
| `output_assignment3_hrrn_priority.log` | Complete terminal execution logs across all test cases for Assignment 3 |
| `output_assignment4_rms.log` | Complete terminal execution logs across all test cases for Assignment 4 |
| `Week 10.pdf` | Comprehensive laboratory report detailing assignment specifications, results, and analyses |

---

## Assignment 1: SRTF with Context Switch Overhead

### 1. Mathematical Formulation & Preemption Rule
In real hardware, switching execution context between processes incurs cache flushes, TLB invalidation, register saves/restores, and PCB state updates. If context switching consumes $C$ time units:
- **Preemption Threshold**: A newly arrived ready process $P_{\text{new}}$ is granted CPU access over running process $P_{\text{curr}}$ **only if**:
  $$\text{rem}(P_{\text{curr}}) > \text{rem}(P_{\text{new}}) + C$$
  If this inequality does not hold, the overhead of switching outweighs the latency benefit of the shorter job, and $P_{\text{curr}}$ continues running.
- **Dispatch Timing**: When the CPU switches contexts (taking $C$ discrete units), the CPU remains dedicated to the switch. The actual process dispatched onto the CPU is selected **at the conclusion of the switch**, ensuring that any process arriving during the switch duration is also considered.
- **CPU Efficiency**:
  $$\text{CPU Efficiency} = \frac{\text{Total Busy Time}}{\text{Total Timeline Length}} \times 100\%$$

### 2. Experimental Cases & Outputs

#### Case 1: Standard Verification ($C = 2$)
- **Input (`in1_a.txt`)**: $P_1(0, 10)$, $P_2(4, 6)$, $P_3(5, 2)$, $C = 2$
- **Gantt Chart**:
  ```text
  0-CS-2-P1-5-CS-7-P3-9-CS-11-P2-17-CS-19-P1-26
  ```
- **Timeline Walkthrough**:
  - $t = 0$: Initial switch overhead of $C = 2$ units. $P_1$ begins execution at $t = 2$.
  - $t = 4$: $P_2(6)$ arrives. $P_1$ has $\text{rem} = 8$. Condition: $8 > 6 + 2 \implies 8 > 8$ (**False**). $P_1$ is **not** preempted.
  - $t = 5$: $P_3(2)$ arrives. $P_1$ has $\text{rem} = 7$. Condition: $7 > 2 + 2 \implies 7 > 4$ (**True**). $P_1$ is preempted!
  - Switch $5 \to 7$: At $t = 7$, $P_3$ is shortest, completes at $t = 9$.
  - Switch $9 \to 11$: $P_2$ runs to completion at $t = 17$.
  - Switch $17 \to 19$: $P_1$ finishes remaining 7 units at $t = 26$.
- **Performance Metrics**:
  - Context switches: `4` (Total switch time = `8`)
  - Busy time = `18`, Timeline length = `26`
  - Average TAT = `14.33`, Average WT = `8.33`
  - **CPU Efficiency = 69.23%**

#### Case 2: Four Processes with Low Overhead ($C = 1$)
- **Input (`in1_b.txt`)**: $P_1(0, 8)$, $P_2(1, 4)$, $P_3(2, 9)$, $P_4(3, 5)$, $C = 1$
- **Gantt Chart**:
  ```text
  0-CS-1-P2-5-CS-6-P4-11-CS-12-P1-20-CS-21-P3-30
  ```
- **Performance Metrics**:
  - Context switches: `4` (Total switch time = `4`)
  - Busy time = `26`, Timeline length = `30`
  - Average TAT = `15.00`, Average WT = `8.50`
  - **CPU Efficiency = 86.67%**

#### Case 3: High Overhead Penalty ($C = 5$)
- **Input (`in1_c.txt`)**: $P_1(0, 10)$, $P_2(4, 6)$, $P_3(5, 2)$, $C = 5$
- **Gantt Chart**:
  ```text
  0-CS-5-P3-7-CS-12-P2-18-CS-23-P1-33
  ```
- **Analysis**:
  - The initial switch consumes $0 \to 5$. By $t = 5$, both $P_2(4, 6)$ and $P_3(5, 2)$ have already arrived in the ready queue.
  - When the switch completes at $t = 5$, $P_3$ is directly selected without $P_1$ ever starting.
  - Context switches drop to `3`, but total switch duration surges to `15` units.
  - **CPU Efficiency plummets from 69.23% to 54.55%**.

---

## Assignment 2: Round Robin with Dynamic Time Quantum

### 1. Dynamic Adaptation Rules
Traditional Round Robin suffers from a fundamental trade-off: a small static time quantum ($TQ$) maximizes interactive responsiveness but explodes context-switch overhead; a large $TQ$ degenerates into FCFS. Dynamic Time Quantum adapts to queue workload cycle-by-cycle:
- **Cycle 1**: $TQ_1 = \text{Median of burst times in initial ready queue}$.
  - For odd $m$: $a[m/2]$ (sorted).
  - For even $m$: $\lceil \frac{a[m/2 - 1] + a[m/2]}{2} \rceil$.
- **Cycle $k > 1$**:
  $$TQ_{k} = \left\lceil \frac{\text{Average Remaining Burst of Ready Queue}}{2} \right\rceil = \left\lfloor \frac{\sum \text{rem} + 2N - 1}{2N} \right\rfloor$$
- **Empty Queue Invariant**: If the ready queue is empty upon CPU release, the previous cycle's $TQ$ is preserved for the next arrival.
- **Queue Discipline**: Processes arriving during an active cycle join the tail of the ready queue and are scheduled in the subsequent cycle.

### 2. Experimental Cases & Cycle Progression

#### Case 1: Staggered Arrival Workload
- **Input (`in2_a.txt`)**: $P_1(0, 12)$, $P_2(1, 8)$, $P_3(3, 18)$, $P_4(5, 2)$
- **Gantt Chart**:
  ```text
  0-P1-12-P2-17-P3-22-P4-24-P2-27-P3-40
  ```
- **Cycle Evolution**:
  | Cycle | $TQ$ | Ready Queue | Remaining Bursts | Action / Preemption |
  |---|---|---|---|---|
  | 1 | 12 | `[P1]` | `[12]` | $P_1$ completes entirely at $t = 12$ |
  | 2 | 5 | `[P2, P3, P4]` | `[8, 18, 2]` | $P_2 \to 3$ left; $P_3 \to 13$ left; $P_4$ completes |
  | 3 | 4 | `[P2, P3]` | `[3, 13]` | $P_2$ completes at $t = 27$; $P_3 \to 9$ left |
  | 4 | 5 | `[P3]` | `[9]` | $P_3 \to 4$ left |
  | 5 | 2 | `[P3]` | `[4]` | $P_3 \to 2$ left |
  | 6 | 1 | `[P3]` | `[2]` | $P_3 \to 1$ left |
  | 7 | 1 | `[P3]` | `[1]` | $P_3$ completes at $t = 40$ |
- **Averages**: Average TAT = `23.50`, Average WT = `13.50`.

#### Case 2: Simultaneous Batch Arrival at $t = 0$
- **Input (`in2_b.txt`)**: $P_1(0, 6)$, $P_2(0, 3)$, $P_3(0, 9)$, $P_4(0, 4)$
- **Gantt Chart**:
  ```text
  0-P1-5-P2-8-P3-13-P4-17-P1-18-P3-22
  ```
- **Cycle Evolution**:
  - Cycle 1: Queue `[P1, P2, P3, P4]` with bursts $[6, 3, 9, 4]$. Sorted: $[3, 4, 6, 9]$. Median $= \lceil \frac{4 + 6}{2} \rceil = 5$.
  - Cycle 2: Remaining `[P1, P3]` with bursts $[1, 4]$. Mean $= 2.5 \implies TQ = \lceil 1.25 \rceil = 2$.
  - Cycle 3 & 4: $P_3$ runs with $TQ = 1$ to completion.
- **Averages**: Average TAT = `16.25`, Average WT = `10.75`.

#### Case 3: Intermittent Idle CPU
- **Input (`in2_c.txt`)**: $P_1(0, 4)$, $P_2(10, 6)$, $P_3(11, 3)$
- **Gantt Chart**:
  ```text
  0-P1-4-IDLE-10-P2-14-P3-16-P2-18-P3-19
  ```
- **Analysis**: $P_1$ finishes at $t = 4$. CPU idles until $t = 10$. Because the queue was empty, Cycle 2 reuses $TQ = 4$ for $P_2$.

### 3. Critical Architectural Trade-Off
A significant insight revealed by Case 1 is the **tail quantum decay problem**: when only a single long process ($P_3$) remains in the queue, applying $\lceil \text{rem} / 2 \rceil$ repeatedly cuts the quantum in half ($5 \to 2 \to 1 \to 1$). While intended to prevent queue monopoly, when no other processes exist, this generates redundant cycle boundaries and scheduling overhead.

---

## Assignment 3: HRRN with Priority and Anti-Starvation

### 1. Algorithm & Anti-Starvation Mechanism
Standard Highest Response Ratio Next (HRRN) computes:
$$RR = \frac{W + B}{B} = 1 + \frac{W}{B}$$
To incorporate explicit process priorities ($PR \ge 1$, where 1 is highest priority), we introduce the **Priority Weight**:
$$\text{Priority Weight} = 1.0 + (PR - 1) \times 0.2$$
The **Adjusted Response Ratio (ARR)** becomes:
$$ARR = \frac{W + B}{B \times \text{Priority Weight}}$$

#### The Starvation Ceiling Rule
In high-load systems, low-priority long jobs can be indefinitely deferred if shorter, higher-priority jobs continuously arrive. To guarantee Dijkstra's Bounded Waiting criterion:
- **Threshold**: Set explicitly or defaulted to $2 \times \max(BT)$.
- **Override Trigger**: If any eligible process has waited $W > \text{Threshold}$, **priority weights are bypassed**.
- Only the starving processes compete, evaluated strictly by standard response ratio:
  $$RR = \frac{W + B}{B}$$
  This immediately forces the CPU to service starving tasks before resuming priority-weighted operation.

### 2. Experimental Cases & Scheduling Decisions

#### Case 1: Standard Priority Balancing (No Starvation)
- **Input (`in3_a.txt`)**: $P_1(0, 10, PR 1)$, $P_2(1, 50, PR 4)$, $P_3(2, 15, PR 2)$, Threshold $= 100$
- **Decisions**:
  - $t = 0$: $P_1$ is only arrival $\implies$ runs $0 \to 10$.
  - $t = 10$: $P_2$ has $W = 9, \text{wt} = 1.6 \implies ARR = \frac{59}{80} = 0.738$. $P_3$ has $W = 8, \text{wt} = 1.2 \implies ARR = \frac{23}{18} = 1.278$. $\implies$ **$P_3$ chosen (Highest ARR)**.
  - $t = 25$: $P_2$ runs $25 \to 75$.
- **Execution Order**: `P1 -> P3 -> P2`
- **Processes Triggering Anti-Starvation**: `None`
- Average TAT = `35.67`, Average WT = `10.67`.

#### Case 2: Hard Starvation Threshold Triggered
- **Input (`in3_b.txt`)**: 6 processes with low-priority $P_2(1, 20, PR 5)$ and Threshold $= 15$.
- **Decisions**:
  - $t = 0$: $P_1$ runs $0 \to 5$.
  - $t = 5$: $P_3(PR 1, ARR 1.750)$ selected over $P_2(ARR 0.667)$. Runs $5 \to 9$.
  - $t = 9$: $P_5(PR 1, ARR 2.667)$ selected over $P_2(ARR 0.778)$. Runs $9 \to 12$.
  - $t = 12$: $P_4(PR 2, ARR 2.083)$ selected over $P_2(ARR 0.861)$. Runs $12 \to 18$.
  - $t = 18$: **Anti-Starvation Triggered!** $P_2$ has waited $W = 17 > 15$.
    - Standard $RR(P_2) = \frac{17 + 20}{20} = 1.850$.
    - Standard $RR(P_6) = \frac{8 + 2}{2} = 5.000$ (not starving, $W=8 \le 15$).
    - $P_2$ is selected via **Anti-starvation override**! Runs $18 \to 38$.
  - $t = 38$: $P_6$ has waited $W = 28 > 15$ $\implies$ runs via anti-starvation $38 \to 40$.
- **Execution Order**: `P1 -> P3 -> P5 -> P4 -> P2 -> P6`
- **Processes Triggering Anti-Starvation**: `P2, P6`
- Average TAT = `17.00`, Average WT = `10.33`.

---

## Assignment 4: Rate Monotonic Scheduling (RMS) Analysis & Simulation

### 1. Theoretical Foundations
Rate Monotonic Scheduling (RMS) is an optimal static-priority preemptive scheduling discipline for independent periodic real-time tasks:
- **Priority Assignment**: Inversely proportional to task period ($T_i$). Shorter period $\implies$ higher priority.
- **Liu-Layland Utilization Bound**:
  For $n$ periodic tasks, schedulability is unconditionally guaranteed if total CPU utilization satisfies:
  $$U = \sum_{i=1}^{n} \frac{C_i}{T_i} \le U_{\text{bound}} = n \left(2^{1/n} - 1\right)$$
  As $n \to \infty$, $U_{\text{bound}} \to \ln 2 \approx 0.693$.
  - $U \le U_{\text{bound}}$: **Guaranteed Schedulable**.
  - $U > 1.0$: **Definitely Unschedulable** (overloaded).
  - $U_{\text{bound}} < U \le 1.0$: **Inconclusive** (requires exact Response-Time Analysis).

### 2. Exact Response-Time Analysis (RTA)
When the utilization test is inconclusive, exact worst-case response times are computed using fixed-point iteration over all higher-priority tasks $hp(i)$:
$$R_i^{(0)} = C_i$$
$$R_i^{(k+1)} = C_i + \sum_{j \in hp(i)} \left\lceil \frac{R_i^{(k)}}{T_j} \right\rceil C_j$$
The recurrence terminates when $R_i^{(k+1)} = R_i^{(k)}$ (converged) or $R_i > T_i$ (deadline miss). The task set is schedulable if and only if:
$$\forall i, \quad R_i \le T_i$$

### 3. Hyperperiod Simulation
The simulator generates job releases across the hyperperiod:
$$H = \text{LCM}(T_1, T_2, \dots, T_n)$$
At each unit of time, the highest-priority active job is granted CPU time. If a job fails to complete before $t = \text{release} + T$, it is flagged as a deadline miss. Late jobs continue to execute (non-drop policy).

### 4. Experimental Results

#### Case 1: Unconditionally Schedulable Task Set
- **Tasks**: $T_1(C=3, T=8)$, $T_2(C=2, T=10)$, $T_3(C=2, T=14)$
- **Utilization**: $U = \frac{3}{8} + \frac{2}{10} + \frac{2}{14} = 0.7179$
- **Liu-Layland Bound**: $3(2^{1/3} - 1) = 0.7798$
- **Test Verdict**: $U \le \text{bound} \implies$ **GUARANTEED SCHEDULABLE**
- **RTA Results**:
  - $T_1: R = 3 \le 8$
  - $T_2: R = 2 \to 5 \to 5 \le 10$
  - $T_3: R = 2 \to 7 \to 7 \le 14$
- **Simulation**: Hyperperiod $H = 280$. Max observed response times equal RTA ($T_1: 3, T_2: 5, T_3: 7$). **Zero missed deadlines**.

#### Case 2: Overloaded Task Set ($U > 1$)
- **Tasks**: $T_1(C=4, T=5)$, $T_2(C=3, T=7)$
- **Utilization**: $U = \frac{4}{5} + \frac{3}{7} = 1.2286 > 1.0$ $\implies$ **DEFINITELY UNSCHEDULABLE**
- **RTA Results**:
  - $T_1: R = 4 \le 5$
  - $T_2: R = 3 \to 7 \to 11 > 7 \implies$ **MISSES DEADLINE**
- **Simulation**: Hyperperiod $H = 35$. $T_2$ misses deadlines repeatedly at $t = 7, 14, 21, 28, 35$ with accumulated backlog.

#### Case 3: Inconclusive Bound but Fully Schedulable
- **Tasks**: $T_1(C=1, T=4)$, $T_2(C=2, T=6)$, $T_3(C=3, T=12)$
- **Utilization**: $U = \frac{1}{4} + \frac{2}{6} + \frac{3}{12} = 0.8333$
- **Liu-Layland Bound**: $0.7798$
- **Test Verdict**: $0.7798 < 0.8333 \le 1.0 \implies$ **INCONCLUSIVE (Check RTA)**
- **RTA Results**:
  - $T_1: R = 1 \le 4$
  - $T_2: R = 2 \to 3 \to 3 \le 6$
  - $T_3: R = 3 \to 6 \to 7 \to 9 \to 10 \to 10 \le 12 \implies$ **SCHEDULABLE**
- **Simulation**: Hyperperiod $H = 12$. Max observed response times ($1, 3, 10$) match RTA precisely. **Zero missed deadlines**.
- **Theoretical Insight**: This test demonstrates that the Liu-Layland utilization bound is **sufficient but not necessary**. Task sets exceeding the bound can frequently be scheduled without deadline misses.

---

## Build and Execution Guide

### Compilation
Build all four binaries simultaneously using the provided Makefile:
```bash
make all
```
Or compile individually:
```bash
gcc -Wall -Wextra -o srtf_cs srtf_cs.c
gcc -Wall -Wextra -o rr_dynamic rr_dynamic.c
gcc -Wall -Wextra -o hrrn_priority hrrn_priority.c
gcc -Wall -Wextra -o rms rms.c -lm
```

### Running Test Cases

#### Assignment 1: SRTF with Context Switch Overhead
```bash
./srtf_cs < in1_a.txt
./srtf_cs < in1_b.txt
./srtf_cs < in1_c.txt
```

#### Assignment 2: Dynamic Round Robin
```bash
./rr_dynamic < in2_a.txt
./rr_dynamic < in2_b.txt
./rr_dynamic < in2_c.txt
```

#### Assignment 3: Priority HRRN with Anti-Starvation
```bash
./hrrn_priority < in3_a.txt
./hrrn_priority < in3_b.txt
```

#### Assignment 4: Rate Monotonic Scheduling
```bash
./rms < in4_a.txt
./rms < in4_b.txt
./rms < in4_c.txt
```

### Cleanup
To remove compiled object files and binaries:
```bash
make clean
```

---

*Maintained by aditya-forge | SRM University AP — Operating Systems*
