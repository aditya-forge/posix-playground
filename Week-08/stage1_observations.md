# OS ASSIGNMENT: IPC SHARED MEMORY RACE CONDITION

**Name:** Aditya Kumar  |  **Registration no:** AP24110010333  |  **Section:** BTech CSE - C

Implement a bounded buffer Producer and Consumer system using POSIX shared memory, where two processes share a circular buffer of size N and a shared counter count. The producer writes the sequence 0, 1, 2, ... and increments count after each write. The consumer reads the items in FIFO order, decrements count after each read and verifies that every item is exactly the previous item plus one. The buffer is empty when count == 0 and full when count == BUFFER_SIZE. No synchronization primitives are allowed, so the lost update race on count can be observed and explained.

## Stage 1: Naive Implementation (No Synchronization)

Forbidden in this stage and not used anywhere in the code: semaphores, mutexes, spinlocks, condition variables, C11 atomics, memory fences and volatile. count, head and tail are plain int fields. Only sched_yield and nanosleep are used inside the wait loops.

**buffer.h:**

```
#ifndef BUFFER_H
#define BUFFER_H

#define SHM_NAME "/pc_counter"

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 8
#endif

#define BUFFER_MASK (BUFFER_SIZE - 1)

typedef struct {
    int head;
    int tail;
    int count;
    int data[BUFFER_SIZE];
    int shutdown;
} shm_buffer_t;

static inline int next_idx(int i) { return (i + 1) & BUFFER_MASK; }

#endif
```

**producer.c:**

```
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sched.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "buffer.h"

static shm_buffer_t *g_buf;

static double now_sec(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "usage: %s <items>\n", argv[0]);
        return 1;
    }
    long N = strtol(argv[1], NULL, 10);

    shm_unlink(SHM_NAME);
    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (fd < 0) { perror("shm_open"); return 1; }
    if (ftruncate(fd, sizeof(shm_buffer_t)) < 0) { perror("ftruncate"); return 1; }
    g_buf = mmap(NULL, sizeof(shm_buffer_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (g_buf == MAP_FAILED) { perror("mmap"); return 1; }
    close(fd);
    memset(g_buf, 0, sizeof(shm_buffer_t));

    fprintf(stderr, "[producer] started; will produce %ld items\n", N);

    for (long i = 0; i < N; ++i) {
        long spins = 0;
        double t0 = 0;
        while (g_buf->count == BUFFER_SIZE) {
            sched_yield();
            if (++spins == 1) t0 = now_sec();
            if ((spins & 0xFFFF) == 0 && now_sec() - t0 > 2.0) {
                fprintf(stderr, "[producer] buffer stuck full at item %ld; consumer gone, giving up\n", i);
                g_buf->shutdown = 1;
                munmap(g_buf, sizeof(shm_buffer_t));
                return 2;
            }
        }
        g_buf->data[g_buf->tail] = (int)i;
        g_buf->tail = next_idx(g_buf->tail);
        g_buf->count = g_buf->count + 1;
    }

    g_buf->shutdown = 1;
    fprintf(stderr, "[producer] done; signaled shutdown\n");
    munmap(g_buf, sizeof(shm_buffer_t));
    return 0;
}
```

**consumer.c:**

```
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sched.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "buffer.h"

static shm_buffer_t *g_buf;

int main(void)
{
    fprintf(stderr, "[consumer] waiting for producer ...\n");

    int fd;
    struct timespec ts = {0, 1000000};
    while ((fd = shm_open(SHM_NAME, O_RDWR, 0666)) < 0)
        nanosleep(&ts, NULL);

    struct stat st;
    do {
        fstat(fd, &st);
        if ((size_t)st.st_size < sizeof(shm_buffer_t)) nanosleep(&ts, NULL);
    } while ((size_t)st.st_size < sizeof(shm_buffer_t));

    g_buf = mmap(NULL, sizeof(shm_buffer_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (g_buf == MAP_FAILED) { perror("mmap"); return 1; }
    close(fd);

    fprintf(stderr, "[consumer] connected\n");

    long expected = 0;

    for (;;) {
        while (g_buf->count == 0) {
            if (g_buf->shutdown) goto done;
            sched_yield();
        }

        int item = g_buf->data[g_buf->head];
        g_buf->head = next_idx(g_buf->head);
        g_buf->count = g_buf->count - 1;

        if ((long)item != expected) {
            fprintf(stderr,
                    "FAIL: expected %ld, got %d (%s) at item index %ld\n",
                    expected, item,
                    ((long)item > expected) ? "gap" :
                    ((long)item < expected) ? "duplicate/reorder" : "unknown",
                    expected);
            munmap(g_buf, sizeof(shm_buffer_t));
            return 1;
        }
        ++expected;
    }

done:
    printf("OK: %ld items transferred\n", expected);
    munmap(g_buf, sizeof(shm_buffer_t));
    shm_unlink(SHM_NAME);
    return 0;
}
```

**Makefile:**

```
CC = gcc
CFLAGS = -std=c11 -O2 -Wall -Wextra
LDFLAGS = -lrt

all: producer consumer

producer: producer.c buffer.h
	$(CC) $(CFLAGS) -o $@ producer.c $(LDFLAGS)

consumer: consumer.c buffer.h
	$(CC) $(CFLAGS) -o $@ consumer.c $(LDFLAGS)

clean:
	rm -f producer consumer
	-rm -f /dev/shm/pc_counter

.PHONY: all clean
```

## Commands Used (Two Terminal Workflow):

**Terminal A (build once):**

```
cd ~/ipc_assignment/stage1
make clean
make
```

**Terminal B (start first, every run):**

```
cd ~/ipc_assignment/stage1
./consumer 2>&1 | tee ../consumer_runN.log
```

**Terminal A (start second, every run):**

```
rm -f /dev/shm/pc_counter
./producer 1000000 2>&1 | tee ../producer_runN.log
```

The consumer was always started first and waited until the producer created the shared memory object. In every run the consumer detected the error first and printed FAIL. About 2 seconds later the producer reported that the buffer was stuck full and exited, because after the consumer exits nobody empties the buffer.

## Output:

**Build:**

```
aditya-forge@LAPTOP-184D82EB:~$ cd ~/ipc_assignment/stage1
aditya-forge@LAPTOP-184D82EB:~/ipc_assignment/stage1$ make clean
rm -f producer consumer
rm -f /dev/shm/pc_counter
aditya-forge@LAPTOP-184D82EB:~/ipc_assignment/stage1$ make
gcc -std=c11 -O2 -Wall -Wextra -o producer producer.c -lrt
gcc -std=c11 -O2 -Wall -Wextra -o consumer consumer.c -lrt
aditya-forge@LAPTOP-184D82EB:~/ipc_assignment/stage1$ ls
Makefile  buffer.h  consumer  consumer.c  producer  producer.c
```

**Run 1 (gap at item index 6035):**

**Terminal A (Producer):**

```
aditya-forge@LAPTOP-184D82EB:~/ipc_assignment/stage1$ ./producer 1000000 2>&1 | tee ../producer_run1.log
[producer] started; will produce 1000000 items
[producer] buffer stuck full at item 6049; consumer gone, giving up
```

**Terminal B (Consumer):**

```
aditya-forge@LAPTOP-184D82EB:~/ipc_assignment/stage1$ ./consumer 2>&1 | tee ../consumer_run1.log
[consumer] waiting for producer ...
[consumer] connected
FAIL: expected 6035, got 6043 (gap) at item index 6035
```

**Run 9 (fastest failure, gap at item index 18):**

**Terminal A (Producer):**

```
aditya-forge@LAPTOP-184D82EB:~/ipc_assignment/stage1$ ./producer 1000000 2>&1 | tee ../producer_run9.log
[producer] started; will produce 1000000 items
[producer] buffer stuck full at item 33; consumer gone, giving up
```

