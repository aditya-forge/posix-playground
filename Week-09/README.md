# Week 09: Process Synchronization — Hardware Atomic Primitives (TAS & CAS)

**Name:** Aditya Kumar  
**Registration No:** AP24110010333  
**Branch/Section:** BTech CSE - C  
**Institution:** SRM University AP  
**Course:** Operating Systems (CSE302)  
**Environment:** Ubuntu Linux / WSL (`aditya-forge@LAPTOP-184D82EB`)  
**Compiler:** GCC (`-pthread -Wall -Wextra`)

---

## Overview

In **Week 08**, we observed how unsynchronized concurrent access to shared memory inevitably triggers race conditions, leading to data corruption and lost updates in the Producer-Consumer problem.

In **Week 09**, we address process synchronization at the fundamental hardware-software boundary. We implement a multi-threaded bounded-buffer Producer-Consumer system in C using POSIX threads (`pthreads`), synchronized entirely through **hardware atomic instructions** (`test_and_set` and `compare_and_swap`) via C11 `<stdatomic.h>`. 

Per assignment requirements, **no high-level OS synchronization primitives** (such as `pthread_mutex_t`, semaphores `sem_t`, or language monitors) are used. The synchronization locks are constructed directly from atomic memory primitives.

---

## Included Files

| File | Description |
|---|---|
| `producer_consumer.c` | Core implementation containing bounded-buffer, atomic spinlocks, threads, and invariant checks |
| `Makefile` | Automates build targets for Case 1 (`pc_tas`), Case 2 (`pc_cas`), and Case 3 (`pc_nolock`) |
| `output_case1_test_and_set.log` | Execution output verifying mutual exclusion using `test_and_set` (3 producers, 3 consumers) |
| `output_case2_compare_and_swap.log` | Execution output verifying mutual exclusion using `compare_and_swap` (3 producers, 3 consumers) |
| `output_case3_no_lock.log` | Execution logs of the no-lock experiment showing clean run vs silent race corruption |
| `OS_LAB_9_Report.pdf` | Formatted lab report detailing experiment theory, verification, and analytical questions |

---

## Synchronization Primitives & Architecture

### 1. Atomic Primitives (`<stdatomic.h>`)

The hardware spinlock relies on two low-level atomic primitives:

- **Test-and-Set (`test_and_set`)**:
  ```c
  bool test_and_set(atomic_bool *target) {
      return atomic_exchange(target, true); // Indivisibly sets to true and returns OLD value
  }
  ```
- **Compare-and-Swap (`compare_and_swap`)**:
  ```c
  bool compare_and_swap(atomic_bool *target, bool expected, bool new_val) {
      return atomic_compare_exchange_strong(target, &expected, new_val);
  }
  ```

### 2. Lock Acquisition and Release

- **Acquire (`acquire`)**:
  ```c
  void acquire(atomic_bool *l) {
  #if USE_CAS
      while (!compare_and_swap(l, false, true)) {
          // CAS failed -> lock is held by another thread -> spin / retry
      }
  #else
      while (test_and_set(l)) {
          // Busy-wait: spin until old value returned is false (lock acquired)
      }
  #endif
  }
  ```
- **Release (`release`)**:
  ```c
  void release(atomic_bool *l) {
      atomic_store(l, false); // Atomically reset lock state to false
  }
  ```

### 3. Critical Section Protection & Invariant Verification

- **Buffer Configuration**: Circular buffer of size $N = 5$ (`BUFFER_SIZE`).
- **Thread Count**: 3 Producers and 3 Consumers (or 1 Producer and 1 Consumer for Case 3). Each thread processes 10 items.
- **Safety Invariant**:
  $$\forall t, \quad 0 \le \text{counter} \le \text{BUFFER\_SIZE}$$
  Inside the locked critical section, an assertion verifies that the counter never drops below 0 (underflow) or exceeds `BUFFER_SIZE` (overflow).
- **Post-Condition Check**:
  $$\text{Final counter} == 0, \quad \text{Total Produced} == \text{Total Consumed}$$

---

## Experimental Cases & Findings

### Case 1: Test-and-Set Lock (`USE_CAS=0`, `USE_LOCK=1`)
- **Build**: `gcc -pthread -Wall -Wextra -DUSE_CAS=0 producer_consumer.c -o pc_tas`
- **Setup**: 3 Producers, 3 Consumers, 10 items each (30 items total).
- **Results**:
  - Total items produced: `30`
  - Total items consumed: `30`
  - Maximum counter observed: `5` (strictly within limit of 5)
  - Invariant assertion failures: `0`
  - Final counter: `0` (clean termination, mutual exclusion verified)

