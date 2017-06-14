#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include "knapsack.h"

void perform_dynamic_pragramming(KNAPSACK * ks){
  int n = ks->size;
  int W = ks->knapsack_capacity;
  int i, j;
  int table[n + 1][W + 1];
  int required_arr[n];
  int added_count = 0;
  int total_profit = 0;
  int total_weight = 0;
  int index;
  int temp;
  for(i = 0 ; i <= W ; i ++){
    table[0][i] = 0;
  }
  for(i = 1 ; i <= n ; i ++){
    table[i][0] = 0;
    for(j = 1;j <= W ; j ++){
      if(ks->weights[ i-1 ] <= j){
        temp = table[ i - 1 ][j - ks->weights[ i-1]] + ks->profits[ i-1];
        if( temp  > table[i-1][j]){
          table[i][j] =  temp;
        }else{
          table[i][j] = table[i -1][j];
        }
      }else{
          table[i][j] = table[i -1][j];
      }
    }
  } 
    printf("Index\tWeight\tProfit\n");
    total_profit = table[n][W];
    i = ks->size;
    temp = ks->knapsack_capacity;
    while(i > 0){
      if( table[i][temp] != table[i-1][temp]){
        i = i -1;
        total_weight += ks->weights[i];
        temp = temp - ks->weights[i];
        printf("%d\t%d\t%d\n", i, ks->weights[i], ks->profits[i]);
      }else{
        i = i -1;
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
  printf("Knapsack Dynamic Programming:\n");
  printf("------------------------ \n");
  perform_dynamic_pragramming(ks);
  free_knap(ks);
}