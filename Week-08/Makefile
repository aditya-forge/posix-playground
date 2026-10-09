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
