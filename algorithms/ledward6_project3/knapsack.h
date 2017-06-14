#define MAX_BUFFER 200

typedef struct _knapsack{
  int size;
  int *weights;
  int *profits;
  int knapsack_capacity;
} KNAPSACK;

void init_array(int *weights, int size);
void parse_input(char * file_name, KNAPSACK * ks);
void display_knap(KNAPSACK *ks);
void free_knap(KNAPSACK * ks);