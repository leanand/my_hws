#include <stdio.h>
#include <stdlib.h>
#include <math.h>
typedef struct _tromino {
  int ** tiles;
  int size;
  int hole_X;
  int hole_Y;
  int insert_count;
} TROMINO;

void display_tromino(TROMINO * tromino){
  int i, j, color;
  printf("Tromino Tiles: \n\n");
  for(i = 0; i < tromino->size; i ++){
    for(j = 0; j < tromino->size; j++){
      if(tromino->tiles[i][j] == -5){
        printf("\e[0;36m%5s\e[m", "X");
        continue;
      }
      color = 31 + (tromino->tiles[i][j]) % 7 ;
      //printf("\e[0;%dm%5d\e[m",color, tromino->tiles[i][j] );
       printf("\e[0;%dm%5d\e[m",color,tromino->tiles[i][j] );
    }
    printf("\n");
  }
  printf("\n");
}

TROMINO * create_tromino(int size, int hole_X, int hole_Y){
  int i,j;
  TROMINO * tromino;
  if(hole_X >= size|| hole_Y >= size){
    fprintf(stderr, "Error: Hole coordinate is out of bound.\n" );
    exit(-1);
  }
  if( (  tromino = malloc(sizeof (TROMINO ) ) ) == NULL){
    fprintf(stderr, "Error in allocating memory\n");
    abort();
  }
  tromino->size = size;
  tromino->hole_X = hole_X;
  tromino->hole_Y = hole_Y;
  tromino->insert_count = 0;
  if((tromino->tiles = (int **)malloc(sizeof(int *)  * size) ) == NULL){
    fprintf(stderr, "Error in allocating memory\n");
    abort();
  }
  for(i = 0; i < size; i++){
    if((tromino->tiles[i] = malloc(sizeof(int)  * size)) == NULL){
      fprintf(stderr, "Error in allocating memory\n");
      abort();
    }
    for(j = 0; j< size; j ++){
      tromino->tiles[i][j] = (i == hole_X && j == hole_Y) ? -5 : 0;
    }
  }
  return tromino;
}

void do_tromino(TROMINO * tromino,  int start_X, int start_Y, int size, int hole_X, int hole_Y){
  int i, j;
  int end_X, end_Y;
  end_X = start_X + size - 1;
  end_Y = start_Y + size - 1;
  tromino->insert_count ++;
  if(size == 2){
    for(i = start_X; i < start_X + size; i ++){
      for(j = start_Y; j< start_Y + size; j ++){
        if(hole_X == i && hole_Y == j){
          continue;
        }
        tromino->tiles[i][j] = tromino->insert_count;
      }  
    }
  }else{
      int new_size = size / 2 ;
      int count = tromino->insert_count;
      // If the hole is in first split
      if(hole_X >= start_X && hole_X <= start_X + new_size - 1 && hole_Y >=start_Y && hole_Y <= start_Y + new_size - 1){
        do_tromino(tromino, start_X, start_Y, new_size, hole_X, hole_Y);
      }else{
        tromino->tiles[start_X + new_size - 1][start_Y + new_size - 1] = count;
        do_tromino(tromino, start_X, start_Y, new_size, start_X + new_size - 1, start_Y + new_size - 1);
      }
      // If the hole is in second split
      if(hole_X >= start_X && hole_X <= start_X + new_size - 1 && hole_Y >=start_Y + new_size && hole_Y <= end_Y ){
        do_tromino(tromino, start_X, start_Y + new_size, new_size, hole_X, hole_Y);
      }else{
        tromino->tiles[start_X + new_size - 1][start_Y + new_size] = count;
        do_tromino(tromino, start_X, start_Y + new_size, new_size, start_X + new_size - 1,start_Y + new_size);
      }
      // If the hole is in third split
      if(hole_X >= start_X + new_size && hole_X <= end_X && hole_Y >=start_Y  && hole_Y <= start_Y + new_size - 1){
        do_tromino(tromino, start_X + new_size,  start_Y, new_size, hole_X, hole_Y);
      }else{
        tromino->tiles[start_X + new_size ][start_Y + new_size - 1] = count;
        do_tromino(tromino, start_X + new_size,  start_Y, new_size, start_X + new_size, start_Y + new_size - 1);
      }
      // If the hole is in fourth split
      if(hole_X >= start_X + new_size && hole_X <= end_X && hole_Y >=start_Y + new_size && hole_Y <= end_Y ){
        do_tromino(tromino, start_X + new_size,  start_Y + new_size, new_size, hole_X, hole_Y);
      }else{
        tromino->tiles[start_X + new_size ][start_Y + new_size] = count;
        do_tromino(tromino, start_X + new_size,  start_Y + new_size, new_size, start_X + new_size, start_Y + new_size);
      }

  }
}

void trominoTile(TROMINO * tromino){
  do_tromino(tromino, 0, 0, tromino->size, tromino->hole_X, tromino->hole_Y);
}

void free_tromino(TROMINO * tromino){
  int i, j;
  for(i = 0 ; i < tromino->size; i ++){
    free(tromino->tiles[i]);
  }
  free(tromino->tiles);
  free(tromino);
}

int main(int args, char **argv){
  int size, hole_X, hole_Y;
  TROMINO * tromino;
  if(args != 4){
    fprintf(stderr, "Error: Invalid arguments\nUsage: ./tromino <value_of_k> <hole_row_no> <hole_col_no>\n" );
    return -1;
  }
  size = atoi(argv[1]);
  size = pow(2, size);
  hole_X = atoi(argv[2]);
  hole_Y = atoi(argv[3]);
  tromino = create_tromino(size, hole_X, hole_Y);
  trominoTile(tromino);
  display_tromino(tromino);
  free_tromino(tromino);
}