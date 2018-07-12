package cz.ger.ffng;

import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.Typeface;
import android.graphics.Paint.Align;

public class FFNGFont {
    private Paint fillPaint;
    private Paint outlinePaint;
    private Typeface face;
    private Rect bounds = new Rect();

    FFNGFont(String file, int size) {
    	face = Typeface.createFromAsset(FFNG.assets, file);
		fillPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
    	fillPaint.setTextAlign(Align.LEFT);
    	fillPaint.setTypeface(face);
    	fillPaint.setTextSize(size);     // TODO convert size(mm) to pixelsize(dpi?)

		outlinePaint = new Paint(fillPaint);
		outlinePaint.setStyle(Paint.Style.STROKE);
		outlinePaint.setColor(Color.BLACK);
    }
    
    public static FFNGFont createFont(String file, int size) {
    	return new FFNGFont(file, size);
    }
    
    public int getWidth(String text) {
    	synchronized(bounds) {
    		bounds = getBounds(text);
	    	return bounds.width();
    	}
    }
    
    synchronized public int getHeight(String text) {
    	synchronized(bounds) {
	    	bounds = getBounds(text);
	    	return bounds.height();
    	}
    }
    
    synchronized public Rect getBounds(String text) {
    	fillPaint.getTextBounds(text, 0, text.length(), bounds);
    	return bounds;
    }
    
    public void render(Canvas canvas, String text, int frontColor, int bgColor, int outlineWidth) {
    	bounds = getBounds(text);
		if (outlineWidth > 0) {
			outlinePaint.setStrokeWidth(2*outlineWidth);
			canvas.drawText(text, -bounds.left + outlineWidth, -bounds.top + outlineWidth, outlinePaint);
		}
    	fillPaint.setColor(frontColor);
    	canvas.drawText(text, -bounds.left + outlineWidth, -bounds.top + outlineWidth, fillPaint);
    }
}
