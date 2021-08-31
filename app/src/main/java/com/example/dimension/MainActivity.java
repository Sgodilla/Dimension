package com.example.dimension;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.widget.TextView;

import com.example.dimension.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'dimension' library on application startup.
    static {
        System.loadLibrary("dimension");
    }

    private ActivityMainBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

    }

    /**
     * A native method that is implemented by the 'dimension' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
}