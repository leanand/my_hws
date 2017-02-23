#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#define MAX_LIMIT_CONTACTS 100
#define MAX_SIZE_NAME 20
#define MAX_LINE_LIMIT 200

typedef struct _contact {
  char first_name[MAX_SIZE_NAME];
  char last_name[MAX_SIZE_NAME];
  char phone_number[MAX_SIZE_NAME];
  char contact_entry[LINE_MAX];
} CONTACT;

typedef struct _contacts {
  CONTACT * contents[MAX_LIMIT_CONTACTS];
  int size;
} CONTACTS;

CONTACTS * Contacts;

void display_contacts(){
  int i;
  printf("\n");
  for(i = 0; i < Contacts->size; i ++ ){
    printf("%d . %s\n", i+1,Contacts->contents[i]->contact_entry);
  }
  printf("\n");
}
void display_contact(int index){
  if(index > -1 && index < Contacts->size){
    printf("\n\t%s\n", Contacts->contents[index]->contact_entry);
  }else{
    printf("\nNothing to display\n");
  }
}
int _compare_contacts(CONTACT * contact_1, CONTACT * contact_2, int should_check_phone){
  int status = strcmp(contact_1->first_name, contact_2->first_name);
  if(status == 0){
    status = strcmp(contact_1->last_name, contact_2->last_name);
    if(status == 0 && should_check_phone == 1){
      status = strcmp(contact_1->phone_number, contact_2->phone_number);
    }
  }  
  return status;
}

void _swap_contact(int x, int y){
  if( x == y){
    return;
  }
  CONTACT * temp = Contacts->contents[x];
  Contacts->contents[x] = Contacts->contents[y];
  Contacts->contents[y] = temp;
}

int _do_partition(int start, int end){
  CONTACT * pivot = Contacts->contents[end];
  int pIndex = start;
  int i;
  for(i = start; i <= end - 1; i ++){
    if(_compare_contacts(Contacts->contents[i], pivot, 1) < 1){
      _swap_contact(i, pIndex);
      pIndex ++;
    }
  }
  _swap_contact(pIndex, end);
  return pIndex;
}

int _get_random(int start, int range){
  srand(time(NULL));
  return rand() % (range - start) + start;
}

void _quick_sort(int start, int end){
  if(start >= end){
    return;
  }
  int rand_no = _get_random(start, end + 1);
  _swap_contact(rand_no, end);
  int pivot = _do_partition(start, end);
  _quick_sort(start, pivot - 1);
  _quick_sort(pivot + 1 , end);
}


int _binary_search(CONTACT * target){
  int start = 0; 
  int end = Contacts->size - 1;
  while (start <= end){
    int mid = start + (end - start) / 2;
    int comp = _compare_contacts(Contacts->contents[mid], target, 0);
    if(comp == 0){
      return mid;
    }else if(comp < 0){
      start = mid + 1;
    }else{
      end = mid - 1;
    }
  }
  return -1;
}


void sort_contact(){
  if(Contacts->size < 2){
    return ;
  }
  _quick_sort(0, Contacts->size - 1);
}

void delete_contact(int index){
  int end = Contacts->size - 1;
  _swap_contact(index, end);
  free(Contacts->contents[end]);
  Contacts->contents[end] = NULL;
  Contacts->size --;
  sort_contact();
}

int search_contact(char * input_name){
  CONTACT * input_contact = malloc(sizeof(CONTACT));
  sscanf(input_name, "%s %s", input_contact->first_name, input_contact->last_name);
  int status = _binary_search(input_contact);
  free(input_contact);
  return status;
}

void insert_contact(char * input_name, char * input_phone_number){
   CONTACT * new_contact = malloc(sizeof(CONTACT));
   strcpy(new_contact->contact_entry, input_name);
   strcat(new_contact->contact_entry, ", ");
   strcat(new_contact->contact_entry, input_phone_number);
   sscanf(input_name, "%s %s", new_contact->first_name, new_contact->last_name);
   Contacts->contents[Contacts->size++] = new_contact;
   sort_contact();
}

void create(char * file_name){
  FILE * input_file;
  char temp_line[MAX_LINE_LIMIT] = "";
  int line_count = 0;

  input_file = fopen(file_name, "r");
  if(input_file == NULL){
    fprintf(stderr, "Cannot open input file\n");
    abort();
  }
 
  while(fgets(temp_line, MAX_LINE_LIMIT, input_file) != NULL){
    int temp_length = strlen(temp_line);
    if(temp_line[temp_length - 1] == '\n'){
      temp_line[temp_length - 1] = '\0';  
    }
    CONTACT *temp_contact = malloc(sizeof(CONTACT));
    strcpy(temp_contact->contact_entry, temp_line);
    sscanf(temp_line, "%s %[^,], %s", temp_contact->first_name, temp_contact->last_name, temp_contact->phone_number);
    Contacts->contents[line_count] = temp_contact;
    temp_line[0] = '\0';
    line_count ++;
  }
  Contacts->size = line_count;
  fclose(input_file);
}
void free_contacts(){
  int i;
  for(i = 0; i < Contacts->size; i ++){
    free(Contacts->contents[i]);
  }
  free(Contacts);
}

void show_menu(){
  char line[MAX_LINE_LIMIT];
  int choice = 1, index;
  char input_name[MAX_SIZE_NAME] = "";
  char input_number[MAX_SIZE_NAME] = "";
  while(choice > 0 && choice < 5){
    choice = 0;
    printf("\nPhone Directory \n");
    printf("1. Sort\n");
    printf("2. Search\n");
    printf("3. Delete\n");
    printf("4. Insert\n");
    printf("5. Exit\n");
    printf("Enter the choice: \n");  
    scanf(" %d", &choice);
    switch(choice){
      case 1:
        sort_contact();
        printf("After sorting: \n");
        display_contacts();
        break;
      case 2:
        memset(input_name, 0, MAX_SIZE_NAME);
        printf("\nEnter full name to be inserted:\n");
        scanf(" %[^\n]s", input_name);
        index = search_contact(input_name);
        printf("\nSearch Result:");
        if(index < 0){
          printf("\n~~~~~Name not found~~~~~\n");
        }else{
          display_contact(index);
        }
        break;
      case 3:
        memset(input_name, 0, MAX_SIZE_NAME);
        printf("\nEnter full name to be deleted:\n");
        scanf(" %[^\n]s", input_name);
        index = search_contact(input_name);
        if(index < 0){
          printf("\nCannot delete.Name not found\n");
        }else{
          delete_contact(index);
          printf("\n~~~~~Name deleted~~~~~\n");
          display_contacts();
        }
        break;
      case 4: 
        memset(input_name, 0, MAX_SIZE_NAME);
        printf("\nEnter full name to be inserted:\n");
        scanf(" %[^,], %s", input_name, input_number);
        insert_contact(input_name, input_number);
        printf("\n~~~~~Name inserted~~~~~\n");
        display_contacts();
        break;
      case 5:
        printf("Exiting\n");
        break;
      default:
        printf("Invalid selection, exiting...\n");
    }
  }
}

int main(int args, char * argv[]){
  Contacts = malloc(sizeof(CONTACTS));
  Contacts->size = 0;
  if(args != 2){
    fprintf(stderr, "Input file not provided \n");
    return -1;
  }
  char * file_name = argv[1];
  create(file_name);
  display_contacts();
  // sort_contact();
  show_menu();
  free_contacts();
  return 1;
}

