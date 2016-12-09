#include <stdio.h>
#include <stdlib.h>
#define MAXCACHE 3
#define MAXADDRESSLINES 4


typedef struct _Cache {
  int cacheNo;
  char current_state[MAXADDRESSLINES];
} CACHE;

int getInput(int * cacheNo, int *isRead, int * lineNo){
  char isReadchar;
  printf("\nEnter next access : ");
  if( scanf("%d%c%d", cacheNo, &isReadchar, lineNo) == 3 && *cacheNo < MAXCACHE && *lineNo < MAXADDRESSLINES){

    if(isReadchar == 'r'){
      *isRead = 0;
    }else{
      *isRead = 1;
    }
    return 1;
  }else{
    return 0;
  }
};

int busRead(CACHE *caches, int exceptCache, int lineNo){
  int status = 0, i = 0;
  for(i = 0; i < MAXCACHE; i ++){
    if(i == exceptCache) { 
      continue;
    }
    printf("Cache %d : Probe Read  %d\n  ", i, lineNo);
    int cacheState = caches[i].current_state[lineNo];
    switch(cacheState){
      case 'M':
        printf("ReadHit\n");
        caches[i].current_state[lineNo] = 'O';
        status ++;
        break;
      case 'O':
        printf("ReadHit\n");
        status ++;
        break;
      case 'E':
        printf("ReadHit\n");
        caches[i].current_state[lineNo] = 'S';
        status ++;
        break;
      case 'S':
        printf("ReadHit\n");
        status ++;
        break;
      case 'I':
        printf("ReadMiss\n");
        break;
    }
    printf("Cache %d Probe Read End:  %c -> %c \n", i, cacheState, caches[i].current_state[lineNo]);
  }
  return status;
};

int busWrite(CACHE *caches, int exceptCache, int lineNo){
  int status = 0, i = 0;
  for(i = 0; i < MAXCACHE; i ++){
    if(i == exceptCache) { 
      continue;
    }
    printf("Cache %d : Probe Write  %d\n  ", i, lineNo);
    int cacheState = caches[i].current_state[lineNo];
    switch(cacheState){
      case 'M':
      case 'E':
      case 'O':
        printf("WriteHit\n");
        caches[i].current_state[lineNo] = 'I';
        status ++;
        break;
      case 'S':
        printf("WriteHit\n");
        caches[i].current_state[lineNo] = 'I';
        break;
      case 'I':
        printf("WriteMiss\n");
        break;
    }
    printf("Cache %d Probe Write End:  %c -> %c \n", i, cacheState, caches[i].current_state[lineNo]);
  }
  return status;
};



void readLine(CACHE * caches, int cacheNo, int lineNo){
  printf("\nReading line %d in cache %d\n", lineNo, cacheNo);
  char cacheState = caches[cacheNo].current_state[lineNo];
  printf("Cache %d : Local Read\n  ", cacheNo);
  switch(cacheState){
    case 'M':
    case 'O':
    case 'E':
    case 'S':
      printf("ReadHit\n");
      break;
    case 'I':
      printf("ReadMiss\n");
      int isReadHit = busRead(caches, cacheNo, lineNo);
      if(isReadHit){
        caches[cacheNo].current_state[lineNo] = 'S';
      }else{
        caches[cacheNo].current_state[lineNo] = 'E';
      }
      break;
  }
  printf("Cache %d Local Read End:  %c -> %c \n", cacheNo, cacheState, caches[cacheNo].current_state[lineNo]);
}

void invalidate(CACHE *caches, int exceptCache, int lineNo){
  int i = 0;
  for(i = 0; i < MAXCACHE; i ++){
    if(i == exceptCache){
      continue;
    }
    printf("CACHE %d: Invalidate\n", i);
    char cacheState = caches[i].current_state[lineNo];
    caches[i].current_state[lineNo] = 'I';
    printf("Cache %d :  %c -> %c \n", i, cacheState, caches[i].current_state[lineNo]);
  }
}

void writeLine(CACHE *caches, int cacheNo, int lineNo){
  printf("\nWriting to line %d in cache %d\n", lineNo, cacheNo);
  char cacheState = caches[cacheNo].current_state[lineNo];
  printf("Cache %d : Local Write\n  ", cacheNo);
  switch(cacheState){
    case 'M':
      printf("WriteHit\n");
      break;
    case 'S':
    case 'O':
      printf("WriteHit\n");
      caches[cacheNo].current_state[lineNo] = 'M';
      invalidate(caches, cacheNo, lineNo);
      break;
    case 'E':
      printf("WriteHit\n");
      caches[cacheNo].current_state[lineNo] = 'M';
      break;
    case 'I':
      printf("WriteMiss\n");
      busWrite(caches, cacheNo, lineNo);
      caches[cacheNo].current_state[lineNo] = 'M';
      break;
  }
  printf("Cache %d Local Write End:  %c -> %c \n", cacheNo, cacheState, caches[cacheNo].current_state[lineNo]);
}

void initializeCaches(CACHE *caches){
  int i, j;
  for(i = 0; i < MAXCACHE; i ++){
    caches[i].cacheNo = i;
    for(j = 0; j < MAXADDRESSLINES; j ++){
      caches[i].current_state[j] = 'I';
    }
  }
}

int main(){
  printf("MOESI Protocol \n");
  printf("3 Caches and 4 Address Lines \n");
  int cacheNo, isRead, lineNo;
  CACHE *caches = malloc(sizeof(CACHE) * MAXCACHE);
  initializeCaches(caches);
  while(getInput(&cacheNo, &isRead, &lineNo) == 1){
    if(isRead == 0){
      readLine(caches, cacheNo, lineNo);
    }else{
      writeLine(caches, cacheNo, lineNo);
    }
  }
  return 1;
}