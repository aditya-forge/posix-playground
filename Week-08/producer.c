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
