CC=gcc
CFLAGS=-c -Wall
LDFLAGS = -pthread

all: mutex defrag

mutex: mutex.o
	$(CC) $(LDFLAGS) mutex.o -o mutex

defrag: defrag.o
	$(CC) $(LDFLAGS) defrag.o -o defrag

mutex.o: mutex.c
	$(CC) $(LDFLAGS) $(CFLAGS) mutex.c

defrag.o: defrag.c
	$(CC) $(LDFLAGS) $(CFLAGS) defrag.c

clean:
	rm -rf *.o mutex defrag
