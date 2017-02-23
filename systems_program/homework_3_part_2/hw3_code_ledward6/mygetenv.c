/* Task: using thread-specific data to implment a thread-safe version of mygetenv() */

#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define MAXSTRINGSZ 4096
extern char **environ;

static pthread_once_t once = PTHREAD_ONCE_INIT;
static pthread_key_t strerrorkey;

static void destructor (void * buf){
  free(buf);
}

static void createKey(void){
  int s = pthread_key_create(&strerrorkey, destructor);
  if(s != 0){
    printf("Error in creating key");
    abort();
  }
}

char * mygetenv(const char *name)
{
  char * buf;  
  int  i, len;
  int s = pthread_once(&once, createKey);
  if(s != 0){
    printf("Error in creating key");
    abort(); 
  }
  buf = pthread_getspecific(strerrorkey);
  if( buf == NULL){
    buf = malloc(MAXSTRINGSZ);
    s = pthread_setspecific(strerrorkey, buf);
    if(s != 0){
      printf("Error in setting key");
      abort();
    }
  }

  len = strlen(name);
  for (i = 0; environ[i] != NULL; i++){
    if ((strncmp(name, environ[i], len) == 0) && (environ[i][len] == '=')){
      strncpy(buf, &environ[i][len+1], MAXSTRINGSZ-1);
      buf[MAXSTRINGSZ-1] = '\0';
    } 
  }

	return buf;
}