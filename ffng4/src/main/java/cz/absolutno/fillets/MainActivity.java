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
import java.util.HashMap;
import java.util.Map;

public class MainActivity extends NativeActivity {
    private static final String TAG = "FFNG4";

    private final Map<String, Typeface> fontMap = new HashMap<>();

    static {
        System.loadLibrary("fillets");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if (Build.VERSION.SDK_INT >= 30) {
            getWindow().getInsetsController().hide(WindowInsets.Type.statusBars());
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

    Typeface typeface(String fontFile) {
        if(fontMap.containsKey(fontFile))
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
        for(int i = 0; i < lineCount; i++)
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

        Bitmap bitmap = Bitmap.createBitmap((int)(bounds.width() + 2 * outline) + 2, (int) (lineHeight + 2 * outline) + 2, Bitmap.Config.ARGB_8888);
        Canvas canvas = new Canvas(bitmap);
        float x = -bounds.left + outline + 1.f;
        float y = top + outline + 1.f;
        if(outline != 0.0)
            canvas.drawText(text, x, y, outlinePaint);
        canvas.drawText(text, x, y, fillPaint);
        return bitmap;
    }
}