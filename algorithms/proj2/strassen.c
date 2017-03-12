#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

typedef struct _matrix {
  float ** matrix;
  int size;
} Matrix;

float _get_random(float start, float range){
  return drand48() * (range - start) + start;
}

Matrix * create_matrix(int size){
  int i, j;
  Matrix * tempMatrix = malloc(sizeof(Matrix *));
  tempMatrix->size = size;
  tempMatrix->matrix = malloc(sizeof(float *) * size);

  for(i = 0; i < size; i ++){
    tempMatrix->matrix[i] = malloc(sizeof(float) * size);
    for(j = 0; j < size; j ++){
        tempMatrix->matrix[i][j] = 0.0;
    }
  }
  return tempMatrix;
}
void fill_random_matrix(Matrix * matrix){
  int i, j;
  for(i = 0; i < matrix->size; i++){
    for(j = 0; j < matrix->size; j++){
      matrix->matrix[i][j] = _get_random(-5.0, 5.0);
    }
  }
}

void display_matrix(Matrix * matrix, char * title){
  int i, j;
  printf("%s\n", title);
  for(i = 0; i < matrix->size; i++){
    for(j = 0; j < matrix->size; j++){
      printf("%10.5f\t", matrix->matrix[i][j]);
    }
    printf("\n");
  }
}

void free_matrix(Matrix * matrix){
  int i;
  if(matrix == NULL){
    return;
  }
  for(i = 0; i < matrix->size; i ++){
    free(matrix->matrix[i]);
  }
  free(matrix);
}

void do_standard_multiplication(Matrix * matrixA, Matrix * matrixB){
  int i, j, k, size = matrixA->size;
  float temp;
  Matrix * matrixC = create_matrix(size);
  for(i = 0; i < size; i ++){
    for(j = 0; j < size; j ++){
      temp = 0.0;
      for(k = 0; k < size; k ++){
        temp = temp + matrixA->matrix[i][k] * matrixB->matrix[k][j];
      }
      matrixC->matrix[i][j] = temp;
    }
  }
  display_matrix(matrixC, "Standard Multiplication:");
  free_matrix(matrixC);
}


Matrix * _add_matrix(Matrix * matrixA, Matrix * matrixB){
  int i, j;
  Matrix * matrixC = create_matrix(matrixA->size);
  for(i = 0; i < matrixA->size; i ++){
    for(j = 0; j < matrixB->size; j ++){
      matrixC->matrix[i][j] = matrixA->matrix[i][j] + matrixB->matrix[i][j];
    }
  }
  return matrixC;
};

Matrix * _subtract_matrix(Matrix * matrixA, Matrix * matrixB){
  int i, j;
  Matrix * matrixC = create_matrix(matrixA->size);
  for(i = 0; i < matrixA->size; i ++){
    for(j = 0; j < matrixB->size; j ++){
      matrixC->matrix[i][j] = matrixA->matrix[i][j] - matrixB->matrix[i][j];
    }
  }
  return matrixC;
};

Matrix * _split_matrix(Matrix * matrixA, int portion){
  int required_size = matrixA->size / 2;
  Matrix * splitMatrix = create_matrix(required_size);
  int i, j, start, end;
  for(i = 0; i < required_size; i ++){
    for(j = 0; j < required_size; j ++){
      switch(portion){
        case 1:
          splitMatrix->matrix[i][j] = matrixA->matrix[i][j];
          break;
        case 2:
          splitMatrix->matrix[i][j] = matrixA->matrix[i][j + required_size];
          break;
        case 3:
          splitMatrix->matrix[i][j] = matrixA->matrix[i + required_size][j];
          break;
        case 4:
          splitMatrix->matrix[i][j] = matrixA->matrix[i + required_size][j + required_size];
          break;
      }
    }
  }
  return splitMatrix;
}

Matrix * _join_matrix(Matrix * matrixA, Matrix * matrixB, Matrix * matrixC, Matrix * matrixD){
  int initial_size = matrixA->size;
  int required_size = initial_size * 2;
  Matrix * mergedMatrix = create_matrix(required_size);
  int i,j;
  for(i = 0; i < required_size; i ++){
    for(j = 0; j < required_size; j ++){
      if(i < initial_size && j < initial_size){
        mergedMatrix->matrix[i][j] = matrixA->matrix[i][j];
      }else if(i < initial_size && j >= initial_size){
        mergedMatrix->matrix[i][j] = matrixB->matrix[i][j - initial_size];
      }else if(i >= initial_size && j < initial_size){
        mergedMatrix->matrix[i][j] = matrixC->matrix[i - initial_size][j];
      }else if(i >= initial_size && j >= initial_size){
        mergedMatrix->matrix[i][j] = matrixD->matrix[i - initial_size][j - initial_size];
      }
    }
  }
  free_matrix(matrixA);
  free_matrix(matrixB);
  free_matrix(matrixC);
  free_matrix(matrixD);
  return mergedMatrix;
}

