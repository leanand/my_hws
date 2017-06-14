#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#define INFINITY -99
#define MAX_LINE_LIMIT 100
int getMinDistanceVert(int *distance, int * is_visited, int no_of_nodes){
  int min = INT_MAX, min_index = -1;
  int i;
  for(i = 0; i < no_of_nodes; i ++){
    if(is_visited[i] == 0 && distance[i] <= min){
      min = distance[i];
      min_index = i;
    }
  }
  return min_index;
}

void perform_dijikstra(int **graph, int *distance,int* is_visited, int no_of_nodes, int src_node){
  int i, j;
  int minDist = 0;
  distance[src_node] = 0;
  for(i = 0; i < no_of_nodes - 1; i++){
    minDist = getMinDistanceVert(distance, is_visited, no_of_nodes);
    is_visited[minDist] = 1;
     for(j = 0; j < no_of_nodes; j ++){
      // It should not be visited
      if(is_visited[j] == 0){
        // There should be an edge
        if(graph[minDist][j] > 0){
          if( distance[minDist] != INT_MAX && distance[minDist] + graph[minDist][j] < distance[j]){
            distance[j] = distance[minDist] + graph[minDist][j];
          }
        }
      }
    }
  }
}   

void init_array(int * arr, int size, int init_value){
  int i = 0;
  for(i = 0; i < size; i ++){
    arr[i] = init_value;
  }
}

int ** create_graph(int no_of_nodes, char * input_file){
  FILE * input_fd;
  int ** graph ;
  int i, j;
  char temp[MAX_LINE_LIMIT];
  char *token;
  size_t read_size;

  input_fd = fopen(input_file, "r");
  if(input_fd == NULL){
    printf("Error in opening file\n");
    abort();
  }
  
  graph = malloc(sizeof(int * ) * no_of_nodes);
  for(i = 0; i < no_of_nodes; i ++){
    graph[i] = malloc(sizeof(int) * no_of_nodes);
    if( fgets(temp, MAX_LINE_LIMIT, input_fd) == NULL){
      printf("Error in reading line");
      abort();
    }
    for(j = 0; j < no_of_nodes; j ++){
      if(j == 0){
        token = strtok(temp, ",");
      }else{
        token = strtok(NULL, ",");
      }
      if(token == NULL){
        printf("Error in creating token");
        abort();
      }
      graph[i][j] = atoi(token);
    }
  }

  while(fgets(temp, MAX_LINE_LIMIT, input_fd) != NULL){
    token = strtok(temp, ",");
    for(i = 0; i < no_of_nodes - 1; i ++){
      token = strtok(NULL, ",");
    }
    printf("the lines are %s", token);

  }
  fclose(input_fd);
  return graph;
}

void display_graph(int **graph, int no_of_nodes){
  int i, j;
  printf("Graph of %d nodes\n", no_of_nodes);
  for(i = 0; i < no_of_nodes; i ++){
    for(j = 0; j < no_of_nodes; j ++){
      printf("%d\t", graph[i][j]);
    }
    printf("\n");
  }
}
void free_graph(int ** graph, int no_of_nodes){
  int i;
  for(i = 0; i < no_of_nodes; i ++){
    free(graph[i]);
  }
  free(graph);
}
void print_distance(int *distance, int no_of_nodes, int source_node){
  int i =0;
  printf("\n\nVertex\tDistance from Source:%d\n", source_node);
  printf("---------------------------\n");
  for(i = 0; i < no_of_nodes; i ++){
    if(distance[i] == INT_MAX){
      printf("%d\tINFINITY\n", i);
    }else{
      printf("%d\t%d\n", i, distance[i]);
    }
  }
}
int main(int args, char ** argv){
  char * input_file;
  int no_of_nodes = 5;
  int src_node = 0;
  if(args < 2){
    printf("Error: Invalid Arguments\n");
    printf("Usage: ./Q2_dijikstra <input_file> <no_of_nodes | 5 > <src_node | 0 >");
    abort();
  }
  input_file = argv[1];
  if(args == 4){
    no_of_nodes = atoi(argv[2]);
    src_node = atoi(argv[3]);
  }

  int ** graph; 
  int distance[no_of_nodes];
  int is_visited[no_of_nodes];
  int i;
  graph = create_graph(no_of_nodes, input_file);
  display_graph(graph, no_of_nodes);
  init_array(distance, no_of_nodes, INT_MAX);
  init_array(is_visited, no_of_nodes, 0);
  perform_dijikstra(graph, distance, is_visited, no_of_nodes, src_node);
  free_graph(graph, no_of_nodes);
  print_distance(distance, no_of_nodes, src_node);
  return 0;
}