/*
 *  barrier1.c
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include "mybarrier.h"

static mybarrier_t *barrier;

typedef struct thread_arg{
    int index;
} thread_arg;

void * thread_fun (void * args)
{
    thread_arg * atgs = args;
    int index = atgs->index;
    free(args);
    time_t  now;
    time (&now);
    printf ("thread %d starting at %s\n", index, ctime (&now));
    sleep(index);
    printf ("thread %d woke at %s, now waiting\n", index, ctime (&now));
    int status = mybarrier_wait(barrier);
    printf("Print status %d\n", status);
    time (&now);
    printf ("thread %d done at %s\n", index, ctime (&now));
}

int main () // ignore arguments
{
    barrier = mybarrier_init(50);
    time_t  now;
    pthread_t *threads = malloc(sizeof(pthread_t) * 100);
    // create a barrier object with a count of 3

    // start up two threads, thread1 and thread2
    for(int i =0 ; i < 100; i ++){
        thread_arg *atgs = malloc(sizeof(thread_arg));
        atgs->index = i;
        pthread_create(&threads[i], NULL, thread_fun, atgs);
    }
    // mybarrier_destroy(barrier);
    printf ("main() waiting for barrier at %s", ctime (&now));
    mybarrier_destroy(barrier);
    printf("============= main thread =======\n");
    // now wait for completion
    time (&now);
    // after this point, all three threads have completed.
    // mybarrier_wait(barrier);

    printf ("barrier in main() done at %s", ctime (&now));
    free(threads);
    return (EXIT_SUCCESS);
}