Matrix * _extend_matrix(Matrix * matrixA, int new_size){
  int old_size = matrixA->size;
  int i, j;
  Matrix * newMatrix = create_matrix(new_size);
  for(i = 0; i < new_size; i ++){
    for(j = 0; j < new_size; j ++){
      if(i < old_size && j < old_size){
        newMatrix->matrix[i][j] = matrixA->matrix[i][j];
      }else{
        newMatrix->matrix[i][j] = 0.0;
      }
    }
  }
  return newMatrix;
}

Matrix * _do_actual_strassen(Matrix * matrixA, Matrix * matrixB){
  float m1, m2, m3, m4, m5, m6, m7;
  if(matrixA->size != 2){
    fprintf(stderr, "Error: Size greater than 2 while performing actual strassen : %d\n", matrixA->size);
    return NULL;
  }
  Matrix * matrixC = create_matrix(matrixA->size);
  float ** _matrixA = matrixA->matrix;
  float ** _matrixB = matrixB->matrix;
  m1 = (_matrixA[0][0] + _matrixA[1][1]) * (_matrixB[0][0] + _matrixB[1][1]);
  m2 = (_matrixA[1][0] + _matrixA[1][1]) * _matrixB[0][0];
  m3 = _matrixA[0][0] * (_matrixB[0][1] - _matrixB[1][1]);
  m4 = _matrixA[1][1] * (_matrixB[1][0] - _matrixB[0][0]);
  m5 = (_matrixA[0][0] + _matrixA[0][1]) * _matrixB[1][1];
  m6 = (_matrixA[1][0] - _matrixA[0][0]) * (_matrixB[0][0] + _matrixB[0][1]);
  m7 = (_matrixA[0][1] - _matrixA[1][1]) * (_matrixB[1][0] + _matrixB[1][1]);

  matrixC->matrix[0][0] = m1 + m4 - m5 + m7;
  matrixC->matrix[0][1] = m3 + m5;
  matrixC->matrix[1][0] = m2 + m4;
  matrixC->matrix[1][1] = m1 - m2 + m3 + m6;
  return matrixC;
}

