#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include "knapsack.h"

#define MAX_BENEFIT 0
#define MIN_WEIGHT 1
#define MAX_WEIGHT 2
#define MAX_BENEFIT_PER_UNIT 3

void find_maximum(int * arr, int size, int * index ){
    int i = 0;
    int ind= -1;
    int value = -1;
    for(i = 0; i < size; i ++){
      if(arr[i] > value){
        value = arr[i];
        ind = i;
      }
    }
    *index = ind;
}

void find_minimum(int * arr, int size, int * index ){
    int i = 0;
    int ind= -1;
    int value = INT_MAX;
    for(i = 0; i < size; i ++){
      if(arr[i] < value){
        value = arr[i];
        ind = i;
      }
    }
    *index = ind;
}

void perform_greedy_max_benefit_first(KNAPSACK * ks){
  int temp_profits[ks->size];
  int still_left = ks->size;
  memcpy(temp_profits, ks->profits, ks->size * sizeof(int));
  int added_to_ks[ks->size];
  int added_count = 0;
  int total_profit = 0;
  int total_weight = 0;
  int index;
  int i;
  while(total_weight <= ks->knapsack_capacity && still_left > 0){
    find_maximum(temp_profits, ks->size, &index);
    if(index == -1){
      printf("Cannot find maximum value");
      abort();
    }
    temp_profits[index] = -1;
    still_left --;

    if(total_weight + ks->weights[index] > ks->knapsack_capacity){
      continue;
    }
    added_to_ks[added_count] = index;
    added_count ++;

    total_weight  += ks->weights[index];
    total_profit += ks->profits[index];
  }
  if(added_count == 0){
    printf("Cannot find any pairs");
  }else{
    printf("\nMax Benefit Approach\n");
    printf("------------------------\n");
    printf("Index\tWeight\tProfit\n");
    for(i = 0; i < added_count; i ++){
      index = added_to_ks[i];
      printf("%d\t%d\t%d\n", index, ks->weights[index], ks->profits[index]);
    }
    printf("------------------------\n");
    printf("Total\t%d\t%d\n", total_weight, total_profit);

  }
};
void perform_greedy_minimum_weight_first(KNAPSACK * ks){
  int temp_weights[ks->size];
  int still_left = ks->size;
  memcpy(temp_weights, ks->weights, ks->size * sizeof(int));
  int added_to_ks[ks->size];
  int added_count = 0;
  int total_profit = 0;
  int total_weight = 0;
  int index;
  int i;
  while(total_weight <= ks->knapsack_capacity && still_left > 0){
    find_minimum(temp_weights, ks->size, &index);
    if(index == -1){
      printf("Cannot find minimum value");
      abort();
    }
    temp_weights[index] = INT_MAX;
    still_left --;
    
    if(total_weight + ks->weights[index] > ks->knapsack_capacity){
      continue;
    }
    added_to_ks[added_count] = index;
    added_count ++;

    total_weight  += ks->weights[index];
    total_profit += ks->profits[index];
  }
  if(added_count == 0){
    printf("Cannot find any pairs");
  }else{
    printf("\nMin Weight Approach\n");
    printf("------------------------\n");
    printf("Index\tWeight\tProfit\n");
    for(i = 0; i < added_count; i ++){
      index = added_to_ks[i];
      printf("%d\t%d\t%d\n", index, ks->weights[index], ks->profits[index]);
    }
    printf("------------------------\n");
    printf("Total\t%d\t%d\n", total_weight, total_profit);
  }
}

void perform_greedy_maximum_weight_first(KNAPSACK * ks){
  int temp_weights[ks->size];
  int still_left = ks->size;
  memcpy(temp_weights, ks->weights, ks->size * sizeof(int));
  int added_to_ks[ks->size];
  int added_count = 0;
  int total_profit = 0;
  int total_weight = 0;
  int index;
  int i;
  while(total_weight <= ks->knapsack_capacity && still_left > 0){
    find_maximum(temp_weights, ks->size, &index);
    if(index == -1){
      printf("Cannot find maximum value");
      abort();
    }
    temp_weights[index] = -1;
    still_left --;
    
    if(total_weight + ks->weights[index] > ks->knapsack_capacity){
      continue;
    }
    added_to_ks[added_count] = index;
    added_count ++;

    total_weight  += ks->weights[index];
    total_profit += ks->profits[index];
  }
  if(added_count == 0){
    printf("Cannot find any pairs");
  }else{
    printf("\nMax Weight Approach\n");
    printf("------------------------\n");
    printf("Index\tWeight\tProfit\n");
    for(i = 0; i < added_count; i ++){
      index = added_to_ks[i];
      printf("%d\t%d\t%d\n", index, ks->weights[index], ks->profits[index]);
    }
    printf("------------------------\n");
    printf("Total\t%d\t%d\n", total_weight, total_profit);
  }
}

void perform_greedy_max_benefit_per_unit_first(KNAPSACK * ks){
  int temp_ben_per_unit[ks->size];
  int still_left = ks->size;
  int i;
  for(i = 0; i < ks->size; i ++){
    temp_ben_per_unit[i] = ks->profits[i] / ks->weights[i];
  }
  int added_to_ks[ks->size];
  int added_count = 0;
  int total_profit = 0;
  int total_weight = 0;
  int index;
  while(total_weight <= ks->knapsack_capacity && still_left > 0){
    find_maximum(temp_ben_per_unit, ks->size, &index);
    if(index == -1){
      printf("Cannot find maximum value");
      abort();
    }
    temp_ben_per_unit[index] = -1;
    still_left --;

    if(total_weight + ks->weights[index] > ks->knapsack_capacity){
      continue;
    }
    added_to_ks[added_count] = index;
    added_count ++;

    total_weight  += ks->weights[index];
    total_profit += ks->profits[index];
  }
  if(added_count == 0){
    printf("Cannot find any pairs");
  }else{
    printf("\nMax Benefit Per Unit Approach\n");
    printf("------------------------\n");
    printf("Index\tWeight\tProfit\n");
    for(i = 0; i < added_count; i ++){
      index = added_to_ks[i];
      printf("%d\t%d\t%d\n", index, ks->weights[index], ks->profits[index]);
    }
    printf("------------------------\n");
    printf("Total\t%d\t%d\n", total_weight, total_profit);

  }
};

int main(int args, char **argv){
  if(args != 2){
    printf("Invalid Arguments\nUsage: ./Q1-1_brute_force_knapsack <input_file>\n");
    abort();
  }
  KNAPSACK * ks = (KNAPSACK *) malloc(sizeof(KNAPSACK));
  parse_input(argv[1], ks);
  display_knap(ks);
  printf("Greedy Algorithms : \n");
  printf("------------------------ \n");
  perform_greedy_max_benefit_first(ks);
  perform_greedy_minimum_weight_first(ks);
  perform_greedy_maximum_weight_first(ks);
  perform_greedy_max_benefit_per_unit_first(ks);
  free_knap(ks);
}