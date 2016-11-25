/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package scoreboard;

import java.util.Hashtable;
import java.util.logging.Logger;

/**
 *
 * @author lovelinanand
 */
public class UnitsList {
    public static int cycle;
    private static final Logger LOGGER = Logger.getGlobal();
    private Hashtable<String, Unit> unitsTable;
    public UnitsList(){
        this.unitsTable = new Hashtable<String, Unit>();
        this.createAlU();
    }
    public void addUnit(String unitName, Unit unit){
        this.unitsTable.put(unitName, unit);
    }
    private void createAlU(){
        Unit ALU = new Unit("ALU", "pipelined", 1,new String[] {"ADD", "SUB"});
        ALU.setRequiredCycle(4);
        this.addUnit("ALU", ALU);
        
    }
    public Unit getUnitByOperation(String operation){
        Unit targetUnit = null;
        for(String key : this.unitsTable.keySet()){
            Unit tmpUnit = this.unitsTable.get(key);
            if(tmpUnit.doesSupportOpt(operation)){
                targetUnit = tmpUnit;
                break;
            }
        }
        return targetUnit;
    }
    public Unit getUnitByInstruction(Instructions inst){
        Unit targetUnit;
        String operation = inst.getOperation();
        targetUnit = this.getUnitByOperation(operation);
        return targetUnit;
    }
    public boolean canIssue(Instructions inst){
        boolean canIssue = false;
        Unit targetUnit = this.getUnitByInstruction(inst);
        if(targetUnit == null){
            System.out.println("Cannot find suitable functional unit for operation "+ inst);
            System.exit(1);
        }
        if(targetUnit.isFree()){
            canIssue = true;
        }
        return canIssue;
    }
    public void issue(Instructions inst){
        if(this.canIssue(inst)){
            Unit targetUnit = this.getUnitByInstruction(inst);
            if(targetUnit == null){
                LOGGER.warning(inst + " cannot find target instruction");
                System.exit(0);
            }
            targetUnit.issue(inst);
        }else{
            LOGGER.warning(inst + " Issuing not issuable instruction");
        }
    }
    public void advanceClock(RegisterList rTable){
        for(String key : this.unitsTable.keySet()){
            Unit targetUnit = this.unitsTable.get(key);
            targetUnit.advanceClock(rTable);
        }
    }
    
    public void dump(){
        String leftAlignFormat = "| %-7s | %-9s | %-61s |%n";
        System.out.println("Function Unit Status");
        System.out.format("+---------+-----------+---------------------------------------------------------------+%n");   
        System.out.format("| FU Name | Status    |                  Instruction(s)                               |%n");
        System.out.format("+---------+-----------+---------------------------------------------------------------+%n");
        for(String key : this.unitsTable.keySet()){
            Unit targetUnit = this.unitsTable.get(key);
            System.out.format(leftAlignFormat, targetUnit.name, targetUnit.getStatusString(), targetUnit.getProcessingInstructions());
        }
        System.out.format("+---------+-----------+---------------------------------------------------------------+%n");
    }
}
