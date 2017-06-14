CFLAGS=-Wall -lm -pthread -g

.c.o:
	gcc -g -c $?

# compile client and server
all: confserver

# compile server program
confserver: multi-threaded-confserver.o confutils.o
	gcc $(CFLAGS) -o proxy multi-threaded-confserver.o  confutils.o

clean:
	rm -f *.o multi-threaded-confserver
