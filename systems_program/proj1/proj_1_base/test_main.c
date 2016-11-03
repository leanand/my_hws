/*
 * Binghamton CS 451/551 Project "Memory manager".
 * You do not need to turn in this file.	
 */

#include "memory_manager.h"
#include "interposition.h"


int main(int argc, char * argv[])
{
	// Your code
    mem_mngr_init();
    char * test = (char *)malloc(10);
    mem_mngr_print_snapshot();
    mem_mngr_print_snapshot();
    char * test1 = (char *)malloc(10);
    char * test2= (char *)malloc(10);
    char * test3 = (char *)malloc(10);
    char * test4 = (char *)malloc(10);
    char * test5 = (char *)malloc(10);
    char * test6 = (char *)malloc(10);
    char * test7 = (char *)malloc(10);
    char * test17 = (char *)malloc(10);
    free(test);
    
    char * test8 = (char *)malloc(sizeof(char));
    char * test9 = (char *)malloc(sizeof(char));
    char * test10 = (char *)malloc(sizeof(char));
    char * test11 = (char *)malloc(sizeof(char));
    char * test12 = (char *)malloc(sizeof(char));
    char * test13 = (char *)malloc(sizeof(char));
    char * test14 = (char *)malloc(sizeof(char));
    char * test15 = (char *)malloc(sizeof(char));
    char * test16 = (char *)malloc(sizeof(char));
    char * test18 = (char *)malloc(sizeof(char));

    printf("After allocating ===========>\n");
    mem_mngr_print_snapshot();
    free(test2);
    free(test);

    free(test18);
    printf("After deallocating ===========>\n");
    mem_mngr_print_snapshot();
    mem_mngr_leave();
    return 0;
}


