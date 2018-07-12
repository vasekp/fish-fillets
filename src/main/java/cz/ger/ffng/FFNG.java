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

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        assets = getAssets();
        files = new AndroidFiles(assets);

        FFNGFiles.createCache();
        
        // turn off the window's title bar
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, 
                       WindowManager.LayoutParams.FLAG_FULLSCREEN);

        app = new FFNGApp();
        
        View view = new FFNGApp.FFNGView(this, app);
        setContentView(view);
        app.setContext(view, this);
        
        app.start();
    }

    public FFNGApp getApp() {
    	return app;
    }
    
    @Override
    protected void onPause() {
    	super.onPause();
    	app.pauseRequest();
    	releaseWakeLock();
    }
    
    @Override
    protected void onResume() {
        super.onResume();
        app.resumeRequest();
        acquireWakeLock();
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
    
    private WakeLock wakeLock;
	private void acquireWakeLock() {
		final PowerManager pm = (PowerManager) this.getSystemService(Context.POWER_SERVICE);
		wakeLock = pm.newWakeLock(PowerManager.SCREEN_BRIGHT_WAKE_LOCK | PowerManager.ON_AFTER_RELEASE, "FFNG");
		wakeLock.acquire();
	}

	private void releaseWakeLock() {
		if(wakeLock != null) {
			wakeLock.release();
		}
	}
}