### Case 2: Compare-and-Swap Lock (`USE_CAS=1`, `USE_LOCK=1`)
- **Build**: `gcc -pthread -Wall -Wextra -DUSE_CAS=1 producer_consumer.c -o pc_cas`
- **Setup**: 3 Producers, 3 Consumers, 10 items each (30 items total).
- **Results**:
  - Total items produced: `30`
  - Total items consumed: `30`
  - Maximum counter observed: `5` (strictly within limit of 5)
  - Invariant assertion failures: `0`
  - Final counter: `0` (clean termination, mutual exclusion verified)

### Case 3: No-Lock Race Experiment (`USE_LOCK=0`, 1 Producer, 1 Consumer)
- **Build**: `gcc -pthread -Wall -Wextra -DUSE_LOCK=0 -DNUM_PRODUCERS=1 -DNUM_CONSUMERS=1 producer_consumer.c -o pc_nolock`
- **Setup**: `LOCK()` and `UNLOCK()` macros are disabled.
- **Results & Analysis**:
  - In clean runs with minimal thread contention, the final counter ended at 0.
  - However, across repeated runs, unsynchronized interleaving of `counter++` and `counter--` produced **lost updates**.
  - In corrupted runs, the execution terminated with `Final counter = 1 (expected 0)`.
  - This directly confirms the presence of race conditions in unprotected critical sections.

---

## Analytical Discussion & Lab Report Q&A

### 1. Why must `test_and_set` be atomic?
`test_and_set` must read the previous value of the lock and write `true` as an **indivisible, single-cycle or hardware-locked transaction**. 

If the operation were non-atomic (implemented as separate read and write instructions):
1. Thread A reads `lock == false`.
2. A context switch occurs before Thread A writes `true`.
3. Thread B runs, reads `lock == false`, sets `lock = true`, and enters the critical section.
4. Thread A resumes, executes its delayed write `lock = true`, and enters the critical section simultaneously.

Both threads believe they hold exclusive access, completely breaking **mutual exclusion**. Hardware atomicity ensures that exactly one thread reads `false` and transitions the state to `true`, while all other competing threads read `true` and are forced to spin.

---

### 2. TAS vs. CAS: Memory Writes and Bus Traffic on Failure Paths

| State / Path | Test-and-Set (`atomic_exchange`) | Compare-and-Swap (`atomic_compare_exchange_strong`) |
|---|---|---|
| **Lock is Free** | Reads `false`, writes `true` (Acquired) | Compares `false == false`, writes `true` (Acquired) |
| **Lock is Held (Failure Path)** | Reads `true`, **still writes `true`** (1 write per spin) | Compares `true != false`, **aborts without writing** (0 writes per spin) |
| **Effect on Memory / Cache** | Incurs continuous cache-line invalidation on the bus | Can spin in shared/read-only cache state |

#### Architectural Significance
Under cache-coherence protocols (such as MESI/MOESI), any memory write requires the processor core to obtain exclusive ownership (`Modified`/`Exclusive`) of the cache line containing the lock variable. This forces the bus to broadcast invalidation signals to all other processor cores.

- With **`test_and_set`**, every spin cycle performs a write—even when the lock remains held. This saturates the memory bus with bus invalidation traffic (bus storm).
- With **`compare_and_swap`**, a failure does not issue a store instruction, allowing the core to spin on the cached read-only value without invalidating peers.
- *Hardware Note:* On modern x86 architectures, `LOCK CMPXCHG` still asserts exclusive bus/cache line locking during execution, so the real-world reduction in contention is maximized when combined with *test-and-test-and-set* (polling with normal memory reads before attempting atomic instructions).

---

### 3. Does this lock guarantee Bounded Waiting?

**No.** Neither the `test_and_set` nor the `compare_and_swap` spinlock satisfies Dijkstra's **Bounded Waiting** criterion.

- **Mutual Exclusion**: Satisfied. At most one thread enters the critical section.
- **Progress**: Satisfied. If no thread is in the critical section, a waiting thread will eventually acquire the lock.
- **Bounded Waiting**: **Violated**. Spinlocks do not maintain an arrival queue or fairness policy. When the lock variable transitions from `true` to `false`, all spinning threads race simultaneously. A thread may repeatedly lose this race indefinitely, leading to **starvation**.
- **Remedy**: Bounded waiting requires explicit ordering mechanisms, such as:
  - Ticket Locks (FIFO order using `fetch_and_add`)
  - Peterson's algorithm or bounded-waiting TAS using a `waiting[n]` flag array.

---

### 4. Why didn't the $0 \le \text{counter} \le 5$ assertion fail in the 1-Producer/1-Consumer No-Lock test?

In repeated executions of Case 3 (1 producer, 1 consumer without synchronization), the range assertion `assert(counter >= 0 && counter <= BUFFER_SIZE)` did not trigger, yet the final result showed `Final counter = 1 (expected 0)`.

#### Root Cause:
1. `counter++` and `counter--` are three-step operations at the assembly level:
   ```text
   mov eax, [counter]    ; Load counter into register
   add eax, 1            ; Modify value
   mov [counter], eax    ; Store back to memory
   ```
