/*
 * producer_consumer_sem.c
 *
 * Bounded-buffer Producer-Consumer problem solved with POSIX semaphores.
 *
 *   empty_slots : counts free slots in the buffer   (initial value = BUFFER_SIZE)
 *   full_slots  : counts filled slots in the buffer (initial value = 0)
 *   mutex       : binary semaphore protecting in, out and counter (initial value = 1)
 *
 * Producer : wait(empty_slots) -> wait(mutex) -> insert -> post(mutex) -> post(full_slots)
 * Consumer : wait(full_slots)  -> wait(mutex) -> remove -> post(mutex) -> post(empty_slots)
 *
 * Build options (all optional):
 *   -DNUM_PRODUCERS=n  -DNUM_CONSUMERS=n  -DITEMS_PER_THREAD=n
 *   -DSWAP_ORDER=1     take mutex BEFORE empty_slots/full_slots (wrong order, deadlock demo)
 *
 * Compile : gcc -pthread producer_consumer_sem.c -o pc_sem
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <assert.h>

#define BUFFER_SIZE 5

#ifndef NUM_PRODUCERS
#define NUM_PRODUCERS 3
#endif
#ifndef NUM_CONSUMERS
#define NUM_CONSUMERS 3
#endif
#ifndef ITEMS_PER_THREAD
#define ITEMS_PER_THREAD 10          /* items made by each producer (must be < 100) */
#endif
#ifndef SWAP_ORDER
#define SWAP_ORDER 0                 /* 1 = wrong semaphore order (deadlock demo) */
#endif

#define TOTAL_ITEMS (NUM_PRODUCERS * ITEMS_PER_THREAD)
_Static_assert(ITEMS_PER_THREAD < 100, "item numbering needs ITEMS_PER_THREAD < 100");
_Static_assert(TOTAL_ITEMS % NUM_CONSUMERS == 0, "items must divide evenly among consumers");
#define ITEMS_PER_CONSUMER (TOTAL_ITEMS / NUM_CONSUMERS)

int buffer[BUFFER_SIZE];
int in = 0, out = 0;
int counter = 0;                     // number of items currently in the buffer

sem_t empty_slots, full_slots, mutex;

// Statistics and checks (only touched while holding mutex)
int total_produced = 0, total_consumed = 0;
int max_counter_seen = 0;
int next_expected[NUM_PRODUCERS + 1];        // per-producer FIFO check
int seen[(NUM_PRODUCERS + 1) * 100];         // how many times each item was consumed

void *producer(void *arg) {
    int id = *(int *)arg;
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        int item = id * 100 + i;

#if SWAP_ORDER
        sem_wait(&mutex);                    // WRONG: holds the mutex while waiting for a slot
        sem_wait(&empty_slots);
#else
        sem_wait(&empty_slots);              // wait for a free slot
        sem_wait(&mutex);                    // enter critical section
#endif
        buffer[in] = item;
        in = (in + 1) % BUFFER_SIZE;
        counter++;
        total_produced++;
        if (counter > max_counter_seen) max_counter_seen = counter;
        assert(counter >= 0 && counter <= BUFFER_SIZE);
        printf("Producer %d produced item %3d | counter = %d\n", id, item, counter);
        sem_post(&mutex);                    // leave critical section
        sem_post(&full_slots);               // one more filled slot
    }
    return NULL;
}

void *consumer(void *arg) {
    int id = *(int *)arg;
    for (int i = 0; i < ITEMS_PER_CONSUMER; i++) {
#if SWAP_ORDER
        sem_wait(&mutex);                    // WRONG: holds the mutex while waiting for an item
        sem_wait(&full_slots);
#else
        sem_wait(&full_slots);               // wait for a filled slot
        sem_wait(&mutex);                    // enter critical section
#endif
        int item = buffer[out];
        out = (out + 1) % BUFFER_SIZE;
        counter--;
        total_consumed++;
        assert(counter >= 0 && counter <= BUFFER_SIZE);

        int p = item / 100, idx = item % 100;
        assert(idx == next_expected[p]);     // each producer's items leave in FIFO order
        next_expected[p]++;
        seen[item]++;

        printf("Consumer %d consumed item %3d | counter = %d\n", id, item, counter);
        sem_post(&mutex);                    // leave critical section
        sem_post(&empty_slots);              // one more free slot
    }
    return NULL;
}

int main(void) {
    pthread_t prod[NUM_PRODUCERS], cons[NUM_CONSUMERS];
    int pid[NUM_PRODUCERS], cid[NUM_CONSUMERS];

    setvbuf(stdout, NULL, _IOLBF, 0);        // line-buffered, so output survives a hang
    sem_init(&empty_slots, 0, BUFFER_SIZE);
    sem_init(&full_slots, 0, 0);
    sem_init(&mutex, 0, 1);

    printf("Semaphores : empty_slots = %d, full_slots = 0, mutex = 1\n", BUFFER_SIZE);
    printf("Wait order : %s\n", SWAP_ORDER ? "mutex first (WRONG, deadlock demo)"
                                           : "empty/full first, then mutex (correct)");
    printf("Config     : %d producer(s), %d consumer(s), %d items per producer, buffer size %d\n\n",
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

    int once = 1;                            // every item consumed exactly once?
    for (int p = 1; p <= NUM_PRODUCERS; p++)
        for (int i = 0; i < ITEMS_PER_THREAD; i++)
            if (seen[p * 100 + i] != 1) once = 0;

    int ev, fv, mv;
    sem_getvalue(&empty_slots, &ev);
    sem_getvalue(&full_slots, &fv);
    sem_getvalue(&mutex, &mv);

    printf("\nTotal produced = %d\n", total_produced);
    printf("Total consumed = %d\n", total_consumed);
    printf("Max counter observed = %d (limit %d)\n", max_counter_seen, BUFFER_SIZE);
    printf("Every item consumed exactly once, in FIFO order per producer : %s\n", once ? "yes" : "NO");
    printf("Final semaphore values : empty_slots = %d, full_slots = %d, mutex = %d\n", ev, fv, mv);
    printf("\nFinal counter = %d (expected 0)\n", counter);

    sem_destroy(&empty_slots);
    sem_destroy(&full_slots);
    sem_destroy(&mutex);
    return (counter == 0 && once && total_produced == total_consumed) ? 0 : 1;
}
