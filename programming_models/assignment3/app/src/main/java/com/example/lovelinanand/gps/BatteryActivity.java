package com.example.lovelinanand.gps;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.BatteryManager;
import android.util.Log;

/**
 * Created by lovelinanand on 5/2/17.
 */

public class BatteryActivity extends BroadcastReceiver {
    private double BATTERY_LIMIT = 50.0;
    private Context context;
    private boolean batteryStatus = true;
    public BatteryActivity(Context context){
        this.context = context;
        if(((MainActivity) context).getBatteryLevel() <= BATTERY_LIMIT){
            batteryStatus = false;
        }
    }
    @Override
    public void onReceive(Context context, Intent intent) {
        int batteryLevel = intent.getIntExtra(BatteryManager.EXTRA_LEVEL, 0);
        int maxLevel = intent.getIntExtra(BatteryManager.EXTRA_SCALE, 0);
        int batteryHealth = intent.getIntExtra(BatteryManager.EXTRA_HEALTH, BatteryManager.BATTERY_HEALTH_UNKNOWN);
        float batteryPercentage = ((float) batteryLevel / (float) maxLevel) * 100;
        boolean newBatteryStatus;
        if(batteryPercentage > BATTERY_LIMIT){
            newBatteryStatus = true;
        }else{
            newBatteryStatus = false;
        }
        if(newBatteryStatus != batteryStatus){
            batteryStatus = newBatteryStatus;
            ((MainActivity) context).processShakeEvent();
        }
        if(batteryPercentage > BATTERY_LIMIT && batteryStatus == false ){
            Log.d("battery", "Battery is greater than 50%");
            batteryStatus = true;

        }else{
            Log.d("battery", "Battery is less than or equal to 50%");
        }
    }
}
