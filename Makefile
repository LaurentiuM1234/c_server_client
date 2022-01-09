CC = gcc
CFLAGS = -Wall
LIBS = -lm
.PHONY: all clean server subscriber

all: server subscriber

server: $(wildcard serv/*.c *.c)
	$(CC) $(CFLAGS) $^ -o server $(LIBS)
subscriber: $(wildcard cli/*.c *.c)
	$(CC) $(CFLAGS) $^ -o subscriber $(LIBS)

clean: server subscriber
	rm server
	rm subscriber