**Terminal B (Consumer):**

```
aditya-forge@LAPTOP-184D82EB:~/ipc_assignment/stage1$ ./consumer 2>&1 | tee ../consumer_run9.log
[consumer] waiting for producer ...
[consumer] connected
FAIL: expected 18, got 26 (gap) at item index 18
```

**Run 10 (duplicate at item index 248):**

**Terminal A (Producer):**

```
aditya-forge@LAPTOP-184D82EB:~/ipc_assignment/stage1$ ./producer 1000000 2>&1 | tee ../producer_run10.log
[producer] started; will produce 1000000 items
[producer] buffer stuck full at item 250; consumer gone, giving up
```

**Terminal B (Consumer):**

```
aditya-forge@LAPTOP-184D82EB:~/ipc_assignment/stage1$ ./consumer 2>&1 | tee ../consumer_run10.log
[consumer] waiting for producer ...
[consumer] connected
FAIL: expected 248, got 240 (duplicate/reorder) at item index 248
```

**Run 15 (longest run, duplicate at item index 23000):**

**Terminal A (Producer):**

```
aditya-forge@LAPTOP-184D82EB:~/ipc_assignment/stage1$ ./producer 1000000 2>&1 | tee ../producer_run15.log
[producer] started; will produce 1000000 items
[producer] buffer stuck full at item 23002; consumer gone, giving up
```

**Terminal B (Consumer):**

```
aditya-forge@LAPTOP-184D82EB:~/ipc_assignment/stage1$ ./consumer 2>&1 | tee ../consumer_run15.log
[consumer] waiting for producer ...
[consumer] connected
FAIL: expected 23000, got 22992 (duplicate/reorder) at item index 23000
```

**Observation Table (20 runs, BUFFER_SIZE = 8):**

| Run | Terminal A output | Terminal B output | Items before failure | Failure type |
|---|---|---|---|---|
| 1 | buffer stuck full at item 6049 | FAIL: expected 6035, got 6043 | 6035 | Gap (+8) |
| 2 | buffer stuck full at item 5447 | FAIL: expected 5432, got 5440 | 5432 | Gap (+8) |
| 3 | buffer stuck full at item 3705 | FAIL: expected 3690, got 3698 | 3690 | Gap (+8) |
| 4 | buffer stuck full at item 8446 | FAIL: expected 8432, got 8440 | 8432 | Gap (+8) |
| 5 | buffer stuck full at item 595 | FAIL: expected 584, got 592 | 584 | Gap (+8) |
| 6 | buffer stuck full at item 4982 | FAIL: expected 4968, got 4976 | 4968 | Gap (+8) |
| 7 | buffer stuck full at item 1386 | FAIL: expected 1376, got 1384 | 1376 | Gap (+8) |
| 8 | buffer stuck full at item 4097 | FAIL: expected 4082, got 4090 | 4082 | Gap (+8) |
| 9 | buffer stuck full at item 33 | FAIL: expected 18, got 26 | 18 | Gap (+8) |
| 10 | buffer stuck full at item 250 | FAIL: expected 248, got 240 | 248 | Duplicate (-8) |
| 11 | buffer stuck full at item 3491 | FAIL: expected 3488, got 3480 | 3488 | Duplicate (-8) |
| 12 | buffer stuck full at item 2257 | FAIL: expected 2244, got 2252 | 2244 | Gap (+8) |
| 13 | buffer stuck full at item 16093 | FAIL: expected 16088, got 16080 | 16088 | Duplicate (-8) |
| 14 | buffer stuck full at item 302 | FAIL: expected 288, got 296 | 288 | Gap (+8) |
| 15 | buffer stuck full at item 23002 | FAIL: expected 23000, got 22992 | 23000 | Duplicate (-8) |
| 16 | buffer stuck full at item 12737 | FAIL: expected 12723, got 12731 | 12723 | Gap (+8) |
| 17 | buffer stuck full at item 158 | FAIL: expected 144, got 152 | 144 | Gap (+8) |
| 18 | buffer stuck full at item 655 | FAIL: expected 647, got 639 | 647 | Duplicate (-8) |
| 19 | buffer stuck full at item 4018 | FAIL: expected 4016, got 4008 | 4016 | Duplicate (-8) |
| 20 | buffer stuck full at item 7912 | FAIL: expected 7896, got 7904 | 7896 | Gap (+8) |