Matrix * _do_recur_strassen(Matrix * matrixA, Matrix * matrixB){
  if(matrixA->size < 2){
    fprintf(stderr, "Error: Size should be greater than 2 while performing Strassen: %d\n", matrixA->size);
    return NULL;
  }
  if(matrixA->size == 2){
    return _do_actual_strassen(matrixA, matrixB);
  }else{
    Matrix * matrix_A, * matrix_B, * matrix_C, * matrix_D, * matrix_E, * matrix_F, * matrix_G, * matrix_H;
    Matrix * matrixP1, * matrixP2, * matrixP3, * matrixP4, * matrixP5, * matrixP6, * matrixP7;
    Matrix * merge_1, * merge_2, * merge_3, * merge_4;
    Matrix * matrixTemp1, * matrixTemp2;
    
    matrix_A = _split_matrix(matrixA, 1);
    matrix_B = _split_matrix(matrixA, 2);
    matrix_C = _split_matrix(matrixA, 3);
    matrix_D = _split_matrix(matrixA, 4);
    
    matrix_E = _split_matrix(matrixB, 1);
    matrix_F = _split_matrix(matrixB, 2);
    matrix_G = _split_matrix(matrixB, 3);
    matrix_H = _split_matrix(matrixB, 4);

    /* P1 = A(F - H) */
    matrixTemp1 = _subtract_matrix(matrix_F, matrix_H);
    matrixP1 = _do_recur_strassen(matrix_A, matrixTemp1);
    free_matrix(matrixTemp1);

    /* P2 = (A + B)H */
    matrixTemp1 = _add_matrix(matrix_A, matrix_B);
    matrixP2 = _do_recur_strassen(matrixTemp1, matrix_H);
    free_matrix(matrixTemp1);

    /* P3 = (C + D)E */
    matrixTemp1 = _add_matrix(matrix_C, matrix_D);
    matrixP3 = _do_recur_strassen(matrixTemp1, matrix_E);
    free_matrix(matrixTemp1);

    /* P4 = D(G - E) */
    matrixTemp1 = _subtract_matrix(matrix_G, matrix_E);
    matrixP4 = _do_recur_strassen(matrix_D, matrixTemp1); 
    free_matrix(matrixTemp1);   

    /* P5 = (A + D)(E + H) */
    matrixTemp1 = _add_matrix(matrix_A, matrix_D);
    matrixTemp2 = _add_matrix(matrix_E, matrix_H);
    matrixP5 = _do_recur_strassen(matrixTemp1, matrixTemp2);        
    free_matrix(matrixTemp1);
    free_matrix(matrixTemp2);

    /* P6 = (B - D)(G + H) */
    matrixTemp1 = _subtract_matrix(matrix_B, matrix_D);
    matrixTemp2 = _add_matrix(matrix_G, matrix_H);
    matrixP6 = _do_recur_strassen(matrixTemp1, matrixTemp2);
    free_matrix(matrixTemp1);
    free_matrix(matrixTemp2);            

    /* P7 = (A - C)(E + F) */
    matrixTemp1 = _subtract_matrix(matrix_A, matrix_C);
    matrixTemp2 = _add_matrix(matrix_E, matrix_F);
    matrixP7 = _do_recur_strassen(matrixTemp1, matrixTemp2); 
    free_matrix(matrixTemp1);
    free_matrix(matrixTemp2);                       
    free_matrix(matrix_A);
    free_matrix(matrix_B);
    free_matrix(matrix_C);
    free_matrix(matrix_D);
    free_matrix(matrix_E);
    free_matrix(matrix_F);
    free_matrix(matrix_G);
    free_matrix(matrix_H);

    /* P5 + P4 - P2 + P6*/
    matrixTemp1 = _add_matrix(matrixP5, matrixP4);
    matrixTemp2 = _add_matrix(matrixTemp1, matrixP6);
    merge_1 = _subtract_matrix(matrixTemp2, matrixP2);
    free_matrix(matrixTemp1);
    free_matrix(matrixTemp2); 

    /* P1 + P2 */
    merge_2 = _add_matrix(matrixP1, matrixP2);

    /* P3 + P4 */
    merge_3 = _add_matrix(matrixP3, matrixP4);

    /* P1 + P5 - P3 - P7 */
    matrixTemp1 = _add_matrix(matrixP1, matrixP5);
    matrixTemp2 = _subtract_matrix(matrixTemp1, matrixP3);
    merge_4 = _subtract_matrix(matrixTemp2, matrixP7);   
    free_matrix(matrixTemp1);
    free_matrix(matrixTemp2); 
    
    
    return _join_matrix(merge_1, merge_2, merge_3, merge_4);
  }
}

void do_strassen_multiplication(Matrix * matrixA, Matrix * matrixB){
  int matrixSize = matrixA->size;
  int newSize = 0;
  int isPowerOf2 = (matrixSize > 0 && ((matrixSize & (matrixSize - 1)) == 0));
  Matrix * matrixC, * matrixAnew, * matrixBnew, * matrixCnew ;
  if(isPowerOf2 != 1 || matrixSize < 2){
    newSize = (matrixSize < 2)? 2 : pow(2, ceil(log(matrixSize) / log(2)));
    matrixAnew = _extend_matrix(matrixA, newSize);
    matrixBnew = _extend_matrix(matrixB, newSize);
    matrixCnew = _do_recur_strassen(matrixAnew, matrixBnew);
    matrixC = _extend_matrix(matrixCnew, matrixSize);
    free_matrix(matrixCnew);
  }else{
    matrixC = _do_recur_strassen(matrixA, matrixB);
  }
  display_matrix(matrixC, "Strassen Multiplication: ");
  free_matrix(matrixC);    
}

int main(int argv, char **argc){
  srand48(time(NULL));
  int n_value;
  Matrix * matrixA = NULL, * matrixB = NULL;

  if(argv != 2){
    fprintf(stderr, "Invalid arguments!\nUsage:\n\t./strassen.out <value_of_n>\n");
    return -1;
  }
  n_value = atoi(argc[1]);
  printf("The value of N = %d\n", n_value);
  matrixA = create_matrix(n_value);
  fill_random_matrix(matrixA);
  matrixB = create_matrix(n_value);
  fill_random_matrix(matrixB);
  display_matrix(matrixA, "Matrix A:");
  display_matrix(matrixB, "Matrix B:");
  do_standard_multiplication(matrixA, matrixB);
  do_strassen_multiplication(matrixA, matrixB);
  free_matrix(matrixA);
  free_matrix(matrixB);
  return 1;
}