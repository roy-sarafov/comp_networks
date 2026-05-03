CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c11 -Wpedantic -march=native -g -mno-avx2 -fsanitize=address -fsanitize=pointer-compare -fsanitize=pointer-subtract
LDFLAGS = -lm -lpthread

# Detect the operating system
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    LDFLAGS += -latomic
endif

all: server client

.PHONY: all clean

server: server.c
	$(CC) $(CFLAGS) -o server server.c $(LDFLAGS)

client: client.c
	$(CC) $(CFLAGS) -o client client.c $(LDFLAGS)

clean:
	rm -f server client