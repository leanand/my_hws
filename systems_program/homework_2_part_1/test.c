#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
int main (void) {
    int fd = open("output.txt", O_RDWR|O_CREAT);
    write(fd, "123", 3);
    close(fd);
    return 0;
}