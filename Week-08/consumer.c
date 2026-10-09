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
