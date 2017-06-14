#include <stdio.h>
#include <stdlib.h>
#define MAXIMUM_BRANCHES 10
#define MAXIMUM_GLOBAL_PREDICTOR_STATES 64

typedef struct _branch {
  int branch_id;
  char is_taken_char;
  int is_branch_taken;
  int taken_branch_id;
} Branch;

int do_local_prediction(int local_predictor_states[], Branch current_branch){
  if(current_branch.branch_id < 0 && current_branch.branch_id >= MAXIMUM_BRANCHES){
    fprintf(stderr, "Branch out of limit\n" );
    exit(1);
  }
  int branch_state = local_predictor_states[current_branch.branch_id];
  if(branch_state > 1){
    return 1;
  }else{
    return 0;
  }
}

int do_global_prediction(int global_predictor_states[], unsigned last_global_state){
  if(last_global_state >= MAXIMUM_GLOBAL_PREDICTOR_STATES){
    fprintf(stderr, "Global state out of limit\n" );
    exit(1);
  }
  int branch_state = global_predictor_states[last_global_state];
  if(branch_state > 1){
    return 1;
  }else{
    return 0;
  }
}

int do_selection(int selector_states[], Branch current_branch){
  if(current_branch.branch_id < 0 && current_branch.branch_id >= MAXIMUM_BRANCHES){
    fprintf(stderr, "Branch out of limit\n" );
    exit(1);
  }
  int branch_state = selector_states[current_branch.branch_id];
  if(branch_state > 1){
    return 1;
  }else{
    return 0;
  }
}

int do_tournament_prediction(int selection, int global_prediction, int local_prediction){
  if(selection == 0){
    return local_prediction;
  }else{
    return global_prediction;
  }
}

void update_local_predictor(int local_predictor_states[], Branch current_branch){
  if(current_branch.branch_id < 0 && current_branch.branch_id >= MAXIMUM_BRANCHES){
    fprintf(stderr, "Branch out of limit\n" );
    exit(1);
  }
  int branch_state = local_predictor_states[current_branch.branch_id];
  if(current_branch.is_branch_taken == 1 && branch_state < 3){
    branch_state ++ ;
  }else if(current_branch.is_branch_taken == 0 && branch_state > 0){
    branch_state --;
  }
  local_predictor_states[current_branch.branch_id] = branch_state;
}

void update_global_predictor(int global_predictor_states[], unsigned *last_global_predictor_index, Branch current_branch){
  if(*last_global_predictor_index >= MAXIMUM_GLOBAL_PREDICTOR_STATES){
    fprintf(stderr, "Global state out of limit\n" );
    exit(1);
  }
  unsigned temp_global_index = *last_global_predictor_index;
  // printf("update_global_predictor_index : %u\n", temp_global_index);
  int branch_state = global_predictor_states[temp_global_index] ;
  if(current_branch.is_branch_taken == 1 && branch_state < 3){
    branch_state ++ ;
  }else if(current_branch.is_branch_taken == 0 && branch_state > 0){
    branch_state --;
  }
  global_predictor_states[temp_global_index] = branch_state;

  temp_global_index = (temp_global_index << 1) | current_branch.is_branch_taken;
  temp_global_index = temp_global_index & (MAXIMUM_GLOBAL_PREDICTOR_STATES - 1);
  *last_global_predictor_index = temp_global_index;
};

void update_selector(int selector_states[], int local_prediction, int global_prediction, Branch current_branch){
  if(current_branch.branch_id < 0 && current_branch.branch_id >= MAXIMUM_BRANCHES){
    fprintf(stderr, "Branch out of limit\n" );
    exit(1);
  }
  if(local_prediction == global_prediction){
    return;
  }
  int branch_state = selector_states[current_branch.branch_id];

  if(local_prediction == current_branch.is_branch_taken && branch_state > 0){
    branch_state --;
  }else if(global_prediction == current_branch.is_branch_taken && branch_state < 3){
    branch_state ++;
  }
  selector_states[current_branch.branch_id] = branch_state;
}

void initialize_zero(int ar[], int size){
  for(int i = 0; i < size; i ++){
    ar[i] = 0;
  }
}

char parse_taken(int is_branch_taken){
  if(is_branch_taken == 1){
    return 't';
  }else{
    return 'n';
  }
}

char parse_selector(int is_branch_taken){
  if(is_branch_taken == 1){
    return 'g';
  }else{
    return 'l';
  }
}