Summary: 0 out of 20 runs succeeded. Items transferred before failure ranged from 18 to 23000, with a median of about 3853. Gaps: 14 runs. Duplicates: 6 runs. In every failure the wrong value differed from the expected value by exactly 8, which is BUFFER_SIZE.

## Observations:

**1. Did the program ever complete successfully?**

No. 0 out of 20 runs completed. Every run ended with a FAIL from the consumer. The 40 extra runs done for the buffer size and startup order experiments also failed, so the program failed 60 out of 60 times.

**2. How quickly did it fail?**

The failures happened in the tens to tens of thousands of items: fastest at item 18, slowest at 23000, median about 3853. Even the slowest run moved only about 2.3 percent of the 1,000,000 items requested. If the race were only a narrow timing window, most runs would finish and only a few would fail somewhere in the millions. Here both processes write count on every single item, so the race is hit almost all the time and a failure appears very early.

**3. Which failure type dominated?**

Gaps were more common (14 of 20) than duplicates (6 of 20). Every error was off by exactly BUFFER_SIZE (8), which shows the cause directly:

- Lost producer increment, giving a gap: count becomes one less than the real number of items. The producer thinks there is a free slot when the buffer is actually full, so it overwrites the oldest unread slot with a value 8 ahead. The consumer then reads expected + 8.
- Lost consumer decrement, giving a duplicate: count becomes one more than the real number of items. The consumer thinks an item is present when the buffer is actually empty, so it reads a slot that still holds the value from the previous lap, which is expected - 8.

Since a lost update can move count wrong in either direction, both types appear. Gaps were slightly more frequent in these runs, most likely because the producer loop is shorter and it spends more time waiting on a full buffer.

**4. Two terminal evidence**

The screenshots in the Output section show Terminal A (producer) and Terminal B (consumer) for the same run at the moment of failure, for four different runs. The consumer prints FAIL first, then the producer gives up because the buffer stays full.

**5. Was the failure deterministic or nondeterministic?**

Nondeterministic. The same binary with the same argument (./producer 1000000) failed at a different item index in every run, for example 6035, 5432, 3690 and 8432 in runs 1 to 4, and the failure type also changed between gap and duplicate. The result depends on how the scheduler interleaves the two processes on the CPU cores, which is different each time.

**6. Does the buffer size matter?**

The programs were rebuilt with BUFFER_SIZE set to 2, 8, 64 and 1024, and each size was run 10 times with the consumer started first.

| BUFFER_SIZE | Successes | Fastest failure | Median | Slowest failure |
|---|---|---|---|---|
| 2 | 0 / 10 | 990 | about 5116 | 51697 |
| 8 | 0 / 10 | 32 | about 856 | 4560 |
| 64 | 0 / 10 | 114 | about 178 | 445 |
| 1024 | 0 / 10 | 1024 | about 1782 | 4654 |

A bigger buffer does not remove or really delay the failure. Every size failed in all 10 runs.

- Every error was off by exactly the buffer size (2, 8, 64 or 1024). A lost update only becomes visible when a slot is reused, so the consumer notices it about one lap later. That is why size 1024 never failed before item 1024: the race happened earlier, but its effect could only be seen after a full lap.
- Size 2 survived the longest. With such a small buffer it is almost always full or empty, so one process is usually spinning on sched_yield while the other works, and they collide on count less often. With larger buffers both processes run freely at the same time and update count back to back, so collisions are more frequent.

So buffer size only changes when and how the damage becomes visible. The race on count exists at every size.

**BUFFER_SIZE = 1024 build:**

