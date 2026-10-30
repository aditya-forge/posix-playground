CC = gcc
CFLAGS = -pthread -Wall -Wextra
SRC = producer_consumer_sem.c

all: pc_sem pc_sem_1p1c pc_sem_deadlock

pc_sem: $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o pc_sem

pc_sem_1p1c: $(SRC)
	$(CC) $(CFLAGS) -DNUM_PRODUCERS=1 -DNUM_CONSUMERS=1 $(SRC) -o pc_sem_1p1c

pc_sem_deadlock: $(SRC)
	$(CC) $(CFLAGS) -DSWAP_ORDER=1 -DNUM_PRODUCERS=1 -DNUM_CONSUMERS=1 -DITEMS_PER_THREAD=50 $(SRC) -o pc_sem_deadlock

clean:
	rm -f pc_sem pc_sem_1p1c pc_sem_deadlock

.PHONY: all clean
