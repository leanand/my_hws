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
}

/*
 * Clean up the memory manager (e.g., release all the memory allocated)
 */
void mem_mngr_leave(void)
{
    STRU_MEM_LIST* itr_list = mem_pool;
    while(itr_list != NULL){
        STRU_MEM_BATCH *itr_batch =itr_list->first_batch;
        while(itr_batch != NULL){
            free(itr_batch->batch_mem);
            itr_batch = itr_batch -> next_batch;
        }
        itr_list = itr_list->next_list;
    }
}

/*
 * Allocate a chunk of memory   
 */
void * mem_mngr_alloc(size_t size)
{   
    size_t alligned_size = SLOT_ALLINED_SIZE(size);
    STRU_MEM_LIST *required_list = find_mem_list_by_size(alligned_size);
    if(required_list == NULL){
        STRU_MEM_LIST *last_list = get_last_memory_list();
        required_list = create_new_memory_list(alligned_size);
        last_list->next_list = required_list;
    }
    int available_bit = bitmap_find_first_bit(required_list->free_slots_bitmap, required_list->bitmap_size, 1);
    if(BITMAP_OP_NOT_FOUND == available_bit){
        create_new_memory_batch(required_list);
        available_bit = MEM_BATCH_SLOT_COUNT * (required_list->batch_count - 1);
    }
    int target_batch = available_bit / MEM_BATCH_SLOT_COUNT;
    STRU_MEM_BATCH *get_last_memory_batch = get_batch_by_num(required_list, target_batch);
    int target_bit = available_bit % MEM_BATCH_SLOT_COUNT;
    bitmap_clear_bit(required_list->free_slots_bitmap, required_list->bitmap_size, available_bit);
    return get_last_memory_batch->batch_mem + (target_bit * alligned_size);
}

/*
 * Free a chunk of memory pointed by ptr
 */
void mem_mngr_free(void * ptr)
{
    if( NULL == ptr){
        return;
    }
    STRU_MEM_LIST *target_list = get_list_by_address(ptr);
    if(target_list == NULL){
        printf("Error: Memory not managed by manager cannot be deallocated\n");
        return;
    }
    STRU_MEM_BATCH *target_batch = get_batch_by_address(ptr);
    int target_bit = get_target_pos(target_list, target_batch, ptr);
    if(target_bit< 0){
        printf("ERROR: \n");
        return;
    }
    int is_target_set = bitmap_bit_is_set(target_list->free_slots_bitmap, target_list->bitmap_size, target_bit);
    if(is_target_set == 1){
        printf("Error: Cannot deallocate memory which is already deallocated\n");
        return; 
    }
    int if_success = bitmap_set_bit(target_list->free_slots_bitmap, target_list->bitmap_size, target_bit);
    if(if_success < 0){
        printf("Error: while setting bit in bitmap\n");
    }
}


/*
 * Create a new memory list with the required size
 * @param slot_size : the slot size of the required list
 * @return: the pointer to newly created memory list
 */
