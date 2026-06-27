CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -std=c11 -I$(RESDIR)
RESDIR  = PA2_resource_beta

# Default target: build the node process (bfproc).
# netproc uses C11 threads (threads.h) which requires glibc (Linux).
# On Linux:  make all
# On macOS:  make mac   (uses pthread-based netproc_mac.c for local testing)
all: bfproc

bfproc: nodeproc.c $(RESDIR)/bf.h
	$(CC) $(CFLAGS) -o $@ $<

netproc: $(RESDIR)/netproc.c $(RESDIR)/bf.h
	$(CC) $(CFLAGS) -o $@ $< -lpthread

# macOS local testing only – DO NOT SUBMIT netproc_mac.c
mac: bfproc netproc_mac

netproc_mac: netproc_mac.c $(RESDIR)/bf.h
	$(CC) $(CFLAGS) -o $@ $< -lpthread

clean:
	rm -f netproc bfproc

.PHONY: all clean
