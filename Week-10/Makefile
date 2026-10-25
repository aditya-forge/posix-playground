CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -lm

TARGETS = srtf_cs rr_dynamic hrrn_priority rms

all: $(TARGETS)

srtf_cs: srtf_cs.c
	$(CC) $(CFLAGS) -o $@ srtf_cs.c

rr_dynamic: rr_dynamic.c
	$(CC) $(CFLAGS) -o $@ rr_dynamic.c

hrrn_priority: hrrn_priority.c
	$(CC) $(CFLAGS) -o $@ hrrn_priority.c

rms: rms.c
	$(CC) $(CFLAGS) -o $@ rms.c $(LDFLAGS)

test: all
	@echo "=== Testing Assignment 1: SRTF with Context Switch Overhead ==="
	./srtf_cs < in1_a.txt
	./srtf_cs < in1_b.txt
	./srtf_cs < in1_c.txt
	@echo "=== Testing Assignment 2: Dynamic Round Robin ==="
	./rr_dynamic < in2_a.txt
	./rr_dynamic < in2_b.txt
	./rr_dynamic < in2_c.txt
	@echo "=== Testing Assignment 3: Priority HRRN with Anti-Starvation ==="
	./hrrn_priority < in3_a.txt
	./hrrn_priority < in3_b.txt
	@echo "=== Testing Assignment 4: Rate Monotonic Scheduling ==="
	./rms < in4_a.txt
	./rms < in4_b.txt
	./rms < in4_c.txt

clean:
	rm -f $(TARGETS) *.exe

.PHONY: all test clean
