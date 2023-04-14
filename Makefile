CC = gcc
CFLAGS = -Wall -g

OBJFILES = main.o utils.o process_manager.o

all: allocate

allocate: $(OBJFILES)
	$(CC) $(CFLAGS) -o allocate $(OBJFILES)

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

utils.o: utils.c
	$(CC) $(CFLAGS) -c utils.c

process_manager.o: process_manager.c
	$(CC) $(CFLAGS) -c process_manager.c

.PHONY: clean

clean:
	rm -f allocate *.o