STRU_MEM_LIST *create_new_memory_list(int slot_size){
    //printf("Initializing Memory list with slot_size = %d \n", slot_size);
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

/*
 * Find the address of the memory list by given size  
 * @param alligned_size: the size of the memory list to be found
 * @return: the pointer to the matched memory list or return NULL if not found
 */

STRU_MEM_LIST *find_mem_list_by_size(size_t alligned_size){
    STRU_MEM_LIST * required_list = mem_pool;
    while(required_list != NULL){
        if(required_list->slot_size == alligned_size){
            return required_list;
        }
        required_list = mem_pool->next_list;
    }
    return NULL;
}

/*
 * Returns the pointer to the last linked list
 * @param alligned_size: the size of the memory list to be found
 * @return: the pointer to the matched memory list or return NULL if not found
 */

STRU_MEM_LIST *get_last_memory_list(){
    STRU_MEM_LIST *last_list = mem_pool;
    while(last_list->next_list !=NULL){
        last_list = last_list->next_list;
    }
    return last_list;
}

/*
 * Returns the list matched by the pointer address given 
 * @param ptr: the pointer of the list to be found
 * @return: the pointer to the matched memory list or return NULL if not found
 */

STRU_MEM_LIST *get_list_by_address(void *ptr){
    STRU_MEM_LIST * itr_list = mem_pool;
    while(itr_list != NULL){
        STRU_MEM_BATCH * itr_batch = itr_list->first_batch;
        while(itr_batch != NULL){
            if(check_mem_in_batch(itr_batch, ptr, itr_list->slot_size)){
                return itr_list;
            }
            itr_batch = itr_batch->next_batch;
        }
        itr_list = itr_list->next_list;
    }
    return NULL;
}



STRU_MEM_BATCH *create_new_memory_batch(STRU_MEM_LIST * mem_list){
    //printf("Initializing Memory Batch with slot_size = %d\n",mem_list->slot_size);
    STRU_MEM_BATCH * mem_batch = (STRU_MEM_BATCH *)malloc(sizeof(STRU_MEM_BATCH));
    mem_batch->batch_mem = malloc(mem_list->slot_size * MEM_BATCH_SLOT_COUNT);
    mem_batch->next_batch = NULL;
    if(NULL == mem_list->first_batch){
        mem_list->first_batch = mem_batch;
    }else{
        STRU_MEM_BATCH *last_mem_batch = get_last_memory_batch(mem_list->first_batch);
        last_mem_batch->next_batch = mem_batch;
    }
    mem_list->batch_count ++;
    increment_free_slots_bitmap(mem_list);
    return mem_batch;

}

STRU_MEM_BATCH *get_batch_by_address(void * ptr){
    STRU_MEM_LIST * itr_list = mem_pool;
    while(itr_list != NULL){
        STRU_MEM_BATCH * itr_batch = itr_list->first_batch;
        while(itr_batch != NULL){
            if(check_mem_in_batch(itr_batch, ptr, itr_list->slot_size)){
                return itr_batch;
            }
            itr_batch = itr_batch->next_batch;
        }
        itr_list = itr_list->next_list;
    }
    return NULL;
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

STRU_MEM_BATCH *get_batch_by_num(STRU_MEM_LIST *mem_list, int batch_no){
    STRU_MEM_BATCH * required_batch = mem_list->first_batch;
    for(int i = 1; i <= batch_no; i ++){
        required_batch = required_batch->next_batch;
    }
    return required_batch;
}

int check_mem_in_batch(STRU_MEM_BATCH* mem_batch, void *ptr, int slot_size){
    int if_exists = 0;
    if(mem_batch != NULL){
        void *batch_address = mem_batch->batch_mem;
         if(ptr >= batch_address && ptr <= (batch_address + slot_size * MEM_BATCH_SLOT_COUNT )){
            if_exists = 1;
        }
    }
    return if_exists;
}

void increment_free_slots_bitmap(STRU_MEM_LIST *mem_list){
    unsigned char full_bit = 0xFF;
    size_t single_slot_size = MEM_BATCH_SLOT_COUNT / (sizeof(unsigned char) * 8);
    size_t required_slot_size = mem_list->bitmap_size + single_slot_size;
    mem_list->free_slots_bitmap = realloc(mem_list->free_slots_bitmap, required_slot_size);
    for(int i = 0; i < single_slot_size; i++){
        *(mem_list->free_slots_bitmap + mem_list->bitmap_size + i) = full_bit;
    }
    mem_list->bitmap_size += single_slot_size;
}

int get_target_pos(STRU_MEM_LIST* mem_list, STRU_MEM_BATCH* mem_batch, void *ptr){
    int mem_offset = (ptr - mem_batch->batch_mem) / mem_list->slot_size;
    int batch_offset = 0;
    STRU_MEM_BATCH *itr_batch = mem_list->first_batch;
    while(itr_batch != NULL){
        if(mem_batch == itr_batch){
            break;
        }
        itr_batch = itr_batch->next_batch;
        batch_offset ++;
    }

    if(itr_batch == NULL){
        return -1;
    }
    return mem_offset + (batch_offset * MEM_BATCH_SLOT_COUNT);
}

