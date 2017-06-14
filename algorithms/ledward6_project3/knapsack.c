#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "knapsack.h"

void init_array(int *weights, int size){
  int i = 0;
  for(i = 0; i < size; i ++){
    weights[i]= 0;
  }
}

void parse_input(char * file_name, KNAPSACK * ks){
  char temp_line[MAX_BUFFER];
  int number_of_inputs;
  char * token;
  FILE * input_file;
  int i;
  if((input_file = fopen(file_name, "r") )== NULL){
    printf("Error in opening file\n");
    abort();
  }
  if(fgets(temp_line, MAX_BUFFER, input_file) == NULL){
    printf("Cannot parse file : 1\n");
    abort();
  }
  sscanf(temp_line, "%d", &number_of_inputs);
  ks->size = number_of_inputs;
  ks->weights = malloc(number_of_inputs * sizeof(int));
  ks->profits = malloc(number_of_inputs * sizeof(int));
  init_array(ks->weights, number_of_inputs);
  init_array(ks->weights, number_of_inputs);
  if(fgets(temp_line, MAX_BUFFER, input_file) == NULL ){
    printf("Cannot parse file : 2\n");
    abort();
  }
  
  for(i = 0; i < number_of_inputs; i ++){
    if(i == 0){
      token = strtok(temp_line,",");
    }else{
      token = strtok(NULL, ",");
    }
    ks->weights[i] = atoi(token);
  }
  if(fgets(temp_line, MAX_BUFFER, input_file) == NULL ){
    printf("Cannot parse file : 2\n");
    abort();
  }
  for(i = 0; i < number_of_inputs; i ++){
    if(i == 0){
      token = strtok(temp_line,",");
    }else{
      token = strtok(NULL, ",");
    }
    ks->profits[i] = atoi(token);
  }
  if(fgets(temp_line, MAX_BUFFER, input_file) == NULL ){
    printf("Cannot parse file : 2\n");
    abort();
  }
  sscanf(temp_line, "%d", &ks->knapsack_capacity);
  fclose(input_file);
}
void display_knap(KNAPSACK *ks){
  int i = 0;
  printf("Total number of elements: %d\n", ks->size);
  printf("Weights of elements: ");
  for(i = 0; i < ks->size; i ++){
    printf("%d ", ks->weights[i]);
  }
  printf("\nProfits of elements: ");
  for(i = 0; i < ks->size; i ++){
    printf("%d ", ks->profits[i]);
  }
  printf("\nKnapsack Capacity: %d\n", ks->knapsack_capacity);
}

void free_knap(KNAPSACK * ks){
  if(ks == NULL){
    return;
  }
  free(ks->weights);
  free(ks->profits);
  free(ks);
}
