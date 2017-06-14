#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#define MAX_PROCESS 300
#define PER_PROCESS_MAX_SIZE 21

int comma_needed = 0;
void _print_state(char *state){
  if(comma_needed == 1){
    printf(", ");
  }
  printf("%s", state);
  comma_needed = 1;
}
void print_state(long state){
  long i;
  comma_needed = 0;
  if(state == -1){
    _print_state("TASK_UNRUNNABLE");
  }
  if(state == 0){
    _print_state("TASK_RUNNING");
  }
  for(i = 0; i < 12; i ++){
    if((state & (1 << i) ) != 0){
      switch(i){
        case 0:
          _print_state("TASK_INTERRUPTIBLE");
          break;
        case 1:
          _print_state("TASK_UNINTERRUPTIBLE");
          break;
        case 2:
          _print_state("TASK_STOPPED");
          break;
        case 3: 
          _print_state("TASK_TRACED");
          break;
        case 6:
          _print_state("TASK_DEAD");
          break;
        case 7:
          _print_state("TASK_WAKEKILL");
          break;
        case 8:
          _print_state("TASK_WAKING");
          break;
        case 9:
          _print_state("TASK_PARKED");
          break;
        case 10:
          _print_state("TASK_NOLOAD"); 
          break;
        case 11:
          _print_state("TASK_NEW");        
          break;
      }
    }
  }
}
void print_ps(char ps_list[], int size){
  int offset = 0, bytes_read;
  char *temp_plist = ps_list;
  int pid, ppid, cpu;
  long state;
  while(temp_plist - ps_list < size - 1){
    bytes_read = sscanf(temp_plist, "%d %d %d %ld\n", &pid, &ppid, &cpu, &state);
    if(bytes_read < 1){
      return;
    }
    while(*temp_plist != '\n'){
      temp_plist++;
    }
    temp_plist ++;
    printf("PID=%d   PPID=%d   CPU=%d   ", pid, ppid, cpu);
    printf("STATE=");
    print_state(state);
    printf("\n");
  }
}
int main(){
  int filed, bytesret;
  int offset;
  char buffer[MAX_PROCESS * PER_PROCESS_MAX_SIZE];
  filed = open("/dev/process_list", O_RDONLY);
  if(filed < 0){
    perror("Failed to open process_list device");
    return errno;
  }
  bytesret = read(filed, buffer, sizeof(buffer));
  if(bytesret < 0){
    perror("Failed to receive message from process_list device");
    return errno;
  }
  print_ps(buffer, bytesret);
  close(filed);
  return 0;
} 