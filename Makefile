# Variables
CC = gcc
CFLAGS = -std=gnu99 -Wall
LDFLAGS = -lrt -lpthread

all: slave app view2

slave: slave.c utils.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

app: application.c utils.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

view2: view2.c utils.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)


clean:
	rm -f slave app view2
.PHONY: all clean

