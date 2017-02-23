#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "mybarrier.h"


mybarrier_t * mybarrier_init(unsigned int count)
{
  mybarrier_t * barrier = malloc(sizeof(*barrier));
	if (NULL == barrier) {
    return NULL;
  }
  if(pthread_mutex_init(&barrier->mutex, NULL) != 0){
    printf("Error in initiating pthread_mutex -> %s\n", strerror(errno));
    return NULL;
  }

  if(pthread_cond_init(&barrier->cond, NULL) != 0){
    pthread_mutex_destroy(&barrier->mutex);
    printf("Error in initiating pthread_cond -> %s\n", strerror(errno));
    return NULL;
  }
	barrier->count = count;
  barrier->event_occured = 0;
  barrier->executed_count = 0;
  return barrier;	
}

int mybarrier_destroy(mybarrier_t * barrier)
{
  int s;
  
  s = pthread_mutex_lock(&barrier->mutex);
  if(s != 0){
    printf("Error in barrier destroy : mutex lock, %s\n", strerror(s));
    return -1;
  }
  barrier->event_occured ++;
  
  s = pthread_cond_broadcast(&barrier->cond);
  if(s != 0){
    printf("Error in barrier destroy , %s\n", strerror(s));
    return -1;
  }
  s = pthread_mutex_unlock(&barrier->mutex);
  if(s != 0){
    printf("Error in barrier destroy : mutex unlock, %s\n", strerror(s));
    return -1;
  }

  s = pthread_cond_destroy(&barrier->cond);
  if(s != 0){
    printf("Error in destroying pthread_cond -> %s\n", strerror(s));
    return -1;
  }
  s = pthread_mutex_destroy(&barrier->mutex);
  if(s != 0){
    printf("Error in destroying pthread_mutex-> %s\n", strerror(s));
    return -1;
  }
	free(barrier);

	return 0;
}

int mybarrier_wait(mybarrier_t * barrier)
{
  int status;

  if(NULL == barrier){
    return -1;
  }

  status = pthread_mutex_lock(&barrier->mutex);
  if(status != 0){
    printf("Error in locking pthread_mutex-> %s\n", strerror(status));
    return -1;    
  }
  
  if(barrier->event_occured != 0){
    pthread_mutex_unlock(&barrier->mutex);
    return -1;
  }
  
  barrier->executed_count ++;
  if(barrier->executed_count < barrier->count){
    do {
      status = pthread_cond_wait(&barrier->cond, &barrier->mutex);      
      if(status != 0){
        printf("Error in waiting for conditional signal -> %s\n", strerror(errno));   
      }
    }while(barrier->event_occured == 0);
    pthread_mutex_unlock(&barrier->mutex);
  }else{
    barrier->event_occured ++;
    status = pthread_cond_broadcast(&barrier->cond);
    if(status != 0){
       printf("Error in broadcasting condition signal -> %s\n", strerror(errno));
    }
  
    if(pthread_mutex_unlock(&barrier->mutex) != 0){
      printf("Error in unlocking mutex lock");
      return -1;
    }
  }
	return 0;
}


