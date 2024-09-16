## quiero: gcc application.c -std=gnu17 -lrt -lpthread utils.c -o app

# Variables
CC = gcc
CFLAGS = -std=gnu17
LDFLAGS = -lrt -lpthread
SRC = application.c utils.clean

# Reglas
all: slave app view


app: application.c utils.c
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@	

slave: slave.c utils.c
	$(CC) $(CFLAGS) $^ $(LDFLAGS) $^ -o $@

view: view.c utils.c
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean