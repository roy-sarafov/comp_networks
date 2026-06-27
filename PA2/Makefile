CC     = gcc
CFLAGS = -Wall -Wextra -O2 -std=c11 -D_POSIX_C_SOURCE=200809L

all: bfproc netproc

bfproc: nodeproc.c bf.h
	$(CC) $(CFLAGS) -o $@ nodeproc.c

netproc: netproc.c bf.h
	$(CC) $(CFLAGS) -o $@ netproc.c -lpthread

clean:
	rm -f netproc bfproc

.PHONY: all clean
