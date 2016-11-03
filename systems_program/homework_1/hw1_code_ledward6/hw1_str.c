#include<stdio.h>
#include<ctype.h>
#include<string.h>
#include<stdlib.h>
#include"hw1.h"

int str_manip(char * str, char * substr){
  size_t strLen = strlen(str);
  size_t newstrLen = strLen;
  size_t substrLen = strlen(substr);
  if(substrLen < 1 || strLen < 1){
    fprintf(stderr, "Given string length is 0\n");
    return -1;
  }
  int count = 0;
  char *newstr = (char *)malloc(sizeof(char) * strLen * 2 + 1);
  if(newstr == NULL){
    fprintf(stderr, "failed to allocate memory.\n");
    return -1;
  }
  char *lowerSubstr = (char *)malloc(sizeof(char) * substrLen + 1);
  if(lowerSubstr == NULL){
    fprintf(stderr, "failed to allocate memory.\n");
    return -1;
  }
  for(int i = 0; str[i]; i++){
    newstr[i] = tolower(str[i]);
  }

  for(int i = strLen - 1 ; i > -1 ; i --){
    newstr[newstrLen] = tolower(str[i]);
    newstrLen ++;
  }
  newstr[newstrLen] = '\0';

  for(int i = 0; substr[i]; i ++){
    lowerSubstr[i] = tolower(substr[i]);
  }
  char *temp = strstr(newstr, lowerSubstr);
  while(temp && substrLen > 0){
    count ++;
    temp += substrLen;
    temp = strstr(temp, lowerSubstr);
  }

  printf("str %s\n", str);
  printf("newstr %s\n", newstr);
  printf("substr %s\n", substr);
  printf("occurences %d\n", count);
  return 0;
}