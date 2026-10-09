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
