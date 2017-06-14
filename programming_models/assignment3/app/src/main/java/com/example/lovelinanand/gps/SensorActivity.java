package com.example.lovelinanand.gps;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.util.Log;

/**
 * Created by lovelinanand on 5/1/17.
 */

public class SensorActivity implements SensorEventListener {
    private Context context;
    private SensorManager sensorManager;
    private Sensor accSensor;
    private long MIN_TIME_BETWEEN_SHAKES = 3000;
    private long SHAKE_THRESHOLD = 800;
    private long lastShakeTime;
    public SensorActivity(Context context){
        this.context = context;
        this.sensorManager = (SensorManager)context.getSystemService(context.SENSOR_SERVICE);
        this.accSensor = sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        this.sensorManager.registerListener(this, accSensor, SensorManager.SENSOR_DELAY_NORMAL);
        Log.d("sensor", "Sensor Listener attached");
    }
    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy){
    }
    @Override
    public void onSensorChanged(SensorEvent event){
        if(event.sensor.getType() == Sensor.TYPE_ACCELEROMETER){
            long currTime = System.currentTimeMillis();
            if((currTime - lastShakeTime) > MIN_TIME_BETWEEN_SHAKES){
                float x = event.values[0];
                float y = event.values[1];
                float z = event.values[2];

                double acceleration = Math.sqrt(Math.pow(x, 2)+ Math.pow(y, 2) + Math.pow(z, 2)) - SensorManager.GRAVITY_EARTH;
//                Log.d("Sensor", acceleration + "");
                if(Math.abs(acceleration) > 8){
                    lastShakeTime = currTime;
                    ((MainActivity) this.context).processShakeEvent();
                }
            }
        }
    }
}
