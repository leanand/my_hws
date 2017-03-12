CFLAGS=-Wall -lm -pthread -g

.c.o:
	gcc -g -c $?

# compile client and server
all: confclient confserver

# compile client only
confclient: confclient.o confutils.o
	gcc $(CFLAGS) -o confclient confclient.o  confutils.o

# compile server program
confserver: multi-threaded-confserver.o confutils.o
	gcc $(CFLAGS) -o multi-threaded-confserver multi-threaded-confserver.o  confutils.o

clean:
	rm -f *.o confclient multi-threaded-confserver
