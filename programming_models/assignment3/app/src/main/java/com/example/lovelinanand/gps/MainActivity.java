package com.example.lovelinanand.gps;

import android.Manifest;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.hardware.Sensor;
import android.os.BatteryManager;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;
import android.widget.Toast;

import org.w3c.dom.Text;

import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Calendar;

public class MainActivity extends AppCompatActivity implements ActivityCompat.OnRequestPermissionsResultCallback {
    // X - Max update Seconds
    // Y - Reduce by or Increase by seconds
    // U - Min update Seconds
    private Locator locator;
    private SensorActivity sensor;
    private BatteryActivity battery;
    private static final int REQUEST_FINE_LOCATION = 0;
    private static int X = 120;
    private static int Y = 30;
    private static int W = 50;
    private static int U = 60;
    private static int Z = 120;
    public static int update_interval = X;
    public boolean ascending = false;
    public boolean batteryStatus = true; // true is > 50% and false is <= 50 %
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        final TextView intervalTimer = (TextView) findViewById(R.id.interval_text);
        final TextView latitudeInput =  (TextView) findViewById(R.id.latitude_cont);
        final TextView longitudeInput =  (TextView) findViewById(R.id.longitude_cont);
        ((TextView) findViewById(R.id.value_update)).setText("X = "+X+", Y = "+Y+", W = "+W+", U = "+U+", Z = "+Z);
        if(getBatteryLevel() <= 50.0){
            update_interval = X + Z;
            batteryStatus = false;
        }
        locator = new Locator(this, latitudeInput, longitudeInput);
        sensor = new SensorActivity(this);
//        battery = new BatteryActivity(this);

        registerReceiver(battery, new IntentFilter(Intent.ACTION_BATTERY_CHANGED));
        updateInterval();
        Log.d("locator", "Location is called");
    }
    protected void updateTimer(){
        final TextView cdTextView =  (TextView) findViewById(R.id.last_update);
        DateFormat df = new SimpleDateFormat("hh:mm:ss"); //format time
        String time = df.format(Calendar.getInstance().getTime());
        cdTextView.setText(time);
    }
    public void updateInterval(){
        final TextView text_interval = (TextView) findViewById(R.id.interval_text);
        text_interval.setText(update_interval + " sec");
    }
    public void updateLocationProvider(String input){
        final TextView locationProvider = (TextView) findViewById(R.id.location_provider);
        locationProvider.setText(input);
    }
    @Override
    public void onRequestPermissionsResult(int requestCode, String permissions[], int[] grantResults){
        switch(requestCode){
            case REQUEST_FINE_LOCATION: {
                if(grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED){
                    Log.d("locator", "Location permission is granted");
                    this.locator.checkPermission();
                }else{
                    Log.d("locator", "Permission not granted");
                }
            }
            break;
            default:{
                Log.d("locator", "Invalid Value");
            }

        }
    }

    public void processShakeEvent() {
        float currentBatteryLevel;
        boolean currentBatteryStatus;
        int minimal_interval = U;
        int maximum_interval = X;
        if(ascending == true){
            update_interval += Y;
        }else{
            update_interval -= Y;
        }
        currentBatteryLevel = getBatteryLevel();
        if(currentBatteryLevel <= 50.0) {
            minimal_interval = U + Z;
            maximum_interval = X + Z;
            currentBatteryStatus = false;
            if(batteryStatus == true){
                update_interval += Z;
            }
        }else{
            currentBatteryStatus = true;
            if(batteryStatus == false){
                update_interval -= Z;
            }
        }
        if(batteryStatus != currentBatteryStatus){
            batteryStatus = currentBatteryStatus;

        }
        if((update_interval <= minimal_interval) || (update_interval >= maximum_interval)){
            Log.d("MainActivity", "its changed");
            ascending = !ascending;
        }
        Log.d("MainActivity", "Processing shake event update_interval = " +update_interval + " Ascending is" + ascending + " U is " + U + " X is " + X);
        locator.unregisterListener();
        locator.registerListener();
        Toast.makeText(this, "Interval Updated", Toast.LENGTH_LONG).show();
        updateInterval();
    }
    public float getBatteryLevel() {
        Intent batteryIntent = registerReceiver(null, new IntentFilter(Intent.ACTION_BATTERY_CHANGED));
        int level = batteryIntent.getIntExtra(BatteryManager.EXTRA_LEVEL, -1);
        int scale = batteryIntent.getIntExtra(BatteryManager.EXTRA_SCALE, -1);

        return ((float)level / (float)scale) * 100.0f;
    }
}
