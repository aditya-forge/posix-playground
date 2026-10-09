# Week 08: Inter-Process Communication — Producer-Consumer Race Conditions

This directory documents **Week 08** of Operating Systems coursework, focusing on **Inter-Process Communication (IPC)** using POSIX shared memory (`/dev/shm`) and observing race conditions in a classic Producer-Consumer problem without synchronization.

---

## Experiment Overview

In this lab session, we explored the effects of concurrent execution on shared resources. A **producer** process generates a sequence of items and places them into a shared ring buffer, while a **consumer** process reads them out and verifies their sequence. 

Because the `count` variable is modified by both processes concurrently without atomic operations or locks (like Mutexes or Semaphores), race conditions occur, leading to data corruption (lost updates).

### Included Files
- `buffer.h`: Defines the shared memory layout, buffer size, and data structures.
- `producer.c`: Maps shared memory, generates items sequentially, and writes to the buffer.
- `consumer.c`: Maps shared memory, reads items, and checks for sequence integrity (gaps or duplicates).
- `Makefile`: Automates the GCC compilation pipeline with POSIX realtime extensions (`-lrt`).
- `stage1_observations.md`: Detailed analysis of the race condition failures across 60 execution runs with varying buffer sizes and startup orders.

---

## Key Observations (from `stage1_observations.md`)

1. **Deterministic Failure**: 0 out of 60 runs succeeded. A race condition is hit almost immediately because both processes frequently update the `count` variable.
2. **Failure Types**: 
   - **Gaps**: Lost producer increment (count becomes smaller than actual). Producer thinks buffer has free space and overwrites oldest unread slot.
   - **Duplicates**: Lost consumer decrement (count becomes larger than actual). Consumer thinks buffer has items when empty and reads a stale slot.
3. **Buffer Size Impact**: Modifying `BUFFER_SIZE` (from 2 to 1024) does not prevent the race condition, it merely delays how quickly the data corruption becomes visible to the consumer (since a lost update is only noticed after a full wrap-around lap of the ring buffer).
4. **Assembly-Level Root Cause**: The statement `count = count + 1` requires a load into a CPU register, an increment, and a store back to memory. Without synchronization, context switches during this instruction sequence lead to one process overwriting the other's update.

---

## Execution Workflow

```bash
# Compile the producer and consumer binaries using the Makefile
aditya-forge@LAPTOP-184D82EB:~/OS/Week-08$ make

# Start the consumer in one terminal (Terminal B)
aditya-forge@LAPTOP-184D82EB:~/OS/Week-08$ ./consumer 

# Start the producer in another terminal (Terminal A)
aditya-forge@LAPTOP-184D82EB:~/OS/Week-08$ ./producer 1000000

# Observe the failure logs
# [consumer] FAIL: expected 2827, got 1803 (duplicate/reorder) at item index 2827
```

*Conclusion: Safe concurrent modifications to shared state require synchronization primitives (Semaphores/Locks).*