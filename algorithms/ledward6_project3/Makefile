all: brute_force_approach greedy_algorithm dynamic_programming dijikstra

brute_force_approach: Q1-1_brute_force_knapsack.c knapsack.c
	gcc -lm Q1-1_brute_force_knapsack.c knapsack.c -o Q1-1_brute_force_knapsack 

greedy_algorithm: Q1-2_greedy_algorithm.c knapsack.c
	gcc -lm  Q1-2_greedy_algorithm.c knapsack.c -o Q1-2_greedy_algorithm

dynamic_programming: Q1-3_dynamic_programming_knapsack.c
	gcc -lm  Q1-3_dynamic_programming_knapsack.c knapsack.c -o Q1-3_dynamic_programming_knapsack	

dijikstra: Q2_dijikstra.c
	gcc -lm  Q2_dijikstra.c -o Q2_dijikstra

clean:
	rm -rf Q1-1_brute_force_knapsack Q1-2_greedy_algorithm Q1-3_dynamic_programming_knapsack Q2_dijikstra