int main(int args, char *argv[]){

  int local_predictor_states[MAXIMUM_BRANCHES];
  int global_predictor_states[MAXIMUM_GLOBAL_PREDICTOR_STATES];
  int selector_states[MAXIMUM_BRANCHES];
  unsigned last_global_predictor_index = 0;
  printf("Branch Predictor\n");
  initialize_zero(local_predictor_states, MAXIMUM_BRANCHES);
  initialize_zero(global_predictor_states, MAXIMUM_GLOBAL_PREDICTOR_STATES);


  FILE *input_file;
  FILE *output_file;
  FILE *statistics;

  if(args != 3){
    fprintf(stderr, "Invalid arguments\n");
  }
  
  char *input_file_st = argv[1];
  char *output_file_st = argv[2];
  input_file = fopen(input_file_st, "r");
  output_file = fopen(output_file_st, "w");
  statistics = fopen("statistics.txt", "w");
  if(input_file == NULL){
    fprintf(stderr, "Cant open file.\n");
    exit(1);
  }



  Branch current_branch;
  int local_prediction = 0;
  int global_prediction = 0;
  int selection= 0;
  int prediction = 0;
  int is_predicted_rite = 0;
  int i = 0;
  int local_prediction_count = 0;
  int global_predictor_count = 0;
  int tournament_prediction_count = 0;
  int input_size;
  while(!feof(input_file)){

    input_size = fscanf(input_file, "%d%c%d", &current_branch.branch_id, &current_branch.is_taken_char, &current_branch.taken_branch_id);
    if(input_size != 3){
      continue;
    }
    current_branch.is_branch_taken = (current_branch.is_taken_char =='t')? 1 : 0;
    local_prediction = do_local_prediction(local_predictor_states, current_branch);
    global_prediction = do_global_prediction(global_predictor_states, last_global_predictor_index);
    selection = do_selection(selector_states, current_branch);
    prediction = do_tournament_prediction(selection, global_prediction, local_prediction);
    
    if(local_prediction == current_branch.is_branch_taken){
      local_prediction_count++;
    }
    if(global_prediction == current_branch.is_branch_taken){
      global_predictor_count++;
    }
    if(prediction == current_branch.is_branch_taken){
      tournament_prediction_count++;
    }
    
    printf("%d :Local Selection = %d Global Selection = %d Selection = %d  Prediction = %d \n",i, local_prediction, global_prediction, selection , prediction);
    // printf("%d%c%c%c%c%c\n",current_branch.branch_id, parse_taken(local_prediction), parse_taken(global_prediction), parse_selector(selection), parse_taken(prediction), current_branch.is_taken_char);
    fprintf(output_file, "%d%c%c%c%c%c\n",current_branch.branch_id, parse_taken(local_prediction), parse_taken(global_prediction), parse_selector(selection), parse_taken(prediction), current_branch.is_taken_char);
    // printf("%s\n",tempBuffer);
    if(prediction == current_branch.is_branch_taken){
      is_predicted_rite = 1;
      // printf("Prediction true\n");
    }else{
      is_predicted_rite = 0;
      // printf("Prediction false\n");
    }
    update_local_predictor(local_predictor_states, current_branch);
    update_global_predictor(global_predictor_states, &last_global_predictor_index, current_branch);
    update_selector(selector_states, local_prediction, global_prediction, current_branch);
   /* printf("\n");
    for(int j =0; j < MAXIMUM_BRANCHES ; j ++){
      printf("%d", local_predictor_states[j]);
    }
    printf("\n");
    for(int j =0; j < MAXIMUM_GLOBAL_PREDICTOR_STATES ; j ++){
      printf("%d", global_predictor_states[j]);
    }
    printf("\n");
    for(int j =0; j < MAXIMUM_BRANCHES ; j ++){
      printf("%d", selector_states[j]);
    }
    printf("\n\n");*/


    i ++;
  }

  printf("Statistics\n");
  printf("Local Prediction : %d of %d are predicted correct\n", local_prediction_count , i);
  printf("Global Prediction : %d of %d are predicted correct\n", global_predictor_count , i);
  printf("Tournament Prediction : %d of %d are predicted correct\n", tournament_prediction_count , i);

  fprintf(statistics, "Statistics\n");
  fprintf(statistics,"Local Prediction : %d of %d are predicted correct\n", local_prediction_count , i);
  fprintf(statistics,"Global Prediction : %d of %d are predicted correct\n", global_predictor_count , i);
  fprintf(statistics, "Tournament Prediction : %d of %d are predicted correct\n", tournament_prediction_count , i);
  fclose(input_file);
  fclose(output_file);
  fclose(statistics);

  return 1;
}