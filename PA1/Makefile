CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c11 -Wpedantic -march=native
LDFLAGS = -lm -lpthread

# Dev flags: sanitizers + debug info (not for submission)
DEV_CFLAGS = $(CFLAGS) -g -mno-avx2 -fsanitize=address -fsanitize=pointer-compare -fsanitize=pointer-subtract

# Detect the operating system
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    LDFLAGS += -latomic
endif

all: server client

.PHONY: all dev clean

server: server.c
	$(CC) $(CFLAGS) -o server server.c $(LDFLAGS)

client: client.c
	$(CC) $(CFLAGS) -o client client.c $(LDFLAGS)

dev: server.c client.c
	$(CC) $(DEV_CFLAGS) -o server server.c $(LDFLAGS)
	$(CC) $(DEV_CFLAGS) -o client client.c $(LDFLAGS)

clean:
	rm -f server client
