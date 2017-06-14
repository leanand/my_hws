//User-level Program

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
 
#define BUFFER_LENGTH 2048               
static char buffer[BUFFER_LENGTH];     

char * getState(int s){
	char * state;
	if(s == 0){
		state = "TASK_RUNNING";
	}else if(s == 1){
		state = "TASK_INTERRUPTIBLE";
	}else if(s == 2){
		state = "TASK_UNINTERRUPTIBLE";
	}else if(s == 4){
		state = "TASK_STOPPED";
	}else if(s == 8){
		state = "TASK_TRACED";
	}else if(s == 64){
		state = "TASK_DEAD";
	}else if(s == 128){
		state = "TASK_WAKEKILL";
	}else if(s == 256){
		state = "TASK_WAKING";
	}else if(s == 512){
		state = "TASK_PARKED";
	}else if(s == 1024){
		state ="TASK_NOLOAD";
	}else if(s == 2048){
		state ="TASK_NEW";
	}else if(s == 4096){
		state ="TASK_STATE_MAX";
	}else if(s == 100){
		state ="TASK_READY ,TASK_RUNNING , TASK_BLOCKED, TASK_TERMINATED, TASK_DEAD";
	}else{
		state = "TASK_INVALID";		
	}
	return state;
}
 
int main(){
   int bytes_read, fd , count = 0;
      fd = open("/dev/chardev", O_RDONLY); 
	printf("%d",fd);
	if(fd < 0){
	printf("Device cannot be registered");
	return 0;
   }      
   printf("Reading from the Device \n");

   while(buffer != "EOF"){
	int pid,ppid,cpu,s;
	count++;
   	bytes_read = read(fd, buffer, BUFFER_LENGTH); 
   	if(strcmp(buffer,"EOF") == 0){
		printf("Read Complete \n");
	break;
	}	
	sscanf(buffer,"%d,%d,%d,%d",&pid,&ppid,&cpu,&s);
	printf("PID : %d PPID : %d CPU : %d STATE : %s\n",pid,ppid,cpu,getState(s));	
   }

   return 0;
   close(fd);
}
