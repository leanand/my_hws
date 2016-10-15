/*
 * Binghamton CS 451/551 Project "Memory manager".
 * You do not need to turn in this file.	
 */

#include "memory_manager.h"
// #include "interposition.h"


int main(int argc, char * argv[])
{
	// Your code
    mem_mngr_init();
    mem_mngr_print_snapshot();
    char * test = (char *)mem_mngr_alloc(10);
    char * test1 = (char *)mem_mngr_alloc(10);
    char * test2= (char *)mem_mngr_alloc(10);
    char * test3 = (char *)mem_mngr_alloc(10);
    char * test4 = (char *)mem_mngr_alloc(10);
    char * test5 = (char *)mem_mngr_alloc(10);
    char * test6 = (char *)mem_mngr_alloc(10);
    char * test7 = (char *)mem_mngr_alloc(10);
    char * test17 = (char *)mem_mngr_alloc(10);
    char * test8 = (char *)mem_mngr_alloc(sizeof(char));
    char * test9 = (char *)mem_mngr_alloc(sizeof(char));
    char * test10 = (char *)mem_mngr_alloc(sizeof(char));
    char * test11 = (char *)mem_mngr_alloc(sizeof(char));
    char * test12 = (char *)mem_mngr_alloc(sizeof(char));
    char * test13 = (char *)mem_mngr_alloc(sizeof(char));
    char * test14 = (char *)mem_mngr_alloc(sizeof(char));
    char * test15 = (char *)mem_mngr_alloc(sizeof(char));
    char * test16 = (char *)mem_mngr_alloc(sizeof(char));
    char * test18 = (char *)mem_mngr_alloc(sizeof(char));

    get_batch_by_address(test18);
    printf("After allocating ===========>\n");
    mem_mngr_print_snapshot();
    mem_mngr_free(test2);
    mem_mngr_free(test);
    mem_mngr_free(test18);
    mem_mngr_free(test18);
    printf("After deallocating ===========>\n");
    mem_mngr_print_snapshot();
    mem_mngr_leave();
    return 0;
}


