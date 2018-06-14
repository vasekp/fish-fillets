package cz.ger.ffng;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Rect;
import android.opengl.GLES20;
import android.opengl.GLUtils;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.egl.EGLSurface;

public class FFNGSurface {

    //private int width;
    //private int height;
    //private final int texture;
    //private static final int framebuffer;
    private static final int FLOAT_SIZE = 4;
    /*private static final int programCopy;
    private static final int programUniform;
    private static final int programScaled;
    private static final int programMasked;
    private static final int programWavy;
    private static final int programDisintegrate;
    private static final int programMirror;
    private static final int programCircle;
    private static final int programZX;
    private static final FloatBuffer buffer;

    static final EGL10 egl;
    static final EGLDisplay dpy;
    static final EGLSurface sfc;*/

    /*static {
        egl = (EGL10) EGLContext.getEGL();
        dpy = egl.eglGetDisplay(EGL10.EGL_DEFAULT_DISPLAY);
        if (dpy == EGL10.EGL_NO_DISPLAY) {
            throw new RuntimeException("unable to get EGL10 display");
        }
        final int EGL_OPENGL_ES2_BIT = 0x0004;
        final int EGL_CONTEXT_CLIENT_VERSION = 0x3098;
        int attribList[] = {
                EGL10.EGL_RED_SIZE, 8,
                EGL10.EGL_GREEN_SIZE, 8,
                EGL10.EGL_BLUE_SIZE, 8,
                EGL10.EGL_ALPHA_SIZE, 8,
                EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                EGL10.EGL_SURFACE_TYPE, EGL10.EGL_PBUFFER_BIT,
                EGL10.EGL_NONE
        };
        int[] version = new int[2];
        if (!egl.eglInitialize(dpy, version)) {
            throw new RuntimeException("unable to initialize EGL10");
        }
        EGLConfig[] configs = new EGLConfig[1];
        int[] numConfigs = new int[1];
        if (!egl.eglChooseConfig(dpy, attribList, configs, configs.length,
                numConfigs)) {
            throw new RuntimeException("unable to find RGB888+recordable ES2 EGL config");
        }
        int[] attrib_list = {
                EGL_CONTEXT_CLIENT_VERSION, 2,
                EGL10.EGL_NONE
        };
        EGLContext ctx = egl.eglCreateContext(dpy, configs[0], EGL10.EGL_NO_CONTEXT,
                attrib_list);
        checkEglError("eglCreateContext");
        if (ctx == null) {
            throw new RuntimeException("null context");
        }

        // Create a pbuffer surface.
        int[] surfaceAttribs = {
                EGL10.EGL_WIDTH, 900,
                EGL10.EGL_HEIGHT, 900,
                EGL10.EGL_NONE
        };
        sfc = egl.eglCreatePbufferSurface(dpy, configs[0], surfaceAttribs);
        checkEglError("eglCreatePbufferSurface");
        if (sfc == null) {
            throw new RuntimeException("surface was null");
        }
        if (!egl.eglMakeCurrent(dpy, sfc, sfc, ctx)) {
            throw new RuntimeException("eglMakeCurrent failed");
        }

        GLES20.glClearColor(0f, 0f, 0f, 1f);
        GLES20.glBlendFuncSeparate(GLES20.GL_SRC_ALPHA, GLES20.GL_ONE_MINUS_SRC_ALPHA, GLES20.GL_ONE, GLES20.GL_ONE);
        GLES20.glEnable(GLES20.GL_BLEND);

        String vertexSrcCopy =
                "attribute vec2 aPosition;\n" +
                "varying vec2 vTexPos;\n" +
                "mat4 fxd = mat4(2.0, 0.0, 0.0, 0.0,  0.0, 2.0, 0.0, 0.0,  0.0, 0.0, 1.0, 0.0,  -1.0, -1.0, 0.0, 1.0);\n" +
                "void main() {\n" +
                "  vTexPos = aPosition;\n" +
                "  gl_Position = fxd * vec4(aPosition.xy, 0.0, 1.0);\n" +
                "}";

        String vertexSrcScaled =
                "attribute vec2 aPosition;\n" +
                "uniform vec2 uScale;\n" +
                "uniform vec2 uOffset;\n" +
                "varying vec2 vTexPos;\n" +
                "mat4 fxd = mat4(2.0, 0.0, 0.0, 0.0,  0.0, 2.0, 0.0, 0.0,  0.0, 0.0, 1.0, 0.0,  -1.0, -1.0, 0.0, 1.0);\n" +
                "void main() {\n" +
                "  vTexPos = uScale * aPosition + uOffset;\n" +
                "  gl_Position = fxd * vec4(aPosition.xy, 0.0, 1.0);\n" +
                "}";

        String vertexSrcMirror =
                "attribute vec2 aPosition;\n" +
                "uniform vec2 uScale;\n" +
                "uniform vec3 uOffset;\n" +
                "varying vec2 vTexPos;\n" +
                "varying vec2 vMirrorTgtTexPos;\n" +
                "mat4 fxd = mat4(2.0, 0.0, 0.0, 0.0,  0.0, 2.0, 0.0, 0.0,  0.0, 0.0, 1.0, 0.0,  -1.0, -1.0, 0.0, 1.0);\n" +
                "void main() {\n" +
                "  vTexPos = aPosition;\n" +
                "  vMirrorTgtTexPos = vec2(-1.0, 1.0) * uScale * aPosition.xy + uOffset.xy + vec2(uOffset.z, 0.0);\n" +
                "  gl_Position = fxd * vec4(uScale * aPosition.xy + uOffset.xy, 0.0, 1.0);\n" +
                "}";


        String fragmentSrcCopy =
                "precision mediump float;\n" +
                "uniform sampler2D uSrcTexture;\n" +
                "varying vec2 vTexPos;\n" +
                "void main(void)\n" +
                "{\n" +
                "  gl_FragColor = texture2D(uSrcTexture, vTexPos);\n" +
                "}";

        String fragmentSrcUniform =
                "precision mediump float;\n" +
                "uniform vec4 uColor;\n" +
                "void main(void)\n" +
                "{\n" +
                "  gl_FragColor = uColor;\n" +
                "}";

        String fragmentSrcMasked =
                "precision mediump float;\n" +
                "uniform sampler2D uSrcTexture;\n" +
                "uniform sampler2D uMaskTexture;\n" +
                "uniform vec4 uMaskColor;\n" +
                "varying vec2 vTexPos;\n" +
                "void main(void)\n" +
                "{\n" +
                "  gl_FragColor = distance(texture2D(uMaskTexture, vTexPos), uMaskColor) < 0.1 ? texture2D(uSrcTexture, vTexPos) : vec4(0.0);\n" +
                "}";

        String fragmentSrcWavy =
                "precision mediump float;\n" +
                "uniform sampler2D uSrcTexture;\n" +
                "uniform float uAmplitude;\n" +
                "uniform float uShift;\n" +
                "uniform float uPeriod;\n" +
                "varying vec2 vTexPos;\n" +
                "void main(void)\n" +
                "{\n" +
                "  gl_FragColor = texture2D(uSrcTexture, fract(vTexPos + uAmplitude * vec2(sin(vTexPos.y / uPeriod + uShift), 0.0)));\n" +
                "}";

        String fragmentSrcDisintegrate =
                "precision mediump float;\n" +
                "uniform sampler2D uSrcTexture;\n" +
                "uniform int uWidth;\n" +
                "uniform int uHeight;\n" +
                "uniform int uPhase;\n" +
                "varying vec2 vTexPos;\n" +
                "float noise(int x, int y) {\n" +
                "  return fract(sin(mod(float(x) + 1.618 * float(y), 6.28)) * 100.0);\n" +
                "}\n" +
                "void main(void)\n" +
                "{\n" +
                "  gl_FragColor = noise(int(vTexPos.x * float(uWidth)), int(vTexPos.y * float(uHeight))) < float(uPhase) / 255.0 ? texture2D(uSrcTexture, vTexPos) : vec4(0.0);\n" +
                "}";

        String fragmentSrcMirror =
                "precision mediump float;\n" +
                "uniform sampler2D uSrcTexture;\n" +
                "uniform sampler2D uTgtTexture;\n" +
                "uniform vec4 uMaskColor;\n" +
                "varying vec2 vTexPos;\n" +
                "varying vec2 vMirrorTgtTexPos;\n" +
                "void main(void)\n" +
                "{\n" +
                "  gl_FragColor = distance(texture2D(uSrcTexture, vTexPos), uMaskColor) < 0.1 ? texture2D(uTgtTexture, vMirrorTgtTexPos) : texture2D(uSrcTexture, vTexPos);\n" +
                "}";

        String fragmentSrcCircle =
                "precision mediump float;\n" +
                "varying vec2 vTexPos;\n" +
                "uniform vec4 uColor;\n" +
                "void main(void)\n" +
                "{\n" +
                "  gl_FragColor = distance(vTexPos, vec2(0.5, 0.5)) < 0.5 ? uColor : vec4(0.0);\n" +
                "}";


        String fragmentSrcZX =
                "precision mediump float;\n" +
                "uniform sampler2D uSrcTexture;\n" +
                "uniform vec4 uMaskColor;\n" +
                "uniform vec4 uColors[2];\n" +
                "uniform float uStartY;\n" +
                "uniform float uPeriodY;\n" +
                "varying vec2 vTexPos;\n" +
                "void main(void)\n" +
                "{\n" +
                "  gl_FragColor = distance(texture2D(uSrcTexture, vTexPos), uMaskColor) < 0.1\n" +
                "    ? (fract((vTexPos.y + uStartY) / uPeriodY) < 0.5 ? uColors[0] : uColors[1])\n" +
                "    : vec4(0.0);\n" +
                "}";

        int vertexCopy = loadShader(GLES20.GL_VERTEX_SHADER, vertexSrcCopy);
        int vertexScaled = loadShader(GLES20.GL_VERTEX_SHADER, vertexSrcScaled);
        int vertexMirror = loadShader(GLES20.GL_VERTEX_SHADER, vertexSrcMirror);

        int fragmentCopy = loadShader(GLES20.GL_FRAGMENT_SHADER, fragmentSrcCopy);
        int fragmentUniform = loadShader(GLES20.GL_FRAGMENT_SHADER, fragmentSrcUniform);
        int fragmentMasked = loadShader(GLES20.GL_FRAGMENT_SHADER, fragmentSrcMasked);
        int fragmentWavy = loadShader(GLES20.GL_FRAGMENT_SHADER, fragmentSrcWavy);
        int fragmentDisintegrate = loadShader(GLES20.GL_FRAGMENT_SHADER, fragmentSrcDisintegrate);
        int fragmentMirror = loadShader(GLES20.GL_FRAGMENT_SHADER, fragmentSrcMirror);
        int fragmentCircle = loadShader(GLES20.GL_FRAGMENT_SHADER, fragmentSrcCircle);
        int fragmentZX = loadShader(GLES20.GL_FRAGMENT_SHADER, fragmentSrcZX);

        programCopy = createProgram(vertexCopy, fragmentCopy);
        programUniform = createProgram(vertexCopy, fragmentUniform);
        programScaled = createProgram(vertexScaled, fragmentCopy);
        programMasked = createProgram(vertexCopy, fragmentMasked);
        programWavy = createProgram(vertexCopy, fragmentWavy);
        programDisintegrate = createProgram(vertexCopy, fragmentDisintegrate);
        programMirror = createProgram(vertexMirror, fragmentMirror);
        programCircle = createProgram(vertexCopy, fragmentCircle);
        programZX = createProgram(vertexCopy, fragmentZX);

        final float[] square = {
                0f, 0f,
                0f, 1f,
                1f, 0f,
                1f, 1f
        };
        buffer = ByteBuffer.allocateDirect(square.length * FLOAT_SIZE).order(ByteOrder.nativeOrder()).asFloatBuffer();
        buffer.put(square);

        int fb[] = new int[1];
        GLES20.glGenFramebuffers(1, fb, 0);
        framebuffer = fb[0];
    }

    private static void checkEglError(String msg) {
        int error;
        if ((error = egl.eglGetError()) != EGL10.EGL_SUCCESS) {
            throw new RuntimeException(msg + ": EGL error: 0x" + Integer.toHexString(error));
        }
    }

    private FFNGSurface() {
        final int tex[] = new int[1];
        GLES20.glGenTextures(1, tex, 0);
        texture = tex[0];
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, texture);
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_NEAREST);
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_NEAREST);
        width = 0;
        height = 0;
    }

    FFNGSurface(int width, int height) {
        this();
        GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_RGBA, width, height, 0, GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, null);
        this.width = width;
        this.height = height;
    }

    FFNGSurface(String file) {
        this();
        Bitmap bitmap = BitmapFactory.decodeStream(FFNG.files.internal(file).read(), null, null);
        GLUtils.texImage2D(GLES20.GL_TEXTURE_2D, 0, bitmap, 0);
        this.width = bitmap.getWidth();
        this.height = bitmap.getHeight();
        bitmap.recycle();
    }*/

