CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -std=c11 -I$(RESDIR)
RESDIR  = PA2_resource_beta

all: bfproc netproc

bfproc: nodeproc.c $(RESDIR)/bf.h
	$(CC) $(CFLAGS) -o $@ $<

netproc: $(RESDIR)/netproc.c $(RESDIR)/bf.h
	$(CC) $(CFLAGS) -o $@ $< -lpthread

clean:
	rm -f netproc bfproc

.PHONY: all clean
