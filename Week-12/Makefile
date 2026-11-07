CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -Werror -pedantic

bankers: bankers.c
	$(CC) $(CFLAGS) -o bankers bankers.c

test: bankers
	@for t in tests/test*.txt; do echo "== $$t"; ./bankers < $$t; done

clean:
	rm -f bankers

.PHONY: test clean
