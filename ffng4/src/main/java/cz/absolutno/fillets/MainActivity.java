package cz.absolutno.fillets;

import android.app.NativeActivity;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.WindowInsets;
import android.view.WindowManager;

import java.io.IOException;
import java.io.InputStream;

public class MainActivity extends NativeActivity {
    private static final String TAG = "FFNG4";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if (Build.VERSION.SDK_INT >= 30) {
            getWindow().getInsetsController().hide(
                    WindowInsets.Type.statusBars() | WindowInsets.Type.navigationBars());
        } else {
            getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        }

        /*Intent intent = new Intent(this, VideoActivity.class);
        startActivity(intent);*/
    }

    Bitmap loadBitmap(String filename) {
        AssetManager assets = getAssets();
        try {
            InputStream is = assets.open(filename);
            return BitmapFactory.decodeStream(is);
        } catch (IOException e) {
            Log.e(TAG, "asset not found: " + filename);
            return null;
        }
    }
}