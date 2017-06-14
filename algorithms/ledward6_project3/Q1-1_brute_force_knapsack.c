#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "knapsack.h"

void perform_brute_force(KNAPSACK * ks){
  unsigned int power_size = pow(2, ks->size);
  int i , j;
  int total_profit = 0;
  int total_weight = 0;
  int required_comb = -1;
  int temp_weight = 0;
  int temp_profit = 0;
  for(i = 0;i < power_size; i ++){
    temp_profit = 0;
    temp_weight = 0;
    for(j = 0; j  < ks->size; j ++){
      if(i & (1 << j)){
        temp_weight += ks->weights[j];
        temp_profit  += ks->profits[j];
      }
    }
    if(temp_weight <= ks->knapsack_capacity && temp_profit > total_profit){
        required_comb = i;
        total_profit = temp_profit;
        total_weight = temp_weight;
    }
  }
  if(required_comb == -1){
    printf("Cannot find any pairs \n");
    return;
  }
  printf("\nBrute Force Approach\n");
  printf("The selected items are :\nIndex\tWeight\tProfit\n");
  for(j = 0; j < ks->size; j ++){
    if(required_comb & (1 << j)){
      printf("%d\t%d\t%d\n", j, ks->weights[j], ks->profits[j]);
    }   
  }
  printf("------------------------\n");
  printf("Total\t%d\t%d\n", total_weight, total_profit);
}
int main(int args, char **argv){
  if(args != 2){
    printf("Invalid Arguments\nUsage: ./Q1-1_brute_force_knapsack <input_file>\n");
    abort();
  }
  KNAPSACK * ks = (KNAPSACK *) malloc(sizeof(KNAPSACK));
  parse_input(argv[1], ks);
  display_knap(ks);
  perform_brute_force(ks);
  free_knap(ks);
}