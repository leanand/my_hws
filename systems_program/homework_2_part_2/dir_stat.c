#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
typedef struct  {
  int regular_files;
  int directories;
  int sym_links;
} COUNTER;

char * concatWithSlash(char* path, char* directory){
  char * new_directory = (char *) calloc(1, strlen(path) + strlen(directory) + 2);
  strcpy(new_directory, path);
  strncat(new_directory, "/", 1);
  strcat(new_directory, directory);
  return new_directory;
}

void explore_directory(char * directory, COUNTER * counter){
  DIR * targetDir;
  struct dirent *entry;
  if((targetDir = opendir(directory)) != NULL){
    while((entry = readdir(targetDir)) != NULL){
      struct stat fileStat;
      char *full_path = concatWithSlash(directory, entry->d_name);
      if(lstat(full_path, &fileStat) < 0){
        printf("Error in stat calculation\n");
        return ;
      }
      // printf("Stats value %d \n", stat(full_path, &fileStat));
      // printf("file_name  %s SYM_link ===> %d, DIR ===> %d, REG_FILE ===> %d\n", entry->d_name, S_ISLNK(fileStat.st_mode),S_ISDIR(fileStat.st_mode),S_ISREG(fileStat.st_mode));
      if(S_ISLNK(fileStat.st_mode)){
        counter->sym_links ++;
      }else{
        if(S_ISDIR(fileStat.st_mode)){
          if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..")!=0){
            counter->directories ++;
            // printf("Directory name ===> %s\n", full_path);
            explore_directory(full_path, counter);
            free(full_path);
          }
        }else if(S_ISREG(fileStat.st_mode)){
          counter->regular_files ++;
        }
      }
    }
    // printf ("Director stat %s\n", directory);
  }
}

int main(int argv, char* args[]){
  if(argv != 2){
    printf("Illegal Arguments\n");
    return 1;
  }
  char *directory = args[1];
  COUNTER * counter =(COUNTER *)malloc(sizeof(COUNTER));
  counter->regular_files = 0;
  counter->directories = 0;
  counter->sym_links = 0;
  

  explore_directory(directory, counter);
  printf("Regular Files: %d\n", counter->regular_files);
  printf("Directories: %d\n", counter->directories);
  printf("Sym-links: %d\n", counter->sym_links);

  return 0;
}