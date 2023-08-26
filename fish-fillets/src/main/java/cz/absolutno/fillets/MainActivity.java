package cz.absolutno.fillets;

import android.app.Activity;
import android.content.res.AssetManager;
import android.content.res.Configuration;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.Typeface;
import android.os.Build;
import android.os.Bundle;
import android.text.Layout;
import android.text.StaticLayout;
import android.text.TextPaint;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowInsets;
import android.view.WindowInsetsController;
import android.view.WindowManager;

import java.io.IOException;
import java.io.InputStream;
import java.util.HashMap;
import java.util.Map;

public class MainActivity extends Activity implements SurfaceHolder.Callback, View.OnHoverListener {
    private static final String TAG = "Fillets";
    private final Map<String, Typeface> fontMap = new HashMap<>();
    private int libHandle;

    static {
        System.loadLibrary("fillets");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        SurfaceView view = new SurfaceView(this);
        view.setLayoutParams(new ViewGroup.MarginLayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
        setContentView(view);
        if (Build.VERSION.SDK_INT >= 30) {
            if(!isInMultiWindowMode()) {
                WindowInsetsController insetsController = getWindow().getInsetsController();
                insetsController.hide(WindowInsets.Type.statusBars() | WindowInsets.Type.navigationBars());
                insetsController.setSystemBarsBehavior(WindowInsetsController.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE);
            }
        } else {
            getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
        }

        //getWindow().addFlags(WindowManager.LayoutParams.FLAG_LAYOUT_NO_LIMITS);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        libHandle = startApp(getAssets(), getExternalFilesDir(null).getAbsolutePath());

        view.getHolder().addCallback(this);
        view.setOnHoverListener(this);

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        quitApp(libHandle);
    }

    @Override
    protected void onResume() {
        super.onResume();
        setForeground(libHandle, true);
    }

    @Override
    protected void onPause() {
        super.onPause();
        setForeground(libHandle, false);
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        setForeground(libHandle, hasFocus);
    }

    public boolean onTouchEvent(MotionEvent event) {
        Log.d("Touch", event.toString());
        if(event.getPointerCount() == 1)
            return pointerAction(libHandle, event.getAction(), event.getX(), event.getY());
        else if(event.getPointerCount() == 2 && event.getActionMasked() == MotionEvent.ACTION_POINTER_DOWN)
            return pointerAction(libHandle, event.getActionMasked(), 0, 0);
        else
            return false;
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        return keyAction(libHandle, KeyEvent.ACTION_DOWN, event.getKeyCode());
    }

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        return keyAction(libHandle, KeyEvent.ACTION_UP, event.getKeyCode());
    }

    @Override
    public boolean onHover(View view, MotionEvent event) {
        return pointerAction(libHandle, event.getAction(), event.getX(), event.getY());
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        Log.d(TAG, "surfaceCreated (Java)");
        setSurface(libHandle, holder.getSurface());
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        Log.d(TAG, "surfaceChanged (Java): " + format + " " + width + "x" + height);
        setSurface(libHandle, holder.getSurface());
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        Log.d(TAG, "surfaceDestroyed (Java)");
        setSurface(libHandle, null);
    }

    @Override
    public void onMultiWindowModeChanged(boolean isInMultiWindowMode, Configuration newConfig) {
        super.onMultiWindowModeChanged(isInMultiWindowMode, newConfig);
        Log.d(TAG, "onmwmc");
        if(isInMultiWindowMode) {
            if (Build.VERSION.SDK_INT >= 30) {
                WindowInsetsController insetsController = getWindow().getInsetsController();
                insetsController.show(WindowInsets.Type.statusBars() | WindowInsets.Type.navigationBars());
            } else {
                getWindow().clearFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
            }
        } else {
            if (Build.VERSION.SDK_INT >= 30) {
                WindowInsetsController insetsController = getWindow().getInsetsController();
                insetsController.hide(WindowInsets.Type.statusBars() | WindowInsets.Type.navigationBars());
                insetsController.setSystemBarsBehavior(WindowInsetsController.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE);
            } else {
                getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
            }
        }
    }

    Bitmap loadBitmap(String filename) {
        Log.d(TAG, "load " + filename);
        AssetManager assets = getAssets();
        try {
            InputStream is = assets.open(filename);
            return BitmapFactory.decodeStream(is);
        } catch (IOException e) {
            Log.e(TAG, "asset not found: " + filename);
            return null;
        }
    }

    Typeface typeface(String fontFile) {
        if (fontMap.containsKey(fontFile))
            return fontMap.get(fontFile);
        else {
            Typeface face = Typeface.createFromAsset(getAssets(), fontFile);
            fontMap.put(fontFile, face);
            return face;
        }
    }

    String[] breakLines(String text, String fontFile, float fontSize, int screenWidth) {
        Typeface face = typeface(fontFile);

        TextPaint fillPaint = new TextPaint(Paint.ANTI_ALIAS_FLAG);
        fillPaint.setColor(Color.WHITE);
        fillPaint.setTypeface(face);
        fillPaint.setTextSize(fontSize);

        Layout.Alignment align = Layout.Alignment.ALIGN_NORMAL;
        StaticLayout layout = new StaticLayout(text, fillPaint, screenWidth, align, 1, 0, false);
        int lineCount = layout.getLineCount();

        String[] ret = new String[lineCount];
        for (int i = 0; i < lineCount; i++)
            ret[i] = text.substring(layout.getLineStart(i), layout.getLineStart(i + 1));
        return ret;
    }

    Bitmap renderText(String text, String fontFile, float fontSize, float outline) {
        Typeface face = typeface(fontFile);

        TextPaint fillPaint = new TextPaint(Paint.ANTI_ALIAS_FLAG);
        fillPaint.setColor(Color.WHITE);
        fillPaint.setTypeface(face);
        fillPaint.setTextSize(fontSize);

        TextPaint outlinePaint = new TextPaint(fillPaint);
        outlinePaint.setStyle(Paint.Style.STROKE);
        outlinePaint.setColor(Color.BLACK);
        outlinePaint.setStrokeWidth(2.f * outline);

        Paint.FontMetrics fm = fillPaint.getFontMetrics();
        float top = Math.max(-fm.top, -fm.ascent);
        float bottom = Math.max(fm.bottom, fm.descent);
        float lineHeight = top + bottom;
        Rect bounds = new Rect();
        fillPaint.getTextBounds(text, 0, text.length(), bounds);
        Log.d(TAG, "fontSize " + fontSize + " top " + top + " bottom " + bottom + " bitmap size " + bounds.width() + "x" + bounds.height());

        Bitmap bitmap = Bitmap.createBitmap((int) (bounds.width() + 2 * outline) + 2, (int) (lineHeight + 2 * outline) + 2, Bitmap.Config.ARGB_8888);
        Canvas canvas = new Canvas(bitmap);
        float x = -bounds.left + outline + 1.f;
        float y = top + outline + 1.f;
        if (outline != 0.0)
            canvas.drawText(text, x, y, outlinePaint);
        canvas.drawText(text, x, y, fillPaint);
        return bitmap;
    }

    String getLang() {
        return getResources().getConfiguration().locale.getLanguage();
    }

    private native int startApp(AssetManager amgr, String userPath);
    private native void quitApp(int handle);
    private native void setForeground(int handle, boolean focus);
    private native void setSurface(int handle, Surface surface);
    private native boolean pointerAction(int handle, int action, float x, float y);
    private native boolean keyAction(int handle, int action, int keyCode);
}
