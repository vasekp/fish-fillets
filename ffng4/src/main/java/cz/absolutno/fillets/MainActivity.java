package cz.absolutno.fillets;

import android.app.NativeActivity;
import android.content.Intent;
import android.content.res.AssetManager;
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
import android.view.WindowInsets;
import android.view.WindowInsetsController;
import android.view.WindowManager;

import java.io.IOException;
import java.io.InputStream;

public class MainActivity extends NativeActivity {
    private static final String TAG = "FFNG4";

    static {
        System.loadLibrary("fillets");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if (Build.VERSION.SDK_INT >= 30) {
            getWindow().getInsetsController().hide(
                    WindowInsets.Type.statusBars() | WindowInsets.Type.navigationBars());
            getWindow().getInsetsController().setSystemBarsBehavior(
                    WindowInsetsController.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE);
        } else {
            getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
        }
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
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

    void playIntro() {
        Intent intent = new Intent(this, VideoActivity.class);
        startActivity(intent);
    }

    private TextPaint fillPaint;
    private TextPaint outlinePaint;
    private Typeface face;

    Bitmap text(String file, String text, int size) {
        Log.d("FFNG", "density " + String.valueOf(getResources().getDisplayMetrics().density));
        Log.d("FFNG", "densityDpi " + String.valueOf(getResources().getDisplayMetrics().densityDpi));
        face = Typeface.createFromAsset(getAssets(), file);
        fillPaint = new TextPaint(Paint.ANTI_ALIAS_FLAG);
        //fillPaint.setTextAlign(Paint.Align.LEFT);
        fillPaint.setTypeface(face);
        fillPaint.setColor(Color.WHITE);
        fillPaint.setTextSize(size * getResources().getDisplayMetrics().density);

        outlinePaint = new TextPaint(fillPaint);
        outlinePaint.setStyle(Paint.Style.STROKE);
        outlinePaint.setColor(Color.BLACK);

        int outlineWidth = 2;
        int width = 640 - 2 * outlineWidth;
        Layout.Alignment alignment = Layout.Alignment.ALIGN_NORMAL;
        float spacingMultiplier = 1;
        float spacingAddition = 0;
        boolean includePadding = false;

        Paint.FontMetrics fm = fillPaint.getFontMetrics();
        float height = (fm.descent - fm.ascent) * 1.2f;

        StaticLayout myStaticLayout = new StaticLayout(text, outlinePaint, width, alignment, spacingMultiplier, spacingAddition, includePadding);
        int lines = myStaticLayout.getLineCount();

        Bitmap bmp = Bitmap.createBitmap(640, (int)(height * lines), Bitmap.Config.ARGB_8888);
        Canvas cvs = new Canvas(bmp);
        Log.d("FFNG", String.valueOf(myStaticLayout.getLineCount()) + " " + String.valueOf(myStaticLayout.getHeight()));

        int i;

        for(i = 0; i < lines; i++) {
            String line = text.substring(myStaticLayout.getLineStart(i), myStaticLayout.getLineStart(i + 1));
            Rect rect = new Rect();
            fillPaint.getTextBounds(line, 0, line.length(), rect);
            Log.d("FFNG", line);
            //Log.d("FFNG", String.valueOf(rect.left) + " " + String.valueOf(rect.top));

            outlinePaint.setStrokeWidth(2 * outlineWidth);
            float x = (640.f - (rect.right - rect.left)) / 2.f;
            float y = -fm.ascent + i * height + outlineWidth;
            cvs.drawText(line, x, y, outlinePaint);
            cvs.drawText(line, x, y, fillPaint);
        }
        return bmp;
    }
}