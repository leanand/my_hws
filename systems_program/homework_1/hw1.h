#define printf myprintf

#define MYMSG(format, ...) printf("Original Message ---> %s:%d "format,__FILE__, __LINE__, ##__VA_ARGS__)

int myprintf(char* format, ...);
int str_manip(char * str, char * substr);