package cz.ger.ffng;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;

public class FFNGSurface {

    /*private static void checkEglError(String msg) {
        int error;
        if ((error = egl.eglGetError()) != EGL10.EGL_SUCCESS) {
            throw new RuntimeException(msg + ": EGL error: 0x" + Integer.toHexString(error));
        }
    }*/

    static Bitmap loadBitmap(String file) {
        BitmapFactory.Options options = new BitmapFactory.Options(); // TODO static
        options.inPreferredConfig = Bitmap.Config.ARGB_8888;
        return BitmapFactory.decodeStream(FFNG.files.internal(file).read(), null, options);
    }

    /*public static FFNGSurface newSurface(FFNGFont font, String text, int frontColor, int bgColor, int outlineWidth) {
        Rect bounds = font.getBounds(text);
        Bitmap bmp = Bitmap.createBitmap(bounds.width() + 2 * outlineWidth, bounds.height() + 2 * outlineWidth, Bitmap.Config.ARGB_8888);
        Canvas canvas = new Canvas(bmp);
        font.render(canvas, text, frontColor, bgColor, outlineWidth);
        FFNGSurface surface = new FFNGSurface(bmp);
        bmp.recycle();
        return surface;
    }*/
}
