#include <stdio.h>

typedef struct _point{
  float x;
  float y;
} Point;

Point GA = {
  .x = 10.0,
  .y = 10.0
}, GB = {
  .x = 10.0,
  .y = 10.0
}, GC = {
  .x = 10.0,
  .y = 10.0
};

typedef struct _distance{
  float da;
  float db;
  float dc;
} Distance;

void generate_datasets(Distance * distances, int size){

}
int main(int args, char ** argv){
  if(args != 2){
    fprintf(stderr, "Invalid Arguments! " );
    return -1;
  }
  int no_of_datasets = atoi(argv[1]);
  Distance distances[no_of_datasets];
  generate_datasets()
  printf("Hello world\n");
  return -1;
}