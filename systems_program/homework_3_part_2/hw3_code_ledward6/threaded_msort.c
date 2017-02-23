#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <pthread.h>
#include <string.h>
#include "common.h"

int str_is_decimal_num(char * str);
int is_regular_file(char * file_path);
int number_of_lines(FILE * fp);
int cmpfunc(const void *, const void *);
typedef struct{
    int *data;
    int size;
    int init_index;
} SPLIT_ARRAY;

typedef struct{
    SPLIT_ARRAY sp_array;
    size_t size;
    int (*compar)(const void *, const void*);
} QSORT_ARGS;

typedef struct{
    SPLIT_ARRAY sp_array1;
    SPLIT_ARRAY sp_array2;
} MSORT_ARGS;


void* quick_sort( void * args){
    QSORT_ARGS * qsort_args = args;
    printf("qsort thread: start element idx %d; sorting index: %d \n", qsort_args->sp_array.init_index, qsort_args->sp_array.size);
    qsort(qsort_args->sp_array.data, qsort_args->sp_array.size, sizeof(int), qsort_args->compar);
    free(qsort_args);
    return ((void *)0);
}

void* merge_sort(void * args){
    MSORT_ARGS *msort_args = args;
    SPLIT_ARRAY sp_array1 = msort_args->sp_array1;
    SPLIT_ARRAY sp_array2 = msort_args->sp_array2;
    int final_size = sp_array1.size + sp_array2.size;
    SPLIT_ARRAY * result = malloc(sizeof(SPLIT_ARRAY));
    result->data = malloc(sizeof(int) * final_size);
    result->size = final_size;
    result->init_index = sp_array1.init_index;
    int i = 0, index1 = 0, index2 = 0;
    printf("merge thread: start element idx = %d; 1st split size: %d; 2nd split size: %d\n", result->init_index, sp_array1.size, sp_array2.size);
    for(i =0; i < final_size; i++){
        if(index1 < sp_array1.size && sp_array1.data[index1] < sp_array2.data[index2]){
            result->data[i] = sp_array1.data[index1];
            index1 ++;
        }else if(index2 < sp_array2.size && sp_array1.data[index1] >= sp_array2.data[index2]){
            result->data[i] = sp_array2.data[index2];
            index2 ++;
        }else if(index1 < sp_array1.size){
            result->data[i] = sp_array1.data[index1];
            index1++;
        }else if(index2 < sp_array2.size){
            result->data[i] = sp_array2.data[index2];
            index2++;
        }
    }
    return result;
}

SPLIT_ARRAY * compute_qsort(int *start, int data_size, int split_num){
    printf("----- qsort round: %d threads needed -----\n", split_num);
    int split_elements_size = floor(data_size/split_num);
    int last_element_size = data_size - ( split_num - 1 ) * split_elements_size;
    SPLIT_ARRAY *sp_array = malloc(sizeof(SPLIT_ARRAY) * split_num);
    int i, j, k = 0;
    pthread_t *pthreads = malloc(sizeof(pthread_t) * split_num);
    QSORT_ARGS *qsort_args;
    for(i = 0; i < split_num ; i ++){
        qsort_args = malloc(sizeof(QSORT_ARGS));

        sp_array[i].size = (i == split_num - 1) ? last_element_size : split_elements_size;
        sp_array[i].data = malloc(sizeof(int) * sp_array[i].size);
        sp_array[i].init_index = k;
        for(j = 0; j < sp_array[i].size; j ++,k++){
            sp_array[i].data[j] = start[k]; 
        }
        
        qsort_args->sp_array = sp_array[i];
        qsort_args->size = sizeof(int);
        qsort_args->compar = cmpfunc;
        int status = pthread_create(&pthreads[i], NULL, quick_sort, qsort_args);
        if(status != 0){
            printf("Error in creating thread ==> %s\n",strerror(status));
            abort();
        }
    }

    for(i =0; i < split_num; i ++){
        int status = pthread_join(pthreads[i], NULL);
        if(status != 0){
            printf("Cannot join thread==> %s\n",strerror(status));
            abort();
        }
        
    }
    if(pthreads){
        free(pthreads);
    }
    return sp_array;
}

