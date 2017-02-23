#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int actual_height; 
int initial_height;

void do_log(const char *format, ...){
  va_list vargs;
  int i;
  for(i = actual_height; i <initial_height ; i ++){
    printf("\t");
  }
  printf("%d : ", getpid());
  va_start(vargs, format);
  vprintf(format, vargs);
  va_end(vargs);
  printf("\n");
}

char ** getArgument(char* process_name, int height, int child_count){
  char **args = malloc(5 * sizeof(char *));
  args[0] = process_name;
  args[1] = malloc(4 * sizeof(char));
  args[2] = malloc(4 * sizeof(char));
  args[3] = malloc(4 * sizeof(char));
  args[4] = 0;
  sprintf(args[1], "%d", height);
  sprintf(args[2], "%d", child_count);
  sprintf(args[3], "%d", initial_height);
  return args;
}

void create_child_processes(char * process_name, int height, int child_count){
  int i, status;
  char **args = getArgument(process_name, height - 1, child_count);
  do_log("Creating %d children at height %d", child_count, height - 1);
  for(i = 0 ; i < child_count; i ++){
    int pid = fork();
    if(pid < 0){
      fprintf(stderr, "Error in forking => %s \nExiting", strerror(errno));
      abort();
    }else if(pid == 0){
      execvp(process_name,  args);
    }
  }
  while(i >= 0){
    wait(&status);
    i --;
  }
} 

int main(int args, char * argv[]){
  if(args != 3 && args != 4){
    fprintf(stderr, "Invalid arguments.. Needed 2 but given %d\nExiting..\n", args - 1);
    return -1;
  }
  char * process_name = argv[0];
  int height = atoi(argv[1]);
  actual_height = height;
  int child_count = atoi(argv[2]);
  if(args == 4){
    initial_height = atoi(argv[3]);
  }else{
    initial_height = height;
  }
  do_log("Process starting");  
  do_log("Parent's id = %d", getppid());
  do_log("Height in the tree = %d", height);
  if(height > 1){
    create_child_processes(process_name, height, child_count);
  }
  do_log("Terminating at height %d", height);
  return 1;
}