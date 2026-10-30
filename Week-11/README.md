# Week 11: Process Synchronization — POSIX Semaphores & Deadlock Demonstration

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

In **Week 09**, we addressed synchronization at the hardware-software boundary by constructing mutual-exclusion spinlocks from **hardware atomic primitives** (`test_and_set` and `compare_and_swap`) using C11 `<stdatomic.h>`. While these low-level spinlocks enforce mutual exclusion, they suffer from **busy-waiting**, lack **bounded waiting** guarantees, and provide no mechanism for thread blocking or signaling when the buffer is full or empty.

In **Week 11**, we elevate the synchronization abstraction to **POSIX counting semaphores** (`<semaphore.h>`). Semaphores solve the bounded-buffer Producer-Consumer problem completely by enforcing:

1. **Mutual Exclusion**: A binary semaphore (`mutex`) protects the critical section (buffer, indices, counter) from concurrent thread access.
2. **Synchronization / Condition Signaling**: Two counting semaphores (`empty_slots` and `full_slots`) encode buffer capacity constraints directly, enabling threads to **block** (rather than spin-wait) when the buffer is full or empty.
3. **Deadlock Vulnerability**: We demonstrate that **incorrect semaphore acquisition ordering** (taking `mutex` before the counting semaphore) produces a classic **deadlock**, where all threads are permanently blocked waiting on resources held by each other.

---

## Included Files

| File | Description |
|---|---|
| `producer_consumer_sem.c` | Core implementation: bounded-buffer Producer-Consumer with POSIX semaphores, configurable via compile-time macros |
| `Makefile` | Build automation for all three experimental cases (`pc_sem`, `pc_sem_1p1c`, `pc_sem_deadlock`) |
| `output_case1_3p3c.log` | Execution output for Case 1: 3 producers, 3 consumers, correct semaphore order — mutual exclusion and FIFO integrity verified |
| `output_case2_1p1c.log` | Execution output for Case 2: 1 producer, 1 consumer, correct semaphore order — minimal contention baseline |
| `output_case3_deadlock.log` | Execution output for Case 3: swapped semaphore order demonstrating deadlock (killed by `timeout 5`) |
| `Week11_Report.pdf` | Laboratory report detailing semaphore theory, experimental methodology, and analytical discussion |

---

## Semaphore Architecture & Synchronization Design

### 1. POSIX Semaphore Primitives

POSIX semaphores (`<semaphore.h>`) provide kernel-assisted blocking synchronization:

- **`sem_wait(sem_t *s)`**: Atomically decrements the semaphore value. If the value drops below zero, the calling thread is **blocked** in a kernel wait queue until another thread posts.
- **`sem_post(sem_t *s)`**: Atomically increments the semaphore value. If threads are blocked on this semaphore, one is **woken up** and moved to the ready queue.
- **`sem_init(sem_t *s, int pshared, unsigned int value)`**: Initializes the semaphore with a starting count.

Unlike spinlocks (Week 09), semaphore-blocked threads yield the CPU entirely, eliminating busy-waiting overhead.

### 2. Semaphore Roles in the Bounded Buffer

| Semaphore | Type | Initial Value | Purpose |
|---|---|---|---|
| `empty_slots` | Counting | `BUFFER_SIZE` (5) | Tracks available free slots. Producers `wait()` on this before inserting. |
| `full_slots` | Counting | `0` | Tracks filled slots. Consumers `wait()` on this before removing. |
| `mutex` | Binary | `1` | Ensures mutual exclusion over `buffer[]`, `in`, `out`, and `counter`. |

### 3. Correct Semaphore Ordering (Cases 1 & 2)

The correct acquisition protocol follows the **two-phase locking** principle:

**Producer:**
```c
sem_wait(&empty_slots);    // Phase 1: Wait for a free slot (may block)
sem_wait(&mutex);          // Phase 2: Enter critical section
// ... insert item into buffer ...
sem_post(&mutex);          // Release critical section
sem_post(&full_slots);     // Signal: one more filled slot
```

**Consumer:**
```c
sem_wait(&full_slots);     // Phase 1: Wait for a filled slot (may block)
sem_wait(&mutex);          // Phase 2: Enter critical section
// ... remove item from buffer ...
sem_post(&mutex);          // Release critical section
sem_post(&empty_slots);    // Signal: one more free slot
```

**Key Invariant**: The counting semaphore (`empty_slots` / `full_slots`) is acquired **before** the binary `mutex`. This ensures that a thread holding the mutex is guaranteed to find a slot available, preventing indefinite blocking while holding an exclusive lock.

### 4. Incorrect Ordering & Deadlock (Case 3)

When `SWAP_ORDER=1` is defined, the acquisition order is **reversed**:

```c
sem_wait(&mutex);          // WRONG: Acquire exclusive lock first
sem_wait(&empty_slots);   // Block on buffer capacity while holding mutex
```

