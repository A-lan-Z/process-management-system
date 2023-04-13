CC = gcc
CFLAGS = -Wall -g

OBJFILES = main.o utils.o

all: allocate

allocate: $(OBJFILES)
	$(CC) $(CFLAGS) -o allocate $(OBJFILES)

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

utils.o: utils.c
	$(CC) $(CFLAGS) -c utils.c

.PHONY: clean

clean:
	rm -f allocate *.o