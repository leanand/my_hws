/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package scoreboard;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.Hashtable;
import java.util.logging.ConsoleHandler;
import java.util.logging.Handler;
import java.util.logging.Logger;
import java.util.logging.Level;
/**
 *
 * @author lovelinanand
 */
public class Scoreboard {
    private static final Logger LOGGER = Logger.getGlobal();
    public static int cycle;
    private InstructionsList itTable;
    private RegisterList rTable;
    public UnitsList uTable;
    public int totalInstructions;
    public int issueWidth;
    public int cacheMiss;
    public Integer[] dumpCycleNo;
    public String orderType = "inorder";
    public Scoreboard(String inputFile, String orderType){
        LOGGER.info("Initializing Scoreboard");
        this.itTable = new InstructionsList(orderType);
        this.loadInstructions(inputFile);
        this.checkConfig();
        this.rTable = new RegisterList();
        this.uTable = new UnitsList(this.cacheMiss);
        this.loadRegisters();
        cycle = 0;
    }
    public void checkConfig(){
        this.cacheMiss = this.itTable.getCacheMiss();
        this.issueWidth = this.itTable.getIssueWidth();
        this.dumpCycleNo = this.itTable.getDump();
    }
    public void loadRegisters(){
        for(Integer key : this.itTable.keyList()){
            Instructions ins = this.itTable.get(key);
            if(ins.getSource1() != null){
                this.rTable.create(ins.getSource1());
            }
            if(ins.getSource2() != null){
                this.rTable.create(ins.getSource2());
            }
            if(ins.getDestination() != null){
                this.rTable.create(ins.getDestination());
            }
        }
    }
    public void loadInstructions(String inputFile){
        LOGGER.fine("Loading Instructions");
        BufferedReader br = null;
        String currentLine;
        int count = 0;
        try{
            br = new BufferedReader(new FileReader(inputFile));
            while((currentLine = br.readLine()) != null){
                this.itTable.add(currentLine, count);
                count ++;
            }
            this.totalInstructions = count;
        }catch(IOException E){
            System.out.println("Error in reading file");
            E.printStackTrace();
            System.exit(0);
        }
    }
    public void checkInorderOutorder(){
        if(this.orderType.equals("outorder")){
            this.issueOutorder();
        }else{
            this.issueInorder();
        }
    }
    public void issueInorder(){
        for(int i =0; i < this.issueWidth; i ++){
            if(!checkNextInstruction()){
                break;
            }
        }
    }
    public void issueOutorder(){
        
    }
    public boolean checkNextInstruction(){
        boolean instructionIssued = false;
        Instructions nextIns = this.itTable.next();
        if(nextIns != null){
            LOGGER.fine("Next instruction to be issue is "+ nextIns);
            if(this.uTable.canIssue(nextIns)){
                LOGGER.fine("Functional unit is free "+ nextIns);
                if(!this.rTable.isDependant(nextIns)){
                    LOGGER.fine(nextIns + " Dependancy not present, can be issued");
                    this.issueInstruction(nextIns);
                    instructionIssued = true;
                }else{
                    LOGGER.fine(nextIns +" Dependancy present, cannot issue");
                }
            }else{
                LOGGER.fine(nextIns +" Function unit is full, cannot issue");
            }
        }else{
            LOGGER.fine("No instructions available");
        }
        return instructionIssued;
    }
    public void issueInstruction(Instructions instr){
        this.uTable.issue(instr);
        this.rTable.setBusy(instr);
    }
    public void advanceClock(){
        this.uTable.advanceClock(rTable);
        LOGGER.fine("Advancing clock");
    }
    public void dumpScoreboard(){
        for(Integer value : this.dumpCycleNo){
            if(value == cycle){
                this.uTable.dump();
                this.rTable.dump();
                this.itTable.dump();
            }
        }
    }
    public void start(){
        LOGGER.info("Starting the scoreboard process");
        LOGGER.info("CacheMiss: "+this.cacheMiss + " IssueWidth: "+ this.issueWidth);
        while(true){
            if(this.itTable.isCompleted() == true){
                break;
            }
            System.out.println("-----------------------");
            System.out.println("Cycle " + cycle);
            System.out.println("-----------------------");
            this.advanceClock();
            this.checkInorderOutorder();
//            this.checkNextInstruction();   
            this.dumpScoreboard();
            cycle ++ ;
        }
    }
    public static void setDebug(){
        LOGGER.setUseParentHandlers(false);
        Handler systemOut = new ConsoleHandler();
        systemOut.setLevel( Level.ALL );
        LOGGER.addHandler( systemOut );
        LOGGER.setLevel( Level.ALL );
    }
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
//        setDebug();
        Scoreboard scorebd = new Scoreboard("input.txt", "inorder");
        scorebd.start();
    }
    
}
