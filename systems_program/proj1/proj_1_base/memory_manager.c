/*
 * Binghamton CS 451/551 Project "Memory manager".
 * This file needs to be turned in.	
 */


#include "memory_manager.h"
#include <string.h>

static STRU_MEM_LIST * mem_pool = NULL;

/*
 * Print out the current status of the memory manager.
 * Reading this function may help you understand how the memory manager organizes the memory.
 * Do not change the implementation of this function. It will be used to help the grading.
 */
void mem_mngr_print_snapshot(void)
{
    STRU_MEM_LIST * mem_list = NULL;

    printf("============== Memory snapshot ===============\n");

    mem_list = mem_pool; // Get the first memory list
    while(NULL != mem_list)
    {
        STRU_MEM_BATCH * mem_batch = mem_list->first_batch; // Get the first mem batch from the list 

        printf("mem_list %p slot_size %d batch_count %d free_slot_bitmap %p\n", 
                   mem_list, mem_list->slot_size, mem_list->batch_count, mem_list->free_slots_bitmap);
        bitmap_print_bitmap(mem_list->free_slots_bitmap, mem_list->bitmap_size);

        while (NULL != mem_batch)
        {
            printf("\t mem_batch %p batch_mem %p\n", mem_batch, mem_batch->batch_mem);
            mem_batch = mem_batch->next_batch; // get next mem batch
        }

        mem_list = mem_list->next_list;
    }

    printf("==============================================\n");
}

/*
 * Initialize the memory manager.
 * You may add your code related to initialization here if there is any.
 */
void mem_mngr_init(void)
{
    mem_pool = create_new_memory_list(MEM_ALIGNMENT_BOUNDARY);
    create_new_memory_batch(mem_pool);
    create_new_memory_batch(mem_pool);
    mem_mngr_print_snapshot();
}

/*
 * Clean up the memory manager (e.g., release all the memory allocated)
 */
void mem_mngr_leave(void)
{
    /* Add your code here */
}

/*
 * Allocate a chunk of memory   
 */
void * mem_mngr_alloc(size_t size)
{
    /* Add you code here */
    void * test;
    return test;
}

/*
 * Free a chunk of memory pointed by ptr
 */
void mem_mngr_free(void * ptr)
{
    /* Add your code here */
}

STRU_MEM_BATCH *create_new_memory_batch(STRU_MEM_LIST * mem_list){
    printf("Initializing Memory Batch with slot_size = %d\n",mem_list->slot_size);
    unsigned char full_bit = 0xFF;
    STRU_MEM_BATCH * mem_batch = (STRU_MEM_BATCH *)malloc(sizeof(STRU_MEM_BATCH));
    mem_batch->batch_mem = malloc(mem_list->slot_size * MEM_BATCH_SLOT_COUNT);
    mem_batch->next_batch = NULL;

    if(NULL == mem_list->first_batch){
        mem_list->first_batch = mem_batch;
    }else{
        STRU_MEM_BATCH *last_mem_batch = get_last_memory_batch(mem_list->first_batch);
        last_mem_batch->next_batch = mem_batch;
    }
    increment_free_slots_bitmap(mem_list);
    mem_list->batch_count ++;

    bitmap_print_bitmap(mem_list->free_slots_bitmap, mem_list->batch_count);
    return mem_batch;

}

void increment_free_slots_bitmap(STRU_MEM_LIST *mem_list){
    if(NULL == mem_list->free_slots_bitmap){
        mem_list->free_slots_bitmap = (unsigned char *)malloc(sizeof(unsigned char *));
        *(mem_list->free_slots_bitmap) = full_bit;
    }else{
        size_t free_slots_size = mem_list->batch_count;
        unsigned char * free_slots_bitmap = (unsigned char *)malloc(free_slots_size + sizeof(full_bit));
        memcpy(free_slots_bitmap, mem_list->free_slots_bitmap, free_slots_size);
        *(mem_list->free_slots_bitmap+free_slots_size) = full_bit;
    }   
    mem_list->bitmap_size ++;
}

STRU_MEM_BATCH *get_last_memory_batch(STRU_MEM_BATCH * mem_batch){
    if(NULL == mem_batch){
        printf("Error: Passed empty mem_batch is passes to get_last_memory_batch \n");
        return NULL;
    }else{
        STRU_MEM_BATCH *current_mem_batch = mem_batch;
        while(current_mem_batch->next_batch != NULL){
            current_mem_batch = current_mem_batch->next_batch;
        }
        return current_mem_batch;
    }
}

STRU_MEM_LIST *create_new_memory_list(int slot_size){
    printf("Initializing Memory list with slot_size = %d \n", slot_size);
    STRU_MEM_LIST * mem_list = (STRU_MEM_LIST *)malloc(sizeof(STRU_MEM_LIST));   
    mem_list->slot_size = slot_size;
    mem_list->batch_count = 0;
    mem_list->bitmap_size = 0;
    mem_list->free_slots_bitmap = NULL;
    mem_list->first_batch=NULL;
    mem_list->next_list = NULL;
    create_new_memory_batch(mem_list);
    return mem_list;
}
