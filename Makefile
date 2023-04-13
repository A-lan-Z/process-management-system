CC = gcc
CFLAGS = -Wall

OBJFILES = main.o other_source_file.o

all: allocate

allocate: $(OBJFILES)
	$(CC) $(CFLAGS) -o allocate $(OBJFILES)

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

other_source_file.o: other_source_file.c
	$(CC) $(CFLAGS) -c other_source_file.c

.PHONY: clean

clean:
	rm -f allocate *.o