int * compute_msort(SPLIT_ARRAY* input_array, int input_size){
    int i, j;
    SPLIT_ARRAY* output_array = NULL;
    int no_of_threads = floor(input_size / 2);
    int total_size = (input_size % 2 == 1) ? no_of_threads + 1 : no_of_threads; 
    pthread_t *pthreads = malloc(sizeof(pthread_t) * no_of_threads);
    while(no_of_threads > 0){
        printf("----- merge round: %d threads needed -----\n", no_of_threads);
        output_array = malloc(sizeof(SPLIT_ARRAY) * total_size);
        SPLIT_ARRAY *temp ;
        for(i = 0 ; i < no_of_threads; i ++ ){
            MSORT_ARGS *msort_args = malloc(sizeof(MSORT_ARGS));
            msort_args->sp_array1 = input_array[2 * i];
            msort_args->sp_array2 = input_array[2 * i+1];
            int status = pthread_create(&pthreads[i], NULL, merge_sort, msort_args);
            if(status !=0){
                printf("Error in creating thread ==> %s\n",strerror(status));
                abort();
            }
        }
        for(j =0; j< no_of_threads; j ++){
            void *returnVal;
            int status = pthread_join(pthreads[j], &returnVal);
            if(status != 0){
                printf("Cannot join thread==> %s\n",strerror(status));
                abort();
            }
            temp = returnVal;
            output_array[j] = *temp;
        }
        if(total_size > no_of_threads){
            output_array[i] = input_array[2 * i];
        }
        input_array = output_array;
        no_of_threads = floor(total_size / 2);
        total_size = (total_size % 2 == 1) ? no_of_threads + 1 : no_of_threads; 
    }
    return input_array[0].data;
};
int main(int argc, char * argv[])
{
    char *idata_fp = NULL, *line = NULL;
    FILE *idata_fs = NULL;
    size_t len;
    unsigned int data_size = 0;
    int split_num = 0, i = 0;
    int *data = NULL;

    /* --- handle user inputs --- */
    if (argc < 3) {
        printf("Usage: %s input_data_file split_num\n", argv[0]);
        exit(1);
    }

    if (!is_regular_file(argv[1])) {
        printf("Regular input data file %s does not exist.\n", argv[1]);
        exit(1);
    }

    if (!str_is_decimal_num(argv[2])) {
        printf("%s is not a valid split size. It should be a decimal number. \n", argv[2]);
        exit(1);
    }

    idata_fp = argv[1];
    split_num = atoi(argv[2]);

    /* --- open the input data file --- */
    idata_fs = fopen(idata_fp, "r");
    if (NULL == idata_fs) {
        EXIT_ERROR(ERROR, "fopen failed!\n");
    }

    /* --- get the number of input data, and allocate data buffer accordingly ---*/
    data_size = number_of_lines(idata_fs);
    if (data_size < split_num)
    {
        printf("data_size(%d) is smaller than split_num(%d)!\n", data_size, split_num);
        exit(1);
    }

    /* --- allocate mem for the input data --- */
    data = malloc(data_size * sizeof(*data));
    if (NULL == data) {
        EXIT_ERROR(ERROR, "malloc failed!\n");
    }

    /* --- read the numbers to sort from the input file to the data buffer ---*/
    i = 0;
    while (getline(&line, &len, idata_fs) != -1) {
        data[i] = atoi(line);
        i++;
    }

    /* --- print the input data ---*/
    printf("========== input data (data_size=%d, split_num=%d) ==========\n", data_size, split_num);
    for (i = 0; i < data_size; i++) {
        printf("%d ", data[i]);
    }
    printf("\n");

    SPLIT_ARRAY* qck_sorts = compute_qsort(data, data_size, split_num);
    data = compute_msort(qck_sorts, split_num);

   

    /* --- print the result ---*/
    fprintf(stderr, "========== threaded merge sort result ==========\n");
    for (i = 0; i < data_size; i++) {
        printf("%d ", data[i]);
    }
    printf("\n");


    /* --- free resourses ---*/
    free(qck_sorts);
    if (line) {
        free(line);
        line = NULL;
    }

    if (data) {
        free (data);
        data = NULL;
    }
    
    fclose(idata_fs);
    exit(0);
}




