package cz.ger.ffng;

import android.app.Activity;
import android.content.Context;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.os.PowerManager;
import android.os.PowerManager.WakeLock;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;

import com.badlogic.gdx.backends.android.AndroidFiles;

public class FFNG extends Activity {
	private FFNGApp app;
	public static AndroidFiles files = null;
	public static AssetManager assets = null;
	public static String storageDir = "";

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        assets = getAssets();
        files = new AndroidFiles(assets);
        storageDir = getExternalFilesDir(null).getPath();

        // turn off the window's title bar
        requestWindowFeature(Window.FEATURE_NO_TITLE);

        int flags = WindowManager.LayoutParams.FLAG_FULLSCREEN
                | WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON;
        getWindow().setFlags(flags, flags);

        app = new FFNGApp(this);
        setContentView(app.getView());
        app.start();
    }

    @Override
    protected void onPause() {
    	super.onPause();
    	app.pauseRequest();
    }
    
    @Override
    protected void onResume() {
        super.onResume();
        app.resumeRequest();
    }
    
    @Override
    protected void onStop() {
        super.onStop();
    }
    
    protected void onDestroy() {
    	super.onDestroy();
        app.disposeRequest();
    }
    
    @Override
    public void onSaveInstanceState(Bundle outState) {
    	
    } 
}