#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#define MAX_LINE_SIZE 64

char * readline(int fd, int col1, int col2, int* no_of_chars){
  char *req_output =NULL;
  char *buffer[MAX_LINE_SIZE];
  char *tempBuffer = (char *)calloc(1, MAX_LINE_SIZE);
  int bytes_read= 0;
  int index = 0;
  int no_of_cols = 0;
  char buf ;
  char temp1[MAX_LINE_SIZE];
  char temp2[MAX_LINE_SIZE];
  while((bytes_read = read(fd, &buf, 1))){
    if(buf == '='){
      return NULL;
    }
    (*no_of_chars)++;
    if(buf == ' ' || buf == '\t' || buf =='\n'){
      buffer[index] = (char *) calloc(1, strlen(tempBuffer) + 1);
      no_of_cols ++;
      strcpy(buffer[index], tempBuffer);
      memset(tempBuffer, '\0', strlen(tempBuffer) + 1);
      index ++;  
      if(buf == '\n'){
        break;
      }
    }else{
      strncat(tempBuffer, &buf, 1);
    }
  } 

  if(no_of_cols == 0){
    return NULL;
  }
  if(no_of_cols > 0 && col1 < no_of_cols && col2 < no_of_cols){
    memset(tempBuffer, '\0', strlen(tempBuffer) + 1);
    strcpy(tempBuffer, buffer[col1]);
    strcpy(buffer[col1], buffer[col2]);
    strcpy(buffer[col2], tempBuffer);
  } 
  free(tempBuffer);
  if(no_of_cols> 0){
    req_output = calloc(1, MAX_LINE_SIZE);
    for(int i=0; i <no_of_cols; i++){
      strcat(req_output, buffer[i]);
      strncat(req_output, " ", 1);
      free(buffer[i]);
    }
  }
  return req_output;
}

int main(int argv, char * args[]){
  if(argv != 4){
    printf("Invalid Arguments\n");
    return 1;
  }
  char * path = args[1];
  int col1 = atoi(args[2]) - 1;
  int col2 = atoi(args[3]) - 1;

  int fd = open(path, O_RDWR | O_APPEND);
  if(fd < 0){
    printf("File cannot be opened\n");
    return 1;
  }
  lseek(fd, -1, SEEK_END);
  char is_EOF;
  read(fd, &is_EOF, 1);
  if(is_EOF == '\n'){
    write(fd, "===\n", 4);
  }else{
    write(fd, "\n===\n", 5);
  }
  lseek(fd, 0, SEEK_SET);
  char *req_output = NULL;
  char * temp;
  int* no_of_chars = malloc(sizeof(int));
  int line = 0;

  while((temp = readline(fd, col1, col2, no_of_chars))){
    if(*no_of_chars > 0){
      write(fd, temp, strlen(temp));
      write(fd,"\n", 1);
      lseek(fd, *no_of_chars , SEEK_SET);  
    }
  }  
  
  return 0;
}
