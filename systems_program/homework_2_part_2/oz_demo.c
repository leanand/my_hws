#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>

long int time_difference(long int start_time){
  struct timeval current_time;  
  gettimeofday(&current_time, NULL);
  return current_time.tv_usec - start_time;
}

int main(){
  struct timeval current_time;  
  gettimeofday(&current_time, NULL);
  long int start_time = current_time.tv_usec;
  pid_t pid;
  pid_t ppid;
  pid_t ppid_2;
  int wait_status;
  printf("          OzDemo\n");
  printf("=================================\n");
  int parent = fork();
  if(parent ==0 ){
    int child = fork();
    if(child == 0){
      printf("%ldus: Child process(pid: %ld, ppid:%ld) created\n", time_difference(start_time), (long)getpid(), (long)getppid());
      ppid_2 = getppid();
      while((ppid = getppid()) != 1){
      }
      printf("%ldus: Parent process terminated, child process(pid: %ld, ppid:%ld, prev ppid: %d) adopted by init process\n", time_difference(start_time), (long)getpid(), (long)getppid(), ppid_2);
      exit(0);
    }else{
      printf("%ldus: Parent process(pid: %ld, ppid:%ld) created\n",  time_difference(start_time), (long)getpid(), (long)getppid());
      sleep(3);
      printf("%ldus: Parent process(pid: %ld, ppid:%ld) terminating\n",  time_difference(start_time), (long)getpid(), (long)getppid());
      exit(0);
    }
  }else{
    printf("%ldus: Grandparent process(pid: %ld, ppid:%ld) created \n", time_difference(start_time),  (long)getpid(), (long)getppid());
    sleep(6);
    printf("%ldus: Grandparent process(pid: %ld, ppid:%ld) calling wait() \n", time_difference(start_time),  (long)getpid(), (long)getppid());
    pid = wait(&wait_status);
    printf("%ldus: Pid returned by wait() : %d , wait_status returned by wait(): %d\n",time_difference(start_time), pid, wait_status);
    printf("%ldus: Grandparent process(pid: %ld, ppid:%ld) terminating\n", time_difference(start_time),  (long)getpid(), (long)getppid());
    exit(0);
  }
  return 0;
}