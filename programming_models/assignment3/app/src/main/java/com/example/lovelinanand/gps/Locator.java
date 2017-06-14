package com.example.lovelinanand.gps;

import android.Manifest;
import android.app.Activity;
import android.content.pm.PackageManager;
import android.location.Criteria;
import android.location.LocationListener;
import android.location.Location;
import android.content.Context;
import android.location.LocationManager;
import android.os.Bundle;
import android.support.v4.app.ActivityCompat;
import android.util.Log;
import android.widget.TextView;
import android.widget.Toast;

import java.util.Calendar;
/**
 * Created by lovelinanand on 5/1/17.
 */

public class Locator implements LocationListener{
    private Context context;
    private LocationManager locationManager;
    private TextView LatitudeText;
    private TextView LongitudeText;
    private static final int REQUEST_FINE_LOCATION = 0;
    public Locator(Context context, TextView LatitudeText, TextView LongitudeText){
        super();
        this.context = context;
        this.LatitudeText = LatitudeText;
        this.LongitudeText = LongitudeText;
        this.checkPermission();
    }
    public void checkPermission(){
        LocationManager locationManager = null;
        if (ActivityCompat.checkSelfPermission(this.context, Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED && ActivityCompat.checkSelfPermission(this.context, Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
                ActivityCompat.requestPermissions((Activity)this.context, new String[]{Manifest.permission.ACCESS_FINE_LOCATION, Manifest.permission.ACCESS_COARSE_LOCATION}, REQUEST_FINE_LOCATION);
        }else {
            Log.d("locator", "Permission granted... Location manager created");
            this.locationManager = (LocationManager) this.context.getSystemService(this.context.LOCATION_SERVICE);
            this.registerListener();
        }
    }
    public void registerListener(){
        if (ActivityCompat.checkSelfPermission(this.context, Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED && ActivityCompat.checkSelfPermission(this.context, Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
//            ActivityCompat.requestPermissions((Activity)this.context, new String[]{Manifest.permission.ACCESS_FINE_LOCATION, Manifest.permission.ACCESS_COARSE_LOCATION}, REQUEST_FINE_LOCATION);
        }else {
            int updateTime = ((MainActivity) this.context).update_interval * 1000;
            this.locationManager.requestLocationUpdates(LocationManager.GPS_PROVIDER, updateTime, 0, this);
//            this.locationManager.requestLocationUpdates(LocationManager.NETWORK_PROVIDER, updateTime, 0, this);
        }
    }
    public void unregisterListener(){
        this.locationManager.removeUpdates(this);
    }
    @Override
    public void onLocationChanged(Location location) {
        Log.d("locator", "Location found : " + location.getLatitude() + ", " + location.getLongitude() + (location.hasAccuracy() ? " : +- " + location.getAccuracy() + " meters" : ""));
        Log.d("locator", "Location has changed");
        this.LatitudeText.setText(location.getLatitude() +"");
        this.LongitudeText.setText(location.getLongitude() + "");
        MainActivity activity = (MainActivity) this.context;
        activity.updateLocationProvider(location.getProvider());
        Toast.makeText(activity, "Location Changed", Toast.LENGTH_SHORT).show();
        activity.updateTimer();
    }

    @Override
    public void onStatusChanged(String provider, int status, Bundle extras) {
        Log.d("locator", "Status has changed");
    }

    @Override
    public void onProviderEnabled(String provider) {
        Log.d("locator", "Provider has enabled");
        this.registerListener();
    }

    @Override
    public void onProviderDisabled(String provider) {
        ((MainActivity) this.context).updateLocationProvider("DISABLED");
        Log.d("locator", "Provider has disabled"
        );
    }
}