```
aditya-forge@LAPTOP-184D82EB:~/ipc_assignment/stage1$ make clean
rm -f producer consumer
rm -f /dev/shm/pc_counter
aditya-forge@LAPTOP-184D82EB:~/ipc_assignment/stage1$ make CFLAGS='-std=c11 -O2 -Wall -Wextra -DBUFFER_SIZE=1024'
gcc -std=c11 -O2 -Wall -Wextra -DBUFFER_SIZE=1024 -o producer producer.c -lrt
gcc -std=c11 -O2 -Wall -Wextra -DBUFFER_SIZE=1024 -o consumer consumer.c -lrt
```

**Terminal A (Producer):**

```
aditya-forge@LAPTOP-184D82EB:~/ipc_assignment/stage1$ ./producer 1000000 2>&1 | tee ../producer_b1024.log
[producer] started; will produce 1000000 items
[producer] buffer stuck full at item 3851; consumer gone, giving up
```

**Terminal B (Consumer):**

```
aditya-forge@LAPTOP-184D82EB:~/ipc_assignment/stage1$ ./consumer 2>&1 | tee ../consumer_b1024.log
[consumer] waiting for producer ...
[consumer] connected
FAIL: expected 2827, got 1803 (duplicate/reorder) at item index 2827
```

**7. Trace of count on paper**

The compiled -O2 code (checked with objdump) keeps count in a register. The producer loads count during its full check and stores count + 1 only after writing the item. The consumer does the same with count - 1. Starting with count = 3:

| Step | Process | Instruction | Register | count in memory |
|---|---|---|---|---|
| 1 | Producer | mov eax, [count] (full check) | eax = 3 | 3 |
| 2 | Consumer | mov ebx, [count] (empty check) | ebx = 3 | 3 |
| 3 | Consumer | sub ebx, 1 | ebx = 2 | 3 |
| 4 | Consumer | mov [count], ebx |  | 2 |
| 5 | Producer | add eax, 1 | eax = 4 | 2 |
| 6 | Producer | mov [count], eax |  | 4 (should be 3) |

The update is lost at step 6. The producer stores 4, which it calculated from the old value 3 loaded in step 1, so it overwrites the consumer's decrement from step 4. count now says 4 items while only 3 are really in the buffer, and the consumer will later read a stale slot and report a duplicate. If the roles are swapped and the consumer stores last, the producer's increment is lost instead, which leads to a gap.

**8. Does the order of startup matter?**

The producer was started first and the consumer second for 10 runs. All 10 failed (8 gaps, 2 duplicates), between item 10 and item 7552, with a median of about 1884. This is in the same range as the runs where the consumer was started first.

Startup order only affects the first moments. When the producer starts first, it fills the 8 slots and spins on a full buffer until the consumer attaches, so the first few items are transferred without collisions. Once both processes are running, the same race on count appears, so the failure rate stays at 100 percent.

**Terminal A (Producer, started first):**

```
aditya-forge@LAPTOP-184D82EB:~/ipc_assignment/stage1$ ./producer 1000000 2>&1 | tee ../producer_pfirst1.log
[producer] started; will produce 1000000 items
[producer] buffer stuck full at item 1846; consumer gone, giving up
```

**Terminal B (Consumer, started second):**

```
aditya-forge@LAPTOP-184D82EB:~/ipc_assignment/stage1$ ./consumer 2>&1 | tee ../consumer_pfirst1.log
[consumer] waiting for producer ...
[consumer] connected
FAIL: expected 1832, got 1840 (gap) at item index 1832
```

## Conclusion:

Without synchronization the shared counter loses updates in every run, and it does so quickly. The statement count = count + 1 is a separate load, add and store, so when the two processes run it at the same time, one change can overwrite the other. Because count alone decides whether the buffer is empty or full, a single lost update makes the producer overwrite unread data (a gap) or makes the consumer read old data (a duplicate). A larger buffer or a different startup order only changes when the damage shows up. Making these updates safe needs atomic operations, memory ordering or locks.