2. When the producer and consumer overlap, a context switch during these three steps causes a **lost update**. The counter value drifts away from the actual number of buffered elements.
3. With only **one producer and one consumer**, the maximum rate of drift in any single scheduling slice is typically $\pm 1$. Because the buffer size is $N = 5$, a drift of 1 or 2 rarely pushes the counter outside the broad interval $[0, 5]$.
4. The range assertion is therefore an **insensitive detector**: it only catches gross overflow/underflow, not subtle corruption. The post-condition check `Final counter == 0` is the conclusive indicator of state desynchronization.

---

## Compilation and Execution

### Using Makefile

```bash
# Build all binaries (pc_tas, pc_cas, pc_nolock)
aditya-forge@LAPTOP-184D82EB:~/os_lab/Week-09$ make all

# Build individual targets
aditya-forge@LAPTOP-184D82EB:~/os_lab/Week-09$ make pc_tas
aditya-forge@LAPTOP-184D82EB:~/os_lab/Week-09$ make pc_cas
aditya-forge@LAPTOP-184D82EB:~/os_lab/Week-09$ make pc_nolock

# Clean built binaries
aditya-forge@LAPTOP-184D82EB:~/os_lab/Week-09$ make clean
```

### Manual Compilation

```bash
# Case 1: Test-and-Set
gcc -pthread -Wall -Wextra -DUSE_CAS=0 producer_consumer.c -o pc_tas

# Case 2: Compare-and-Swap
gcc -pthread -Wall -Wextra -DUSE_CAS=1 producer_consumer.c -o pc_cas

# Case 3: No-lock race experiment
gcc -pthread -Wall -Wextra -DUSE_LOCK=0 -DNUM_PRODUCERS=1 -DNUM_CONSUMERS=1 producer_consumer.c -o pc_nolock
```

---

## Sample Execution Outputs

### Case 1: Test-and-Set Verification (`./pc_tas`)
```text
aditya-forge@LAPTOP-184D82EB:~/os_lab$ ./pc_tas
Lock type : test_and_set (USE_CAS 0)
Lock used : yes
Config    : 3 producer(s), 3 consumer(s), 10 items each, buffer size 5

Producer 1 produced item 100 | counter = 1
Producer 1 produced item 101 | counter = 2
Producer 1 produced item 102 | counter = 3
Producer 1 produced item 103 | counter = 4
Producer 1 produced item 104 | counter = 5
Consumer 1 consumed item 100 | counter = 4
Consumer 1 consumed item 101 | counter = 3
...
Producer 3 produced item 309 | counter = 3
Consumer 3 consumed item 207 | counter = 2
Consumer 3 consumed item 208 | counter = 1
Consumer 3 consumed item 209 | counter = 0

Total produced = 30
Total consumed = 30
Max counter observed = 5 (limit 5)

Final counter = 0 (expected 0)
```

### Case 2: Compare-and-Swap Verification (`./pc_cas`)
```text
aditya-forge@LAPTOP-184D82EB:~/os_lab$ ./pc_cas
Lock type : compare_and_swap (USE_CAS 1)
Lock used : yes
Config    : 3 producer(s), 3 consumer(s), 10 items each, buffer size 5

Producer 2 produced item 200 | counter = 1
Producer 2 produced item 201 | counter = 2
Producer 2 produced item 202 | counter = 3
Producer 2 produced item 203 | counter = 4
Producer 2 produced item 204 | counter = 5
Consumer 1 consumed item 200 | counter = 4
...
Producer 3 produced item 309 | counter = 5
Consumer 3 consumed item 307 | counter = 2
Consumer 3 consumed item 308 | counter = 1
Consumer 3 consumed item 309 | counter = 0

Total produced = 30
Total consumed = 30
Max counter observed = 5 (limit 5)

Final counter = 0 (expected 0)
```

### Case 3: Race Condition Demonstration (`./pc_nolock`)
```text
aditya-forge@LAPTOP-184D82EB:~/os_lab$ ./pc_nolock
Lock type : test_and_set (USE_CAS 0)
Lock used : NO (race experiment)
Config    : 1 producer(s), 1 consumer(s), 10 items each, buffer size 5

Producer 1 produced item 100 | counter = 1
Consumer 1 consumed item 100 | counter = 0
...
Consumer 1 consumed item 108 | counter = 1
Consumer 1 consumed item 109 | counter = 1

Total produced = 10
Total consumed = 10
Max counter observed = 2 (limit 5)
Invariant violations detected = 0

Final counter = 1 (expected 0)
```
*(Notice: Unsynchronized access resulted in a lost update, terminating with `Final counter = 1` instead of `0`.)*

---

*Maintained by aditya-forge | SRM University AP — Operating Systems*
