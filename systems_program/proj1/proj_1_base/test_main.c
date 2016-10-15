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
    // unsigned char  * tes= (unsigned char *)malloc(sizeof(unsigned char *));
    // * tes = 0xff;
    // unsigned char  * test= (unsigned char *)malloc(sizeof(tes) + sizeof(unsigned char *) );

    // printf("sizeee === %lu", sizeof(*tes));
    // printf("testtttttt === %lu", sizeof(test));
   /* int check;
    unsigned char bitmap[] = {0xff,0xff} ;
    printf("Sizeeee ::::%lu\n", sizeof(bitmap));
    bitmap_print_bitmap(bitmap, sizeof(bitmap));
    check = bitmap_find_first_bit(bitmap, sizeof(bitmap), 0);
    bitmap_set_bit(bitmap, sizeof(bitmap), 1);
    printf("BIt position %d\n",check);
    bitmap_print_bitmap(bitmap, sizeof(bitmap));
    printf("After unsetting\n");
    bitmap_clear_bit(bitmap, sizeof(bitmap), 10);
    bitmap_print_bitmap(bitmap, sizeof(bitmap));
    int n = bitmap_bit_is_set(bitmap, sizeof(bitmap), 10);

    printf("asdasdas ==== > %d\n", n);*/

    // unsigned char bit = 0xf6; // | 1 << 3;
    // bitmap_print_bitmap(bitmap, sizeof(bit));



	// test your code here.


    // mem_mngr_leave();

    return 0;
}


