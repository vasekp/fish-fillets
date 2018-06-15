package cz.ger.ffng;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;

public class FFNGSurface {

/*  private static void checkEglError(String msg) {
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
    }

    public void line(int x1, int y1, int x2, int y2, int color) {
        final float[] coords = {
                (float)x1 / width, (float)y1 / height,
                (float)x2 / width, (float)y2 / height,
        };
        FloatBuffer buffer = ByteBuffer.allocateDirect(coords.length * FLOAT_SIZE).order(ByteOrder.nativeOrder()).asFloatBuffer();
        buffer.put(coords);

        GLES20.glUseProgram(programUniform);

        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, framebuffer);
        GLES20.glFramebufferTexture2D(GLES20.GL_FRAMEBUFFER, GLES20.GL_COLOR_ATTACHMENT0, GLES20.GL_TEXTURE_2D, texture, 0);
        GLES20.glViewport(0, 0, width, height);

        int uColor = GLES20.glGetUniformLocation(programUniform, "uColor");
        GLES20.glUniform4f(uColor, ((color >> 16) & 255)/255.0f, ((color >> 8) & 255)/255.0f, ((color >> 0) & 255)/255.0f, ((color >> 24) & 255)/255.0f);

        int aPosition = GLES20.glGetAttribLocation(programUniform, "aPosition");
        buffer.rewind();
        GLES20.glVertexAttribPointer(aPosition, 2, GLES20.GL_FLOAT, false, 2 * FLOAT_SIZE, buffer);
        GLES20.glEnableVertexAttribArray(aPosition);

        GLES20.glDrawArrays(GLES20.GL_LINES, 0, 2);
    }

    public void fillRect(int x, int y, int w, int h, int color) {
        GLES20.glUseProgram(programUniform);

        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, framebuffer);
        GLES20.glFramebufferTexture2D(GLES20.GL_FRAMEBUFFER, GLES20.GL_COLOR_ATTACHMENT0, GLES20.GL_TEXTURE_2D, texture, 0);
        GLES20.glViewport(x, y, w, h);

        int uColor = GLES20.glGetUniformLocation(programUniform, "uColor");
        GLES20.glUniform4f(uColor, ((color >> 16) & 255)/255.0f, ((color >> 8) & 255)/255.0f, ((color >> 0) & 255)/255.0f, ((color >> 24) & 255)/255.0f);

        int aPosition = GLES20.glGetAttribLocation(programUniform, "aPosition");
        buffer.rewind();
        GLES20.glVertexAttribPointer(aPosition, 2, GLES20.GL_FLOAT, false, 2 * FLOAT_SIZE, buffer);
        GLES20.glEnableVertexAttribArray(aPosition);

        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4);
    }

    public void filledCircle(int x, int y, int radius, int color) {
        GLES20.glUseProgram(programCircle);

        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, framebuffer);
        GLES20.glFramebufferTexture2D(GLES20.GL_FRAMEBUFFER, GLES20.GL_COLOR_ATTACHMENT0, GLES20.GL_TEXTURE_2D, texture, 0);
        GLES20.glViewport(x - radius, y - radius, 2*radius, 2*radius);

        int uColor = GLES20.glGetUniformLocation(programCircle, "uColor");
        GLES20.glUniform4f(uColor, ((color >> 16) & 255)/255.0f, ((color >> 8) & 255)/255.0f, ((color >> 0) & 255)/255.0f, ((color >> 24) & 255)/255.0f);

        int aPosition = GLES20.glGetAttribLocation(programCircle, "aPosition");
        buffer.rewind();
        GLES20.glVertexAttribPointer(aPosition, 2, GLES20.GL_FLOAT, false, 2 * FLOAT_SIZE, buffer);
        GLES20.glEnableVertexAttribArray(aPosition);

        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4);
    }*/
}
