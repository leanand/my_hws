#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
int main(){
  int filed, bytesret;
  char buffer[18];
  filed = open("/dev/process_list", O_RDONLY);
  if(filed < 0){
    perror("Failed to open process_list device");
    return errno;
  }
  bytesret = read(filed, buffer, 18);
  if(bytesret < 0){
    perror("Failed to receive message from process_list device");
    return errno;
  }
  printf("Message received: %s\n", buffer);
  close(filed);
  return 0;
} 