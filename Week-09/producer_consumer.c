#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <unistd.h>
#include <assert.h>
#include <sched.h>

#ifndef USE_CAS
#define USE_CAS 0   /* 0 = test_and_set, 1 = compare_and_swap */
#endif

#ifndef USE_LOCK
#define USE_LOCK 1  /* 0 = remove acquire/release (race experiment) */
#endif

#define BUFFER_SIZE 5

#ifndef NUM_PRODUCERS
#define NUM_PRODUCERS 3
#endif

#ifndef NUM_CONSUMERS
#define NUM_CONSUMERS 3
#endif

#define ITEMS_PER_THREAD 10

int buffer[BUFFER_SIZE];
int in = 0, out = 0;
int counter = 0;              // shared counter to verify full / empty
atomic_bool lock = false;     // hardware lock variable

// Statistics (protected by the lock, except where noted)
int total_produced = 0, total_consumed = 0;
int max_counter_seen = 0;
atomic_int assertion_failures = 0;

// ---------- Atomic primitives ----------
bool test_and_set(atomic_bool *target) {
    return atomic_exchange(target, true);   // returns OLD value
}

bool compare_and_swap(atomic_bool *target, bool expected, bool new_val) {
    return atomic_compare_exchange_strong(target, &expected, new_val);
}

// ---------- Lock / Unlock ----------
void acquire(atomic_bool *l) {
#if USE_CAS
    while (!compare_and_swap(l, false, true)) {
        // CAS failed -> someone else holds the lock -> retry
    }
#else
    while (test_and_set(l)) {
        // busy-wait: spin until lock is free
    }
#endif
}

void release(atomic_bool *l) {
    atomic_store(l, false);
}

#if USE_LOCK
#define LOCK()   acquire(&lock)
#define UNLOCK() release(&lock)
#else
#define LOCK()
#define UNLOCK()
#endif

// ---------- Invariant check ----------
// With the lock enabled a failed check aborts via assert().
// In the no-lock experiment we count failures instead so the run can finish.
static void check_invariant(const char *who, int id) {
#if USE_LOCK
    assert(counter >= 0 && counter <= BUFFER_SIZE);
    (void)who; (void)id;
#else
    if (!(counter >= 0 && counter <= BUFFER_SIZE)) {
        atomic_fetch_add(&assertion_failures, 1);
    }
    (void)who; (void)id;
#endif
}

// ---------- Producer / Consumer ----------
void *producer(void *arg) {
    int id = *(int *)arg;
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        int item = id * 100 + i;
        bool done = false;
        while (!done) {
            LOCK();
            if (counter < BUFFER_SIZE) {          // buffer not full
                buffer[in] = item;
                in = (in + 1) % BUFFER_SIZE;
                counter++;
                total_produced++;
                if (counter > max_counter_seen) max_counter_seen = counter;
                check_invariant("Producer", id);
                printf("Producer %d produced item %3d | counter = %d\n", id, item, counter);
                done = true;
            }
            UNLOCK();
            if (!done) sched_yield();             // buffer full: retry
        }
    }
    return NULL;
}

void *consumer(void *arg) {
    int id = *(int *)arg;
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        bool done = false;
        while (!done) {
            LOCK();
            if (counter > 0) {                    // buffer not empty
                int item = buffer[out];
                out = (out + 1) % BUFFER_SIZE;
                counter--;
                total_consumed++;
                check_invariant("Consumer", id);
                printf("Consumer %d consumed item %3d | counter = %d\n", id, item, counter);
                done = true;
            }
            UNLOCK();
            if (!done) sched_yield();             // buffer empty: retry
        }
    }
    return NULL;
}

int main(void) {
    pthread_t prod[NUM_PRODUCERS], cons[NUM_CONSUMERS];
    int pid[NUM_PRODUCERS], cid[NUM_CONSUMERS];

    printf("Lock type : %s\n", USE_CAS ? "compare_and_swap (USE_CAS 1)" : "test_and_set (USE_CAS 0)");
    printf("Lock used : %s\n", USE_LOCK ? "yes" : "NO (race experiment)");
    printf("Config    : %d producer(s), %d consumer(s), %d items each, buffer size %d\n\n",
           NUM_PRODUCERS, NUM_CONSUMERS, ITEMS_PER_THREAD, BUFFER_SIZE);

    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pid[i] = i + 1;
        pthread_create(&prod[i], NULL, producer, &pid[i]);
    }
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        cid[i] = i + 1;
        pthread_create(&cons[i], NULL, consumer, &cid[i]);
    }

    for (int i = 0; i < NUM_PRODUCERS; i++) pthread_join(prod[i], NULL);
    for (int i = 0; i < NUM_CONSUMERS; i++) pthread_join(cons[i], NULL);

    printf("\nTotal produced = %d\n", total_produced);
    printf("Total consumed = %d\n", total_consumed);
    printf("Max counter observed = %d (limit %d)\n", max_counter_seen, BUFFER_SIZE);
#if !USE_LOCK
    printf("Invariant violations detected = %d\n", atomic_load(&assertion_failures));
#endif
    printf("\nFinal counter = %d (expected 0)\n", counter);
    return 0;
}