**Deadlock Scenario** (with buffer of size $N$):
1. The producer acquires `mutex` and inserts items until the buffer is full ($N$ items produced).
2. The producer calls `sem_wait(&empty_slots)` — but the buffer is full, so `empty_slots = 0`. The producer **blocks**, still holding `mutex`.
3. The consumer attempts to consume, calls `sem_wait(&mutex)` — but `mutex` is held by the blocked producer. The consumer **blocks**.
4. **Circular wait**: Producer waits for `empty_slots` (held implicitly by the consumer who hasn't consumed yet), and the consumer waits for `mutex` (held by the producer). **Deadlock**.

This satisfies all four Coffman conditions:
- **Mutual Exclusion**: `mutex` is exclusive.
- **Hold and Wait**: The producer holds `mutex` while waiting for `empty_slots`.
- **No Preemption**: Semaphores cannot be forcibly released.
- **Circular Wait**: Producer → `empty_slots` → Consumer → `mutex` → Producer.

---

## Experimental Cases & Findings

### Case 1: Correct Order — 3 Producers, 3 Consumers (`pc_sem`)

- **Build**: `gcc -pthread -Wall -Wextra producer_consumer_sem.c -o pc_sem`
- **Configuration**: 3 producers, 3 consumers, 10 items per producer (30 total), buffer size 5.
- **Semaphore Order**: `empty_slots`/`full_slots` first, then `mutex` (correct).
- **Results**:
  - Total produced: `30`
  - Total consumed: `30`
  - Maximum counter observed: `5` (exactly at `BUFFER_SIZE` limit)
  - Every item consumed exactly once, in FIFO order per producer: `yes`
  - Final semaphore values: `empty_slots = 5, full_slots = 0, mutex = 1`
  - **Final counter = 0** (clean termination)
- **Analysis**: The counting semaphores perfectly bounded the buffer occupancy. The `assert(counter >= 0 && counter <= BUFFER_SIZE)` invariant held throughout execution. FIFO ordering per producer was maintained by the circular buffer discipline.

### Case 2: Correct Order — 1 Producer, 1 Consumer (`pc_sem_1p1c`)

- **Build**: `gcc -pthread -Wall -Wextra -DNUM_PRODUCERS=1 -DNUM_CONSUMERS=1 producer_consumer_sem.c -o pc_sem_1p1c`
- **Configuration**: 1 producer, 1 consumer, 10 items, buffer size 5.
- **Semaphore Order**: Correct.
- **Results**:
  - Total produced: `10`
  - Total consumed: `10`
  - Maximum counter observed: `5`
  - Every item consumed exactly once, in FIFO order per producer: `yes`
  - Final semaphore values: `empty_slots = 5, full_slots = 0, mutex = 1`
  - **Final counter = 0**
- **Analysis**: With minimal contention, the producer fills the buffer to capacity before the consumer begins draining it. The interleaving observed in the output confirms that the producer blocked on `empty_slots` after producing 5 items and resumed only after the consumer freed slots.

### Case 3: Swapped Order — Deadlock Demonstration (`pc_sem_deadlock`)

- **Build**: `gcc -pthread -Wall -Wextra -DSWAP_ORDER=1 -DNUM_PRODUCERS=1 -DNUM_CONSUMERS=1 -DITEMS_PER_THREAD=50 producer_consumer_sem.c -o pc_sem_deadlock`
- **Configuration**: 1 producer, 1 consumer, 50 items, buffer size 5, **wrong semaphore order**.
- **Results**:
  - The producer successfully produced 5 items (filling the buffer).
  - At item 6, the producer called `sem_wait(&mutex)` (acquired), then `sem_wait(&empty_slots)` — **blocked** (buffer full, `empty_slots = 0`).
  - The consumer attempted `sem_wait(&mutex)` — **blocked** (mutex held by producer).
  - **Both threads permanently deadlocked.**
  - The process was killed by `timeout 5`, returning exit code `124` (timeout signal).
- **Analysis**: The deadlock manifests precisely at the $(N+1)$-th item, where $N$ = `BUFFER_SIZE`. The first $N$ items succeed because `empty_slots` is not yet exhausted when `mutex` is taken. On the $(N+1)$-th iteration, the circular wait condition is satisfied.

---

## Safety Invariants & Verification

The implementation enforces several runtime invariants via `assert()` and post-condition checks:

| Invariant | Enforcement | Verified In |
|---|---|---|
| $0 \le \text{counter} \le \text{BUFFER\_SIZE}$ | `assert()` inside critical section (both producer and consumer) | Cases 1 & 2 |
| FIFO order per producer | `next_expected[producer_id]` tracking array | Cases 1 & 2 |
| Each item consumed exactly once | `seen[item_id]` frequency array, verified post-join | Cases 1 & 2 |
| $\text{Total Produced} = \text{Total Consumed}$ | Post-join equality check | Cases 1 & 2 |
| Final counter = 0 | Post-join check, exit code 0/1 | Cases 1 & 2 |
| Final semaphore state: `empty_slots = N, full_slots = 0, mutex = 1` | `sem_getvalue()` post-join | Cases 1 & 2 |

---

## Build and Execution Guide

### Using Makefile

```bash
# Build all three binaries
aditya-forge@LAPTOP-184D82EB:~/os_lab$ make all

# Build individual targets
aditya-forge@LAPTOP-184D82EB:~/os_lab$ make pc_sem
aditya-forge@LAPTOP-184D82EB:~/os_lab$ make pc_sem_1p1c
aditya-forge@LAPTOP-184D82EB:~/os_lab$ make pc_sem_deadlock

# Clean built binaries
aditya-forge@LAPTOP-184D82EB:~/os_lab$ make clean
```

### Manual Compilation

```bash
# Case 1: 3 producers, 3 consumers (correct order)
gcc -pthread -Wall -Wextra producer_consumer_sem.c -o pc_sem

# Case 2: 1 producer, 1 consumer (correct order)
gcc -pthread -Wall -Wextra -DNUM_PRODUCERS=1 -DNUM_CONSUMERS=1 producer_consumer_sem.c -o pc_sem_1p1c

# Case 3: Wrong semaphore order (deadlock demo)
gcc -pthread -Wall -Wextra -DSWAP_ORDER=1 -DNUM_PRODUCERS=1 -DNUM_CONSUMERS=1 -DITEMS_PER_THREAD=50 producer_consumer_sem.c -o pc_sem_deadlock
```

### Running Test Cases

```bash
# Case 1: Correct order, multi-threaded
./pc_sem

# Case 2: Correct order, single pair
./pc_sem_1p1c

# Case 3: Deadlock demo (use timeout to avoid infinite hang)
timeout 5 ./pc_sem_deadlock
echo $?   # Expected: 124 (killed by timeout)
```

---

## Sample Execution Outputs

### Case 1: 3 Producers, 3 Consumers — Correct Order (`./pc_sem`)
```text
aditya-forge@LAPTOP-184D82EB:~/os_lab$ ./pc_sem
Semaphores : empty_slots = 5, full_slots = 0, mutex = 1
Wait order : empty/full first, then mutex (correct)
Config     : 3 producer(s), 3 consumer(s), 10 items per producer, buffer size 5

Producer 1 produced item 100 | counter = 1
Producer 1 produced item 101 | counter = 2
Producer 1 produced item 102 | counter = 3
Producer 1 produced item 103 | counter = 4
Producer 1 produced item 104 | counter = 5
Consumer 1 consumed item 100 | counter = 4
Consumer 1 consumed item 101 | counter = 3
...
Consumer 3 consumed item 208 | counter = 1
Consumer 3 consumed item 209 | counter = 0

Total produced = 30
Total consumed = 30
Max counter observed = 5 (limit 5)
Every item consumed exactly once, in FIFO order per producer : yes
Final semaphore values : empty_slots = 5, full_slots = 0, mutex = 1

Final counter = 0 (expected 0)
```

### Case 2: 1 Producer, 1 Consumer — Correct Order (`./pc_sem_1p1c`)
```text
aditya-forge@LAPTOP-184D82EB:~/os_lab$ ./pc_sem_1p1c
Semaphores : empty_slots = 5, full_slots = 0, mutex = 1
Wait order : empty/full first, then mutex (correct)
Config     : 1 producer(s), 1 consumer(s), 10 items per producer, buffer size 5

Producer 1 produced item 100 | counter = 1
Producer 1 produced item 101 | counter = 2
Producer 1 produced item 102 | counter = 3
Producer 1 produced item 103 | counter = 4
Producer 1 produced item 104 | counter = 5
Consumer 1 consumed item 100 | counter = 4
Consumer 1 consumed item 101 | counter = 3
Consumer 1 consumed item 102 | counter = 2
Consumer 1 consumed item 103 | counter = 1
Producer 1 produced item 105 | counter = 2
...
Consumer 1 consumed item 109 | counter = 0

Total produced = 10
Total consumed = 10
Max counter observed = 5 (limit 5)
Every item consumed exactly once, in FIFO order per producer : yes
Final semaphore values : empty_slots = 5, full_slots = 0, mutex = 1

Final counter = 0 (expected 0)
```

### Case 3: Swapped Order — Deadlock (`timeout 5 ./pc_sem_deadlock`)
```text
aditya-forge@LAPTOP-184D82EB:~/os_lab$ timeout 5 ./pc_sem_deadlock
Semaphores : empty_slots = 5, full_slots = 0, mutex = 1
Wait order : mutex first (WRONG, deadlock demo)
Config     : 1 producer(s), 1 consumer(s), 50 items per producer, buffer size 5

Producer 1 produced item 100 | counter = 1
Producer 1 produced item 101 | counter = 2
Producer 1 produced item 102 | counter = 3
Producer 1 produced item 103 | counter = 4
Producer 1 produced item 104 | counter = 5
aditya-forge@LAPTOP-184D82EB:~/os_lab$ echo $?
124
```
*(The process hung after producing exactly 5 items (BUFFER_SIZE). Exit code 124 confirms termination by `timeout`, verifying deadlock.)*

---

*Maintained by aditya-forge | SRM University AP — Operating Systems*
