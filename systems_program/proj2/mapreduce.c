#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include "mapreduce.h"
#include "common.h"
#include <ctype.h>

int start_offset = 0;
void initialize_data_split(int fd, int file_size, int split_num, DATA_SPLIT *data_splits, char *file_name){
    char byte;
    int read_ret;
    for(int i = 0; i < split_num; i ++){
        data_splits[i].fd = open(file_name, O_RDONLY);
        int split_size = file_size/split_num;
        
        if(i == split_num - 1){
            split_size = file_size - start_offset;
        }else{
            lseek(data_splits[i].fd, start_offset + split_size, SEEK_SET);
            while(1){
                read_ret = read(data_splits[i].fd, &byte, 1);
                // printf("Letter at position %d ===> %c\n", start_offset+split_size, byte);
                if(read_ret != 1){
                    printf("Cannot separate file into %d parts\n", split_num);
                    exit(1);
                }
                if(byte =='\n'){
                    break;
                }
                split_size++;
            }    
        }
        data_splits[i].size = split_size;
        printf("Debug: Splitting as %d part with size %d\n",i, split_size);
        // printf("split_size ====> %d ===> start_offset ===> %dfile_size ===> %d\n", split_size, start_offset, file_size);
        lseek(data_splits[i].fd, start_offset, SEEK_SET);
        start_offset += split_size ;
    }
};

void mapreduce(MAPREDUCE_SPEC * spec, MAPREDUCE_RESULT * result)
{
    
    struct timeval start, end;
    int file_size;
    int input_fd;
    int output_fd;
    DATA_SPLIT * data_splits= (DATA_SPLIT *)malloc(sizeof(DATA_SPLIT) * spec->split_num);
    int *inter_fds = (int *)malloc(sizeof(int) * spec->split_num);
    char file_name[15];
    if (NULL == spec || NULL == result)
    {
        EXIT_ERROR(ERROR, "NULL pointer!\n");
    }
    
    gettimeofday(&start, NULL);
    input_fd = open(spec->input_data_filepath, O_RDONLY);
    output_fd = open(result->filepath, O_WRONLY | O_CREAT | O_TRUNC, 0660);
    file_size = lseek(input_fd, 0L, SEEK_END);
    printf("Debug : File Size ===> %d\n",file_size);
    printf("Debug : Split Size===> %d\n",spec->split_num);
    lseek(input_fd, 0L, SEEK_SET);
    initialize_data_split(input_fd, file_size, spec->split_num, data_splits, spec->input_data_filepath);
    // printf("File_size %d\n", file_size);
    for(int i=0; i < spec->split_num; i ++){
        sprintf(file_name,"mr-%d.itm",i);
        inter_fds[i] = open(file_name, O_RDWR | O_CREAT | O_TRUNC, 0660);
        data_splits[i].usr_data = spec->usr_data;
        printf("Debug: Applying map function to part %d \n",i);
        spec->map_func(&data_splits[i], inter_fds[i]);
    }
    spec->reduce_func(inter_fds, spec->split_num, output_fd);
    gettimeofday(&end, NULL);   
    close(input_fd);
    result->processing_time = (end.tv_sec - start.tv_sec) * US_PER_SEC + (end.tv_usec - start.tv_usec);
}
