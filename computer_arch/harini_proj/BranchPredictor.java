import java.io.*;
import java.util.Scanner;

public class BranchPredictor {  
  private int[] local_predictor_states;
  private int[] global_predictor_states;
  private int[] selector_states;
  private int last_global_state;
  public BranchPredictor(){
    this.local_predictor_states = new int[10];
    this.global_predictor_states = new int[64];
    this.selector_states = new int[10];
    this.last_global_state = 0;
  }
  public int do_local_prediction(int branch_id){
    int prediction = this.local_predictor_states[branch_id];
    if(prediction > 1){
      return 1;
    }else{
      return 0;
    }
  }
  public int do_global_prediction(){
    int prediction = this.global_predictor_states[last_global_state];
    if(prediction > 1){
      return 1;
    }else{
      return 0;
    }
  }
  public int do_selection(int branch_id){
    int prediction = this.selector_states[branch_id];
    if(prediction > 1){
      return 1;
    }else{
      return 0;
    }
  }
  public int do_tournament_prediction(int selection, int global_prediction, int local_prediction){
    if(selection == 0){
      return local_prediction;
    }else{
      return global_prediction;
    }
  }
  public void update_local_predictor(int branch_id, int is_branch_taken){
    int branch_state = this.local_predictor_states[branch_id];

    if(is_branch_taken == 1 && branch_state < 3 ){
      branch_state ++;
    }else if(is_branch_taken == 0 && branch_state > 0){
      branch_state --;
    }
    this.local_predictor_states[branch_id] = branch_state;
  }

  public void update_global_predictor(int is_branch_taken){
    int last_global_state = this.last_global_state;
    int branch_state = this.global_predictor_states[last_global_state];
    if(is_branch_taken == 1 && branch_state  < 3){
      branch_state ++;
    }else if(is_branch_taken == 0 && branch_state > 0){
      branch_state --;
    }
    this.global_predictor_states[last_global_state] = branch_state;
    last_global_state = (last_global_state << 1) | is_branch_taken;
    last_global_state = last_global_state & 63;
    this.last_global_state = last_global_state;
  }

  public void update_selector(int local_prediction, int global_prediction, int branch_id, int is_branch_taken){
    if(local_prediction == global_prediction){
      return;
    }
    
    int branch_state = this.selector_states[branch_id];

    if(global_prediction == is_branch_taken && branch_state < 3 ){
      branch_state ++;
    }else if(local_prediction == is_branch_taken && branch_state > 0){
      branch_state --;
    }
    this.selector_states[branch_id] = branch_state;
  }

  public static char parseTaken(int taken){
    if(taken == 1){
      return 't';
    }else{
      return 'n';
    }
  }
  public static char parseSelection(int selection){
      if(selection == 1){
        return 'g';
      }else{
        return 'l';
      }
  }
  public static void main(String[] args) throws FileNotFoundException, IOException{
    int argLength = args.length;
    if(argLength != 2){
      System.out.println("Error: Invalid Arguments");
      return;
    }
    String inputLine;
    String input_file = args[0];
    String output_file = args[1];
    File input_file_fd = new File(input_file);
    Scanner sc = new Scanner(input_file_fd);
    FileWriter output_file_fd = new FileWriter(output_file);
    BranchPredictor bp = new BranchPredictor();
    int local_prediction;
    int global_prediction;
    int selection;
    int prediction;
    int local_prediction_count = 0;
    int global_prediction_count = 0;
    int tournament_prediction_count = 0;
    int i = 0;
    while(sc.hasNext()){
      inputLine = sc.nextLine();
      int branch_id;
      int is_branch_taken;
      int next_branch_id;
      branch_id = Character.getNumericValue(inputLine.charAt(0));
      if(inputLine.charAt(1) == 't'){
        is_branch_taken = 1;
      }else{
        is_branch_taken = 0;
      }
      next_branch_id = Character.getNumericValue(inputLine.charAt(2));
      local_prediction = bp.do_local_prediction(branch_id);
      if(local_prediction == is_branch_taken){
        local_prediction_count ++;
      }
      global_prediction = bp.do_global_prediction();
      if(global_prediction == is_branch_taken){
        global_prediction_count ++;
      }
      selection = bp.do_selection(branch_id);
      prediction = bp.do_tournament_prediction(selection, global_prediction, local_prediction);
      if(prediction == is_branch_taken){
        tournament_prediction_count ++;
      }
      output_file_fd.write("" + branch_id +parseTaken(local_prediction) + parseTaken(global_prediction) + parseSelection(selection) + parseTaken(prediction) + inputLine.charAt(1) +"\n");
      bp.update_local_predictor(branch_id, is_branch_taken);
      bp.update_global_predictor(is_branch_taken);
      bp.update_selector(local_prediction, global_prediction, branch_id, is_branch_taken);
      i ++;
    }
    output_file_fd.close();
    System.out.println("Completed!");
    System.out.println("Statistics\n");
    System.out.println("Local Prediction : "+local_prediction_count +" of "+i+" are predicted correct");
    System.out.println("Global Prediction : "+ global_prediction_count +" of "+i+" are predicted correct");
    System.out.println("Tournament Prediction : "+tournament_prediction_count +" of "+i+" are predicted correct");
  }
}