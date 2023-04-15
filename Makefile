CC = gcc
CFLAGS = -Wall -g

OBJFILES = driver.o utils.o process_manager.o memory.o

all: allocate

allocate: $(OBJFILES)
	$(CC) $(CFLAGS) -o allocate $(OBJFILES)

main.o: driver.c
	$(CC) $(CFLAGS) -c driver.c

utils.o: utils.c
	$(CC) $(CFLAGS) -c utils.c

process_manager.o: process_manager.c
	$(CC) $(CFLAGS) -c process_manager.c

memory.o: memory.c
	$(CC) $(CFLAGS) -c memory.c

.PHONY: clean

clean:
	rm -f allocate *.o