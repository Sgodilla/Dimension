package com.example.dimension;

import static android.content.ContentValues.TAG;

import android.content.DialogInterface;
import android.content.res.Resources;
import android.hardware.display.DisplayManager;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.GestureDetector;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;
import android.view.View;
import android.view.WindowManager;
import android.widget.ImageButton;
import android.widget.PopupMenu;
import android.widget.Toast;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.view.MotionEventCompat;

import com.google.android.material.snackbar.Snackbar;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class ARActivity extends AppCompatActivity
    implements GLSurfaceView.Renderer, DisplayManager.DisplayListener {

    private static final int SNACKBAR_UPDATE_INTERVAL_MILLIS = 1000; // In milliseconds.
    private static final int NUM_DEPTH_SETTINGS_CHECKBOXES = 2;
    private static final int NUM_INSTANT_PLACEMENT_SETTINGS_CHECKBOXES = 1;
    private static final String TAG = "dimension-debugger";

    private GLSurfaceView surfaceView;

    private boolean viewportChanged = false;
    private int viewportWidth;
    private int viewportHeight;

    private final DepthSettings depthSettings = new DepthSettings();
    private boolean[] depthSettingsMenuDialogCheckboxes = new boolean[NUM_DEPTH_SETTINGS_CHECKBOXES];

    private final InstantPlacementSettings instantPlacementSettings = new InstantPlacementSettings();
    private boolean[] instantPlacementSettingsMenuDialogCheckboxes =
            new boolean[NUM_INSTANT_PLACEMENT_SETTINGS_CHECKBOXES];

    // Opaque native pointer to the native application instance.
    private long nativeApplication;

    enum transformModes {move, rotate, scale};
    transformModes transformode = transformModes.move;

    private GestureDetector gestureDetector;
    private ScaleGestureDetector scaleGestureDetector;
    private float scaleFactor = 1.0f;

    private Snackbar snackbar;
    private Handler planeStatusCheckingHandler;
    private final Runnable planeStatusCheckingRunnable =
            new Runnable() {
                @Override
                public void run() {
                    // The runnable is executed on main UI thread.
                    try {
                        if (JniInterface.hasDetectedPlanes(nativeApplication)) {
                            if (snackbar != null) {
                                snackbar.dismiss();
                            }
                            snackbar = null;
                        } else {
                            planeStatusCheckingHandler.postDelayed(
                                    planeStatusCheckingRunnable, SNACKBAR_UPDATE_INTERVAL_MILLIS);
                        }
                    } catch (Exception e) {
                        Log.e(TAG, e.getMessage());
                    }
                }
            };


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        surfaceView = (GLSurfaceView) findViewById(R.id.surfaceview);


        // Setup Touch Listner
        gestureDetector =
                new GestureDetector(
                        this,
                        new GestureDetector.SimpleOnGestureListener() {
                            @Override
                            public boolean onDown(MotionEvent motionEvent) {
                                return false;
                            }

                            @Override
                            public void onShowPress(MotionEvent motionEvent) {

                            }

                            @Override
                            public boolean onSingleTapUp(MotionEvent motionEvent) {

                                ARActivity.this.runOnUiThread(() -> showOcclusionDialogIfNeeded());

                                if (transformode == transformModes.move)
                                    surfaceView.queueEvent(
                                            () -> JniInterface.onTouched(nativeApplication, motionEvent.getX(), motionEvent.getY()));
                                return true;
                            }

                            @Override
                            public boolean onScroll(MotionEvent motionEvent, MotionEvent motionEvent1, float v, float v1) {
                                //float xDistance = motionEvent.getX() - motionEvent1.getX();
                                //float yDistance = motionEvent.getY() - motionEvent1.getY();
                                if (transformode == transformModes.move)
                                    surfaceView.queueEvent(
                                            () -> JniInterface.onDrag(nativeApplication, v, v1));
                                else if (transformode == transformModes.rotate)
                                    surfaceView.queueEvent(
                                            () -> JniInterface.onRotate(nativeApplication, v, v1));
                                return true;
                            }

                            @Override
                            public void onLongPress(MotionEvent motionEvent) {

                            }

                            @Override
                            public boolean onFling(MotionEvent motionEvent, MotionEvent motionEvent1, float v, float v1) {
                                return false;
                            }
                        }
                );

        scaleGestureDetector = new ScaleGestureDetector(this, new ScaleListner());

        surfaceView.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent) {
                gestureDetector.onTouchEvent(motionEvent);
                scaleGestureDetector.onTouchEvent(motionEvent);
                return true;
            }
        });

        //surfaceView.setOnTouchListener((View v, MotionEvent event) -> scaleGestureDetector.onTouchEvent(event));

        // Set up renderer.
        surfaceView.setPreserveEGLContextOnPause(true);
        surfaceView.setEGLContextClientVersion(2);
        surfaceView.setEGLConfigChooser(8, 8, 8, 8, 16, 0); // Alpha used for plane blending.
        surfaceView.setRenderer(this);
        surfaceView.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);
        surfaceView.setWillNotDraw(false);

        JniInterface.assetManager = getAssets();
        nativeApplication = JniInterface.createNativeApplication(getAssets());

        planeStatusCheckingHandler = new Handler();

        depthSettings.onCreate(this);
        instantPlacementSettings.onCreate(this);
        ImageButton settingsButton = findViewById(R.id.settings_button);
        settingsButton.setOnClickListener(
                new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        PopupMenu popup = new PopupMenu(ARActivity.this, v);
                        popup.setOnMenuItemClickListener(ARActivity.this::settingsMenuClick);
                        popup.inflate(R.menu.settings_menu);
                        popup.show();
                    }
                });

        ImageButton moveButton = findViewById(R.id.move_button);
        moveButton.setOnClickListener(
                new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        transformode = transformModes.move;
                    }
                }
        );

        ImageButton rotateButton = findViewById(R.id.rotate_button);
        rotateButton.setOnClickListener(
                new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        transformode = transformModes.rotate;
                    }
                }
        );

        ImageButton scaleButton = findViewById(R.id.scale_button);
        scaleButton.setOnClickListener(
                new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        transformode = transformModes.scale;
                    }
                }
        );

    }

    /** Menu button to launch feature specific settings. */
    protected boolean settingsMenuClick(MenuItem item) {
        if (item.getItemId() == R.id.depth_settings) {
            launchDepthSettingsMenuDialog();
            return true;
        } else if (item.getItemId() == R.id.instant_placement_settings) {
            launchInstantPlacementSettingsMenuDialog();
            return true;
        }
        return false;
    }

    @Override
    protected void onResume() {
        super.onResume();
        // ARCore requires camera permissions to operate. If we did not yet obtain runtime
        // permission on Android M and above, now is a good time to ask the user for it.
        if (!CameraPermissionHelper.hasCameraPermission(this)) {
            CameraPermissionHelper.requestCameraPermission(this);
            return;
        }

        try {
            JniInterface.onSettingsChange(
                    nativeApplication, instantPlacementSettings.isInstantPlacementEnabled());
            JniInterface.onResume(nativeApplication, getApplicationContext(), this);
            surfaceView.onResume();
        } catch (Exception e) {
            Log.e(TAG, "Exception creating session", e);
            displayInSnackbar(e.getMessage());
            return;
        }

        displayInSnackbar("Searching for surfaces...");
        planeStatusCheckingHandler.postDelayed(
                planeStatusCheckingRunnable, SNACKBAR_UPDATE_INTERVAL_MILLIS);

        // Listen to display changed events to detect 180° rotation, which does not cause a config
        // change or view resize.
        getSystemService(DisplayManager.class).registerDisplayListener(this, null);
    }

    @Override
    public void onPause() {
        super.onPause();
        surfaceView.onPause();
        JniInterface.onPause(nativeApplication);

        planeStatusCheckingHandler.removeCallbacks(planeStatusCheckingRunnable);

        getSystemService(DisplayManager.class).unregisterDisplayListener(this);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();

        // Synchronized to avoid racing onDrawFrame.
        synchronized (this) {
            JniInterface.destroyNativeApplication(nativeApplication);
            nativeApplication = 0;
        }
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        if (hasFocus) {
            // Standard Android full-screen functionality.
            getWindow()
                    .getDecorView()
                    .setSystemUiVisibility(
                            View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                                    | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                                    | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                                    | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                                    | View.SYSTEM_UI_FLAG_FULLSCREEN
                                    | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
            getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        }
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        GLES20.glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        JniInterface.onGlSurfaceCreated(nativeApplication);
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        viewportWidth = width;
        viewportHeight = height;
        viewportChanged = true;
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        // Synchronized to avoid racing onDestroy.
        synchronized (this) {
            if (nativeApplication == 0) {
                return;
            }
            if (viewportChanged) {
                int displayRotation = getWindowManager().getDefaultDisplay().getRotation();
                JniInterface.onDisplayGeometryChanged(
                        nativeApplication, displayRotation, viewportWidth, viewportHeight);
                viewportChanged = false;
            }
            JniInterface.onGlSurfaceDrawFrame(
                    nativeApplication,
                    depthSettings.depthColorVisualizationEnabled(),
                    depthSettings.useDepthForOcclusion());
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] results) {
        super.onRequestPermissionsResult(requestCode, permissions, results);
        if (!CameraPermissionHelper.hasCameraPermission(this)) {
            Toast.makeText(this, "Camera permission is needed to run this application", Toast.LENGTH_LONG)
                    .show();
            if (!CameraPermissionHelper.shouldShowRequestPermissionRationale(this)) {
                // Permission denied with checking "Do not ask again".
                CameraPermissionHelper.launchPermissionSettings(this);
            }
            finish();
        }
    }

    /**
     * Display the message in the snackbar.
     */
    private void displayInSnackbar(String message) {
        snackbar =
                Snackbar.make(
                        ARActivity.this.findViewById(android.R.id.content),
                        message, Snackbar.LENGTH_INDEFINITE);

        // Set the snackbar background to light transparent black color.
        snackbar.getView().setBackgroundColor(0xbf323232);
        snackbar.show();
    }

    /**
     * Shows a pop-up dialog on the first call, determining whether the user wants to enable
     * depth-based occlusion. The result of this dialog can be retrieved with useDepthForOcclusion().
     */
    private void showOcclusionDialogIfNeeded() {
        boolean isDepthSupported = JniInterface.isDepthSupported(nativeApplication);
        if (!depthSettings.shouldShowDepthEnableDialog() || !isDepthSupported) {
            return; // Don't need to show dialog.
        }

        // Asks the user whether they want to use depth-based occlusion.
        new AlertDialog.Builder(this)
                .setTitle(R.string.options_title_with_depth)
                .setMessage(R.string.depth_use_explanation)
                .setPositiveButton(
                        R.string.button_text_enable_depth,
                        (DialogInterface dialog, int which) -> {
                            depthSettings.setUseDepthForOcclusion(true);
                        })
                .setNegativeButton(
                        R.string.button_text_disable_depth,
                        (DialogInterface dialog, int which) -> {
                            depthSettings.setUseDepthForOcclusion(false);
                        })
                .show();
    }

    private void launchInstantPlacementSettingsMenuDialog() {
        resetSettingsMenuDialogCheckboxes();
        Resources resources = getResources();
        new AlertDialog.Builder(this)
                .setTitle(R.string.options_title_instant_placement)
                .setMultiChoiceItems(
                        resources.getStringArray(R.array.instant_placement_options_array),
                        instantPlacementSettingsMenuDialogCheckboxes,
                        (DialogInterface dialog, int which, boolean isChecked) ->
                                instantPlacementSettingsMenuDialogCheckboxes[which] = isChecked)
                .setPositiveButton(
                        R.string.done,
                        (DialogInterface dialogInterface, int which) -> applySettingsMenuDialogCheckboxes())
                .setNegativeButton(
                        android.R.string.cancel,
                        (DialogInterface dialog, int which) -> resetSettingsMenuDialogCheckboxes())
                .show();
    }

    /** Shows checkboxes to the user to facilitate toggling of depth-based effects. */
    private void launchDepthSettingsMenuDialog() {
        // Retrieves the current settings to show in the checkboxes.
        resetSettingsMenuDialogCheckboxes();

        // Shows the dialog to the user.
        Resources resources = getResources();
        boolean isDepthSupported = JniInterface.isDepthSupported(nativeApplication);
        if (isDepthSupported) {
            // With depth support, the user can select visualization options.
            new AlertDialog.Builder(this)
                    .setTitle(R.string.options_title_with_depth)
                    .setMultiChoiceItems(
                            resources.getStringArray(R.array.depth_options_array),
                            depthSettingsMenuDialogCheckboxes,
                            (DialogInterface dialog, int which, boolean isChecked) ->
                                    depthSettingsMenuDialogCheckboxes[which] = isChecked)
                    .setPositiveButton(
                            R.string.done,
                            (DialogInterface dialogInterface, int which) -> applySettingsMenuDialogCheckboxes())
                    .setNegativeButton(
                            android.R.string.cancel,
                            (DialogInterface dialog, int which) -> resetSettingsMenuDialogCheckboxes())
                    .show();
        } else {
            // Without depth support, no settings are available.
            new AlertDialog.Builder(this)
                    .setTitle(R.string.options_title_without_depth)
                    .setPositiveButton(
                            R.string.done,
                            (DialogInterface dialogInterface, int which) -> applySettingsMenuDialogCheckboxes())
                    .show();
        }
    }

    private void applySettingsMenuDialogCheckboxes() {
        depthSettings.setUseDepthForOcclusion(depthSettingsMenuDialogCheckboxes[0]);
        depthSettings.setDepthColorVisualizationEnabled(depthSettingsMenuDialogCheckboxes[1]);
        instantPlacementSettings.setInstantPlacementEnabled(
                instantPlacementSettingsMenuDialogCheckboxes[0]);

        JniInterface.onSettingsChange(
                nativeApplication, instantPlacementSettings.isInstantPlacementEnabled());
    }

    private void resetSettingsMenuDialogCheckboxes() {
        depthSettingsMenuDialogCheckboxes[0] = depthSettings.useDepthForOcclusion();
        depthSettingsMenuDialogCheckboxes[1] = depthSettings.depthColorVisualizationEnabled();
        instantPlacementSettingsMenuDialogCheckboxes[0] =
                instantPlacementSettings.isInstantPlacementEnabled();
    }

    // DisplayListener methods
    @Override
    public void onDisplayAdded(int displayId) {}

    @Override
    public void onDisplayRemoved(int displayId) {}

    @Override
    public void onDisplayChanged(int displayId) {
        viewportChanged = true;
    }

    @Override
    public boolean onTouchEvent(MotionEvent me) {
        gestureDetector.onTouchEvent(me);
        scaleGestureDetector.onTouchEvent(me);
        return true;
    }

    private class ScaleListner
            extends ScaleGestureDetector.SimpleOnScaleGestureListener {
        @Override
        public boolean onScale(ScaleGestureDetector detector) {
            if (transformode == transformModes.scale) {
                scaleFactor *= detector.getScaleFactor();
                scaleFactor = Math.max(0.1f, Math.min(scaleFactor, 5.0f));
                JniInterface.onScale(nativeApplication, scaleFactor);
            }
            return true;
        }
    }
}