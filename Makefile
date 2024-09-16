# Variables
CC = gcc
CFLAGS = -std=gnu99 -Wall
LDFLAGS = -lrt -lpthread

all: slave app view

slave: slave.c utils.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

app: application.c utils.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

view: view.c utils.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)


clean:
	rm -f slave app view
.PHONY: all clean

