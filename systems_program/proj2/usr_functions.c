#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include "common.h"
#include "usr_functions.h"

/* User-defined map function for the "Letter counter" task.  
   This map function is called in a map worker process.
   @param split: The data split that the map function is going to work on.
                 Note that the file offset of the file descripter split->fd should be set to the properly
                 position when this map function is called.
   @param fd_out: The file descriptor of the itermediate data file output by the map function.
   @ret: 0 on success, -1 on error.
 */
int letter_counter_map(DATA_SPLIT * split, int fd_out)
{
    int letter_array[26];
    int count = split->size;
    char buf;
    int ascii_val;
    // printf("count ===> %d\n", split->size);
    for(int i =0; i < 26; i ++){
      letter_array[i]= 0;
    }
    while(count > -1 && read(split->fd, &buf, 1) != 0){
      ascii_val = (int)buf;
      // A <-> Z
      if(ascii_val > 64 && ascii_val < 91){
        ascii_val -=65;
        letter_array[ascii_val] += 1;
      }
      // a <-> z
      if(ascii_val > 96 && ascii_val < 123){
        ascii_val -=97;
        letter_array[ascii_val] += 1; 
      }
      count --;
    }
    for(int i =0; i < 26; i ++){
      // printf("%c %d\n", i+65, letter_array[i]);
      dprintf(fd_out, "%c %d\n", i+65, letter_array[i]);
    }
    return 0;
}

/* User-defined reduce function for the "Letter counter" task.  
   This reduce function is called in a reduce worker process.
   @param p_fd_in: The address of the buffer holding the intermediate data files' file descriptors.
                   The imtermeidate data files are output by the map worker processes, and they
                   are the input for the reduce worker process.
   @param fd_in_num: The number of the intermediate files.
   @param fd_out: The file descriptor of the final result file.
   @ret: 0 on success, -1 on error.
   @example: if fd_in_num == 3, then there are 3 intermediate files, whose file descriptor is 
             identified by p_fd_in[0], p_fd_in[1], and p_fd_in[2] respectively.

*/
int letter_counter_reduce(int * p_fd_in, int fd_in_num, int fd_out)
{
    char occurences[8];
    char buf;
    int count;
    int letter_index = 0;
    int letter_array[26];
    for(int i = 0; i <26; i ++){
      letter_array[i] = 0;
    }
    for(int i =0; i < fd_in_num; i ++){
      lseek(p_fd_in[i], 0L, SEEK_SET);
      letter_index = 0;
      count = 0;
      while(read(p_fd_in[i], &buf, 1) != 0){
        memset(occurences, 0, 8);
        if(isspace(buf) == 1){
          while(read(p_fd_in[i], &buf, 1) != 0){
            if(isspace(buf) == 1){
              break;
            }
            strncat(occurences, &buf, 1);
          }
          count = atoi(occurences);
          letter_array[letter_index] += count;
          // printf("After ==>lettter _ index ===> %d ===> %d ====> %d\n", letter_index, count, letter_array[letter_index]);
          letter_index ++;
        }
      }  
    }
    for(int i =0 ; i < 26; i ++){
      // printf("lettter _ index ===> %d ===> %d\n", i, letter_array[i]);
      dprintf(fd_out, "%c %d\n", i+65, letter_array[i]);
    }
    return 0;
}

/* User-defined map function for the "Word finder" task.  
   This map function is called in a map worker process.
   @param split: The data split that the map function is going to work on.
                 Note that the file offset of the file descripter split->fd should be set to the properly
                 position when this map function is called.
   @param fd_out: The file descriptor of the itermediate data file output by the map function.
   @ret: 0 on success, -1 on error.
 */
int word_finder_map(DATA_SPLIT * split, int fd_out)
{ 
    printf("Debug: word_finder_map\n");
    char buf;
    int count = 0;
    int line_count = 0;
    int word_length = 0;
    int user_data_length = strlen(split->usr_data);
    char check_word[25];
    memset(check_word, '\0', 25);
    int line_added = -1;
    int start_offset = lseek(split->fd, 0L, SEEK_CUR);
    char *check_buf;
    while(count < split->size){
      read(split->fd, &buf, 1);
      // printf("%c",buf);
      if(isspace(buf)){
        if(line_added != line_count && word_length >= user_data_length){
          check_buf = strstr(check_word, split->usr_data);
          if(check_buf != NULL){
            printf("\nDebug: Word match %s at %d : line %d\n ",check_word, count, line_count);
            // printf("Seeking to position ==> %d, split_size ===> %d, count ==> %d", -(split->size - count), split->size, count);
            int current_off = lseek(split->fd, 0L, SEEK_CUR);
            lseek(split->fd, start_offset , SEEK_SET);
            write_line(fd_out, line_count, split->fd, split->size);
            lseek(split->fd, current_off, SEEK_SET);
            line_added = line_count;
          }
        }
        if(buf == '\n'){
          line_count++;
        }
        memset(check_word, '\0', 25);
        word_length = 0;
      }else{
        check_word[word_length] = buf;
        word_length++;
      }
      count ++;
    }
    if(word_length >= user_data_length && line_added != line_count){
      check_buf = strstr(check_word, split->usr_data);
      if(check_buf != NULL ){
        printf("\nDebug: Word match %s at %d : line %d\n ",check_word, count, line_count);
        // printf("Seeking to position ==> %d, split_size ===> %d, count ==> %d", -(split->size - count), split->size, count);
        int current_off = lseek(split->fd, 0L, SEEK_CUR);
        lseek(split->fd, start_offset , SEEK_SET);
        write_line(fd_out, line_count, split->fd, split->size);
        lseek(split->fd, current_off, SEEK_SET);
      }
    }
    printf("\n");
    return 0;
}

/* User-defined reduce function for the "Word finder" task.  
   This reduce function is called in a reduce worker process.
   @param p_fd_in: The address of the buffer holding the intermediate data files' file descriptors.
                   The imtermeidate data files are output by the map worker processes, and they
                   are the input for the reduce worker process.
   @param fd_in_num: The number of the intermediate files.
   @param fd_out: The file descriptor of the final result file.
   @ret: 0 on success, -1 on error.
   @example: if fd_in_num == 3, then there are 3 intermediate files, whose file descriptor is 
             identified by p_fd_in[0], p_fd_in[1], and p_fd_in[2] respectively.

*/
int word_finder_reduce(int * p_fd_in, int fd_in_num, int fd_out)
{
  printf("Debug:%d word_finder_reduce\n" , (int)getpid());
  printf("Debug: Output\n");
  char buf;
  int status = 0;
  lseek(fd_out, 0L, SEEK_SET);
  for(int i=0;i < fd_in_num; i ++){
    lseek(p_fd_in[i], 0L, SEEK_SET);
    while(1){
      status = read(p_fd_in[i], &buf, 1);
      if(status != 1){
        break;
      }
      printf("%c",buf);
      write(fd_out, &buf, 1);
    }
  }
  return 0;
}

int write_line(int fd_out, int line_no, int fd_in, int size){
  int count = size;
  char buf;
  int line_count = 0;
  printf("Debug: Writing line %d into file \n", line_no);
  while(count > 0 && line_count <= line_no){
    read(fd_in, &buf, 1);
    if(buf == '\n'){
      line_count ++;
    }else if(line_no == line_count){
      printf("%c", buf);
      write(fd_out, &buf, 1);
    }
    count --;
  }
  write(fd_out, "\n", 1);
 return 1;
}


