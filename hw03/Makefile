CC = gcc
CFLAGS = -Wall $(EXTRA_CFLAGS)

NSD = nsd_main.c nsd.c nd.c nd.h nsd.h
FORKPIPE = forkpipe.c

all: forkpipe nsd

forkpipe: $(FORKPIPE)
	$(CC) $(CFLAGS) -o $@ $^

nsd: $(NSD)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f forkpipe nsd