    static Bitmap loadBitmap(String file) {
        BitmapFactory.Options options = new BitmapFactory.Options(); // TODO static
        options.inPreferredConfig = Bitmap.Config.ARGB_8888;
        return BitmapFactory.decodeStream(FFNG.files.internal(file).read(), null, options);
    }

    /*FFNGSurface(Bitmap bmp) {
        this();
        GLUtils.texImage2D(GLES20.GL_TEXTURE_2D, 0, bmp, 0);
        this.width = bmp.getWidth();
        this.height = bmp.getHeight();
    }

    public static FFNGSurface newSurface(int width, int height) {
        return new FFNGSurface(width, height);
    }

    public static FFNGSurface newSurface(String file) {
        return new FFNGSurface(file);
    }

    public static FFNGSurface newSurface(FFNGFont font, String text, int frontColor, int bgColor, int outlineWidth) {
        Rect bounds = font.getBounds(text);
        Bitmap bmp = Bitmap.createBitmap(bounds.width() + 2 * outlineWidth, bounds.height() + 2 * outlineWidth, Bitmap.Config.ARGB_8888);
        Canvas canvas = new Canvas(bmp);
        font.render(canvas, text, frontColor, bgColor, outlineWidth);
        FFNGSurface surface = new FFNGSurface(bmp);
        bmp.recycle();
        return surface;
    }

    public int getTexture() {
        return texture;
    }

    Bitmap getBitmap() {
        ByteBuffer pixelBuffer = java.nio.ByteBuffer.allocateDirect(width * height * 4);
        pixelBuffer.order(ByteOrder.LITTLE_ENDIAN);
        pixelBuffer.rewind();
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);
        GLES20.glViewport(0, 0, width, height);
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);
        blit(0, 0, this, 0, 0, width, height);
        egl.eglSwapBuffers(dpy, sfc);
        GLES20.glReadPixels(0, 0, width, height, GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, pixelBuffer);
        pixelBuffer.rewind();
        Bitmap bmp = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
        bmp.copyPixelsFromBuffer(pixelBuffer);
        return bmp;
    }

    public void blit(int dstx, int dsty, FFNGSurface source, int srcx, int srcy, int srcw, int srch) {
        GLES20.glActiveTexture(GLES20.GL_TEXTURE1);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, source.getTexture());

        boolean full = srcx == 0 && srcy == 0 && srcw == source.width && srch == source.height;
        int program = full ? programCopy : programScaled;
        GLES20.glUseProgram(program);

        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, framebuffer);
        GLES20.glFramebufferTexture2D(GLES20.GL_FRAMEBUFFER, GLES20.GL_COLOR_ATTACHMENT0, GLES20.GL_TEXTURE_2D, texture, 0);
        GLES20.glViewport(dstx, dsty, srcw, srch);

        int uTexture = GLES20.glGetUniformLocation(program, "uSrcTexture");
        GLES20.glUniform1i(uTexture, 1);

        if(!full) {
            int uScale = GLES20.glGetUniformLocation(program, "uScale");
            int uOffset = GLES20.glGetUniformLocation(program, "uOffset");
            GLES20.glUniform2f(uScale, (float)srcw / source.width, (float)srch / source.height);
            GLES20.glUniform2f(uOffset, (float)srcx / source.width, (float)srcy / source.height);
        }

        int aPosition = GLES20.glGetAttribLocation(program, "aPosition");
        buffer.rewind();
        GLES20.glVertexAttribPointer(aPosition, 2, GLES20.GL_FLOAT, false, 2 * FLOAT_SIZE, buffer);
        GLES20.glEnableVertexAttribArray(aPosition);

        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4);
    }

    public void blitMasked(int x, int y, final FFNGSurface mask, int color, final FFNGSurface layer) {
        GLES20.glActiveTexture(GLES20.GL_TEXTURE1);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, layer.getTexture());
        GLES20.glActiveTexture(GLES20.GL_TEXTURE2);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, mask.getTexture());
        GLES20.glUseProgram(programMasked);

        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, framebuffer);
        GLES20.glFramebufferTexture2D(GLES20.GL_FRAMEBUFFER, GLES20.GL_COLOR_ATTACHMENT0, GLES20.GL_TEXTURE_2D, texture, 0);
        GLES20.glViewport(x, y, layer.width, layer.height);

        int uTexture = GLES20.glGetUniformLocation(programMasked, "uSrcTexture");
        int uMask = GLES20.glGetUniformLocation(programMasked, "uMaskTexture");
        GLES20.glUniform1i(uTexture, 1);
        GLES20.glUniform1i(uMask, 2);

        int uMaskColor = GLES20.glGetUniformLocation(programMasked, "uMaskColor");
        GLES20.glUniform4f(uMaskColor, ((color >> 16) & 255)/255.0f, ((color >> 8) & 255)/255.0f, ((color >> 0) & 255)/255.0f, ((color >> 24) & 255)/255.0f);

        int aPosition = GLES20.glGetAttribLocation(programMasked, "aPosition");
        buffer.rewind();
        GLES20.glVertexAttribPointer(aPosition, 2, GLES20.GL_FLOAT, false, 2 * FLOAT_SIZE, buffer);
        GLES20.glEnableVertexAttribArray(aPosition);

        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4);
    }

    public void blitWavy(final FFNGSurface surface, int x, int y, float amp, float period, float shift) {
        GLES20.glActiveTexture(GLES20.GL_TEXTURE1);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, surface.getTexture());
        GLES20.glUseProgram(programWavy);

        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, framebuffer);
        GLES20.glFramebufferTexture2D(GLES20.GL_FRAMEBUFFER, GLES20.GL_COLOR_ATTACHMENT0, GLES20.GL_TEXTURE_2D, texture, 0);
        GLES20.glViewport(x, y, surface.width, surface.height);

        int uTexture = GLES20.glGetUniformLocation(programWavy, "uSrcTexture");
        GLES20.glUniform1i(uTexture, 1);

        int aPosition = GLES20.glGetAttribLocation(programWavy, "aPosition");
        buffer.rewind();
        GLES20.glVertexAttribPointer(aPosition, 2, GLES20.GL_FLOAT, false, 2 * FLOAT_SIZE, buffer);
        GLES20.glEnableVertexAttribArray(aPosition);

        int uAmplitude = GLES20.glGetUniformLocation(programWavy, "uAmplitude");
        int uPeriod = GLES20.glGetUniformLocation(programWavy, "uPeriod");
        int uShift = GLES20.glGetUniformLocation(programWavy, "uShift");
        GLES20.glUniform1f(uAmplitude, amp / width);
        GLES20.glUniform1f(uPeriod, period / height);
        GLES20.glUniform1f(uShift, shift);

        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4);
    }

    public void blitDisintegrate(final FFNGSurface surface, int x, int y, int disint) {
        GLES20.glActiveTexture(GLES20.GL_TEXTURE1);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, surface.getTexture());
        GLES20.glUseProgram(programDisintegrate);

        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, framebuffer);
        GLES20.glFramebufferTexture2D(GLES20.GL_FRAMEBUFFER, GLES20.GL_COLOR_ATTACHMENT0, GLES20.GL_TEXTURE_2D, texture, 0);
        GLES20.glViewport(x, y, surface.width, surface.height);

        int uTexture = GLES20.glGetUniformLocation(programDisintegrate, "uSrcTexture");
        GLES20.glUniform1i(uTexture, 1);

        int aPosition = GLES20.glGetAttribLocation(programDisintegrate, "aPosition");
        buffer.rewind();
        GLES20.glVertexAttribPointer(aPosition, 2, GLES20.GL_FLOAT, false, 2 * FLOAT_SIZE, buffer);
        GLES20.glEnableVertexAttribArray(aPosition);

        int uWidth = GLES20.glGetUniformLocation(programDisintegrate, "uWidth");
        int uHeight = GLES20.glGetUniformLocation(programDisintegrate, "uHeight");
        int uPhase = GLES20.glGetUniformLocation(programDisintegrate, "uPhase");
        GLES20.glUniform1i(uWidth, surface.width);
        GLES20.glUniform1i(uHeight, surface.height);
        GLES20.glUniform1i(uPhase, disint);

        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4);
    }

    public void blitMirror(final FFNGSurface surface, int x, int y, int border) {
        int color = surface.getPixel(surface.width / 2, surface.height / 2);

        GLES20.glActiveTexture(GLES20.GL_TEXTURE1);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, surface.getTexture());
        GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, texture);
        GLES20.glUseProgram(programMirror);

        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, framebuffer);
        GLES20.glFramebufferTexture2D(GLES20.GL_FRAMEBUFFER, GLES20.GL_COLOR_ATTACHMENT0, GLES20.GL_TEXTURE_2D, texture, 0);
        GLES20.glViewport(0, 0, width, height);

        int uSrcTexture = GLES20.glGetUniformLocation(programMirror, "uSrcTexture");
        int uTgtTexture = GLES20.glGetUniformLocation(programMirror, "uTgtTexture");
        GLES20.glUniform1i(uSrcTexture, 1);
        GLES20.glUniform1i(uTgtTexture, 0);

        int uMaskColor = GLES20.glGetUniformLocation(programMirror, "uMaskColor");
        GLES20.glUniform4f(uMaskColor, ((color >> 16) & 255)/255.0f, ((color >> 8) & 255)/255.0f, ((color >> 0) & 255)/255.0f, ((color >> 24) & 255)/255.0f);

        int uScale = GLES20.glGetUniformLocation(programMirror, "uScale");
        int uOffset= GLES20.glGetUniformLocation(programMirror, "uOffset");
        GLES20.glUniform2f(uScale, (float)surface.width / width, (float)surface.height / height);
        GLES20.glUniform3f(uOffset, (float)x / width, (float)y / height, (float)border / width);

        int aPosition = GLES20.glGetAttribLocation(programMirror, "aPosition");
        buffer.rewind();
        GLES20.glVertexAttribPointer(aPosition, 2, GLES20.GL_FLOAT, false, 2 * FLOAT_SIZE, buffer);
        GLES20.glEnableVertexAttribArray(aPosition);

        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4);
    }

    public void blitReverse(final FFNGSurface surface, int x, int y) {
        GLES20.glActiveTexture(GLES20.GL_TEXTURE1);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, surface.getTexture());
        GLES20.glUseProgram(programScaled);

        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, framebuffer);
        GLES20.glFramebufferTexture2D(GLES20.GL_FRAMEBUFFER, GLES20.GL_COLOR_ATTACHMENT0, GLES20.GL_TEXTURE_2D, texture, 0);
        GLES20.glViewport(x, y, surface.width, surface.height);

        int uTexture = GLES20.glGetUniformLocation(programScaled, "uSrcTexture");
        GLES20.glUniform1i(uTexture, 1);

        int uScale = GLES20.glGetUniformLocation(programScaled, "uScale");
        int uOffset= GLES20.glGetUniformLocation(programScaled, "uOffset");
        GLES20.glUniform2f(uScale, -1.0f, 1.0f);
        GLES20.glUniform2f(uOffset, 1.0f, 0.0f);

        int aPosition = GLES20.glGetAttribLocation(programScaled, "aPosition");
        buffer.rewind();
        GLES20.glVertexAttribPointer(aPosition, 2, GLES20.GL_FLOAT, false, 2 * FLOAT_SIZE, buffer);
        GLES20.glEnableVertexAttribArray(aPosition);

        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4);
    }

    public static final int ZX1 = 1;  //red
    public static final int ZX2 = 2;  //cyan
    public static final int ZX3 = 3;  //blue
    public static final int ZX4 = 4;  //yellow
    //public static final int zxColors[] = {0xFF800000, 0xFF008080, 0xFF000080, 0xFF808000};
    private static final float zxColors[] = {
            0.5f, 0.0f, 0.0f, 1.0f,
            0.0f, 0.5f, 0.5f, 1.0f,
            0.0f, 0.0f, 0.5f, 1.0f,
            0.5f, 0.5f, 0.0f, 1.0f
    };
    public void blitZX(final FFNGSurface surface, int x, int y, int zx, int countHeight, int stripeHeight) {
        GLES20.glActiveTexture(GLES20.GL_TEXTURE1);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, surface.getTexture());
        GLES20.glUseProgram(programZX);

        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, framebuffer);
        GLES20.glFramebufferTexture2D(GLES20.GL_FRAMEBUFFER, GLES20.GL_COLOR_ATTACHMENT0, GLES20.GL_TEXTURE_2D, texture, 0);
        GLES20.glViewport(x, y, surface.width, surface.height);

        int uTexture = GLES20.glGetUniformLocation(programZX, "uSrcTexture");
        GLES20.glUniform1i(uTexture, 1);

        int uMaskColor = GLES20.glGetUniformLocation(programZX, "uMaskColor");
        GLES20.glUniform4f(uMaskColor, 0.5f, 0.5f, 0.5f, 1.0f);

        int uColors = GLES20.glGetUniformLocation(programZX, "uColors");
        GLES20.glUniform4fv(uColors, 2, zxColors, zx == ZX1 || zx == ZX2 ? 0 : 8);

        int uStartY = GLES20.glGetUniformLocation(programZX, "uStartY");
        int uPeriodY = GLES20.glGetUniformLocation(programZX, "uPeriodY");
        if(zx == ZX2 || zx == ZX4)
            GLES20.glUniform1f(uStartY, (float)(countHeight + stripeHeight) / height);
        else
            GLES20.glUniform1f(uStartY, (float)countHeight / height);
        GLES20.glUniform1f(uPeriodY, 2 * (float)stripeHeight / height);

        int aPosition = GLES20.glGetAttribLocation(programZX, "aPosition");
        buffer.rewind();
        GLES20.glVertexAttribPointer(aPosition, 2, GLES20.GL_FLOAT, false, 2 * FLOAT_SIZE, buffer);
        GLES20.glEnableVertexAttribArray(aPosition);

        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4);
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
    }

    public int getPixel(int x, int y) {
        ByteBuffer pixelBuffer = java.nio.ByteBuffer.allocateDirect(4);
        pixelBuffer.order(ByteOrder.LITTLE_ENDIAN);
        pixelBuffer.rewind();
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);
        GLES20.glViewport(0, 0, width, height);
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);
        blit(0, 0, this, 0, 0, width, height);
        egl.eglSwapBuffers(dpy, sfc);
        GLES20.glReadPixels(x, y, 1, 1, GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, pixelBuffer);
        return (convertByte(pixelBuffer.get(0)) << 16) | (convertByte(pixelBuffer.get(1)) << 8)
                | (convertByte(pixelBuffer.get(2)) | (convertByte(pixelBuffer.get(3)) << 24));
    }

    private int convertByte(byte b) {
        return b >= 0 ? b : 256 + b;
    }

    public void dispose() {
        int tex[] = {texture};
        GLES20.glDeleteTextures(1, tex, 0);
    }

    public int getWidth() {
        return width;
    }

    public int getHeight() {
        return height;
    }

    protected void finalize() throws Throwable {
        dispose();
        super.finalize(); //not necessary if extending Object.
    }


    private static int loadShader(int shaderType, String shaderSource) {
        int handle = GLES20.glCreateShader(shaderType);

        if (handle == GLES20.GL_FALSE)
            throw new RuntimeException("Error creating shader!");

        GLES20.glShaderSource(handle, shaderSource);
        GLES20.glCompileShader(handle);

        int[] compileStatus = new int[1];
        GLES20.glGetShaderiv(handle, GLES20.GL_COMPILE_STATUS, compileStatus, 0);

        if (compileStatus[0] == 0) {
            String error = GLES20.glGetShaderInfoLog(handle);
            GLES20.glDeleteShader(handle);
            throw new RuntimeException("Error compiling shader: " + error);
        } else
            return handle;
    }

    private static int createProgram(int vertexShader, int fragmentShader) {
        int handle = GLES20.glCreateProgram();

        if (handle == GLES20.GL_FALSE)
            throw new RuntimeException("Error creating program!");

        GLES20.glAttachShader(handle, vertexShader);
        GLES20.glAttachShader(handle, fragmentShader);
        GLES20.glLinkProgram(handle);

        int[] linkStatus = new int[1];
        GLES20.glGetProgramiv(handle, GLES20.GL_LINK_STATUS, linkStatus, 0);

        if (linkStatus[0] == 0) {
            String error = GLES20.glGetProgramInfoLog(handle);
            GLES20.glDeleteProgram(handle);
            throw new RuntimeException("Error in program linking: " + error);
        } else
            return handle;
    }*/
}
