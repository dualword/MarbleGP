package com.example.marblegp;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    static {
        // Please add other libraries used by this activity.
        System.loadLibrary("MarbleGP");
    }

    public native String stringFromJNI();
}
