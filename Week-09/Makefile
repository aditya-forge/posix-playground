CC = gcc
CFLAGS = -pthread -Wall -Wextra

all: pc_tas pc_cas pc_nolock

pc_tas: producer_consumer.c
	$(CC) $(CFLAGS) -DUSE_CAS=0 producer_consumer.c -o pc_tas

pc_cas: producer_consumer.c
	$(CC) $(CFLAGS) -DUSE_CAS=1 producer_consumer.c -o pc_cas

pc_nolock: producer_consumer.c
	$(CC) $(CFLAGS) -DUSE_LOCK=0 -DNUM_PRODUCERS=1 -DNUM_CONSUMERS=1 producer_consumer.c -o pc_nolock

clean:
	rm -f pc_tas pc_cas pc_nolock

.PHONY: all clean
