#include <assert.h>
#include "FFNGSurface.h"
#include <android/log.h>
#include <android/bitmap.h>

EGLContext FFNGSurface::ctx;
EGLDisplay FFNGSurface::dpy;
EGLSurface FFNGSurface::sfc;

GLuint FFNGSurface::framebuffer;
GLuint FFNGSurface::programCopy;
GLuint FFNGSurface::programUniform;
GLuint FFNGSurface::programScaled;
GLuint FFNGSurface::programMasked;
GLuint FFNGSurface::programWavy;
GLuint FFNGSurface::programDisintegrate;
GLuint FFNGSurface::programMirror;
GLuint FFNGSurface::programCircle;
GLuint FFNGSurface::programZX;

constexpr float FFNGSurface::square[8];

SDL_Surface::SDL_Surface() : width(0), height(0) {
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

SDL_Surface::SDL_Surface(int width_, int height_, Uint32, bool) : SDL_Surface() {
    width = width_;
    height = height_;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
}

SDL_Surface::SDL_Surface(const char *path) : SDL_Surface() {
    static JNIEnv *javaEnv = NULL;
    static jclass cls = NULL;
    static jmethodID mid = NULL;

    if(!mid) {
        javaEnv = JNI::getInstance()->getJavaEnv();
        cls = javaEnv->FindClass("cz/ger/ffng/FFNGSurface");
        mid = javaEnv->GetStaticMethodID(cls, "loadBitmap",
                                         "(Ljava/lang/String;)Landroid/graphics/Bitmap;");
    }
    //__android_log_print(ANDROID_LOG_DEBUG, "FFNG", "SDL_Surface::SDL_Surface 1 %p %p %p", javaEnv, cls, mid);

    if(mid == NULL) {
        assert("method not found");
        return;
    }

    jstring pathString = javaEnv->NewStringUTF(path);

    jobject jBitmap = javaEnv->CallStaticObjectMethod(cls, mid, pathString);

    AndroidBitmapInfo info;
    AndroidBitmap_getInfo(javaEnv, jBitmap, &info);
    width = info.width;
    height = info.height;
    if(info.format != ANDROID_BITMAP_FORMAT_RGBA_8888)
        ;

    void *pixels;
    AndroidBitmap_lockPixels(javaEnv, jBitmap, &pixels);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    AndroidBitmap_unlockPixels(javaEnv, jBitmap);

    javaEnv->DeleteLocalRef(pathString);
    javaEnv->DeleteLocalRef(jBitmap);
}

SDL_Surface::SDL_Surface(const std::string &path) : SDL_Surface(path.c_str()) {}

SDL_Surface::SDL_Surface(jobject font, const char *text, int frontColor, int bgColor,
                         int outlineWidth) : SDL_Surface() {}
//        : surface(NULL) {
//    // TODO w, h, colorkey
//    static JNIEnv *javaEnv = NULL;
//    static jclass cls = NULL;
//    static jmethodID mid = NULL;
//
//    if(!mid) {
//        javaEnv = JNI::getInstance()->getJavaEnv();
//        cls = javaEnv->FindClass("cz/ger/ffng/FFNGSurface");
//        mid = javaEnv->GetStaticMethodID(cls, "newSurface",
//                                         "(Lcz/ger/ffng/FFNGFont;Ljava/lang/String;III)Lcz/ger/ffng/FFNGSurface;");
//    }
//    //__android_log_print(ANDROID_LOG_DEBUG, "FFNG", "SDL_Surface::SDL_Surface 1 %p %p %p", javaEnv, cls, mid);
//
//    if(mid == NULL) {
//        assert("method not found");
//        return;
//    }
//
//    jstring textString = javaEnv->NewStringUTF(text);
//
//    surface = javaEnv->CallStaticObjectMethod(cls, mid, font, textString, frontColor, bgColor,
//                                              outlineWidth);
//
//    javaEnv->DeleteLocalRef(textString);
//
//    w = getWidth();
//    h = getHeight();
//}

SDL_Surface::~SDL_Surface() {
    glDeleteTextures(1, &texture);
}

GLuint SDL_Surface::getTexture() const {
    return texture;
}

void
SDL_Surface::blit(int dstx, int dsty, SDL_Surface *source, int srcx, int srcy, int srcw, int srch) {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, source->getTexture());

    bool full = srcx == 0 && srcy == 0 && srcw == source->getWidth() && srch == source->getHeight();

    GLuint program = full ? FFNGSurface::programCopy : FFNGSurface::programScaled;
    glUseProgram(program);

    glBindFramebuffer(GL_FRAMEBUFFER, FFNGSurface::framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    glViewport(dstx, dsty, srcw, srch);

    int uTexture = glGetUniformLocation(program, "uSrcTexture");
    glUniform1i(uTexture, 1);

    if(!full) {
        int uScale = glGetUniformLocation(program, "uScale");
        int uOffset = glGetUniformLocation(program, "uOffset");
        glUniform2f(uScale, (float) srcw / source->getWidth(), (float) srch / source->getHeight());
        glUniform2f(uOffset, (float) srcx / source->getWidth(), (float) srcy / source->getHeight());
    }

    GLuint aPosition = (GLuint) glGetAttribLocation(program, "aPosition");
    glVertexAttribPointer(aPosition, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), FFNGSurface::square);
    glEnableVertexAttribArray(aPosition);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void SDL_Surface::blitMasked(int dstx, int dsty, const SDL_Surface *mask, Uint32 color,
                             const SDL_Surface *layer) {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, layer->getTexture());
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, mask->getTexture());
    glUseProgram(FFNGSurface::programMasked);

    glBindFramebuffer(GL_FRAMEBUFFER, FFNGSurface::framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    glViewport(dstx, dsty, layer->width, layer->height);

    int uTexture = glGetUniformLocation(FFNGSurface::programMasked, "uSrcTexture");
    int uMask = glGetUniformLocation(FFNGSurface::programMasked, "uMaskTexture");
    glUniform1i(uTexture, 1);
    glUniform1i(uMask, 2);

    int uMaskColor = glGetUniformLocation(FFNGSurface::programMasked, "uMaskColor");
    glUniform4f(uMaskColor, ((color >> 16) & 255) / 255.0f, ((color >> 8) & 255) / 255.0f,
                ((color >> 0) & 255) / 255.0f, ((color >> 24) & 255) / 255.0f);

    GLuint aPosition = (GLuint) glGetAttribLocation(FFNGSurface::programMasked, "aPosition");
    glVertexAttribPointer(aPosition, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), FFNGSurface::square);
    glEnableVertexAttribArray(aPosition);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void SDL_Surface::blitWavy(const SDL_Surface *source, int x, int y, float amp, float period,
                           float shift) {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, source->getTexture());
    glUseProgram(FFNGSurface::programWavy);

    glBindFramebuffer(GL_FRAMEBUFFER, FFNGSurface::framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    glViewport(x, y, source->width, source->height);

    int uTexture = glGetUniformLocation(FFNGSurface::programWavy, "uSrcTexture");
    glUniform1i(uTexture, 1);

    GLuint aPosition = (GLuint) glGetAttribLocation(FFNGSurface::programWavy, "aPosition");
    glVertexAttribPointer(aPosition, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), FFNGSurface::square);
    glEnableVertexAttribArray(aPosition);

    int uAmplitude = glGetUniformLocation(FFNGSurface::programWavy, "uAmplitude");
    int uPeriod = glGetUniformLocation(FFNGSurface::programWavy, "uPeriod");
    int uShift = glGetUniformLocation(FFNGSurface::programWavy, "uShift");
    glUniform1f(uAmplitude, amp / width);
    glUniform1f(uPeriod, period / height);
    glUniform1f(uShift, shift);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void SDL_Surface::blitDisintegrate(const SDL_Surface *source, int x, int y, int disint) {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, source->getTexture());
    glUseProgram(FFNGSurface::programDisintegrate);

    glBindFramebuffer(GL_FRAMEBUFFER, FFNGSurface::framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    glViewport(x, y, source->width, source->height);

    int uTexture = glGetUniformLocation(FFNGSurface::programDisintegrate, "uSrcTexture");
    glUniform1i(uTexture, 1);

    GLuint aPosition = (GLuint) glGetAttribLocation(FFNGSurface::programDisintegrate, "aPosition");
    glVertexAttribPointer(aPosition, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), FFNGSurface::square);
    glEnableVertexAttribArray(aPosition);

    int uWidth = glGetUniformLocation(FFNGSurface::programDisintegrate, "uWidth");
    int uHeight = glGetUniformLocation(FFNGSurface::programDisintegrate, "uHeight");
    int uPhase = glGetUniformLocation(FFNGSurface::programDisintegrate, "uPhase");
    glUniform1i(uWidth, source->width);
    glUniform1i(uHeight, source->height);
    glUniform1i(uPhase, disint);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void SDL_Surface::blitMirror(const SDL_Surface *source, int x, int y, int border) {
    int color = source->getPixel(source->width / 2, source->height / 2);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, source->getTexture());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUseProgram(FFNGSurface::programMirror);

    glBindFramebuffer(GL_FRAMEBUFFER, FFNGSurface::framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    glViewport(0, 0, width, height);

    int uSrcTexture = glGetUniformLocation(FFNGSurface::programMirror, "uSrcTexture");
    int uTgtTexture = glGetUniformLocation(FFNGSurface::programMirror, "uTgtTexture");
    glUniform1i(uSrcTexture, 1);
    glUniform1i(uTgtTexture, 0);

    int uMaskColor = glGetUniformLocation(FFNGSurface::programMirror, "uMaskColor");
    glUniform4f(uMaskColor, ((color >> 16) & 255) / 255.0f, ((color >> 8) & 255) / 255.0f,
                ((color >> 0) & 255) / 255.0f, ((color >> 24) & 255) / 255.0f);

    int uScale = glGetUniformLocation(FFNGSurface::programMirror, "uScale");
    int uOffset = glGetUniformLocation(FFNGSurface::programMirror, "uOffset");
    glUniform2f(uScale, (float) source->width / width, (float) source->height / height);
    glUniform3f(uOffset, (float) x / width, (float) y / height, (float) border / width);

    GLuint aPosition = (GLuint) glGetAttribLocation(FFNGSurface::programMirror, "aPosition");
    glVertexAttribPointer(aPosition, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), FFNGSurface::square);
    glEnableVertexAttribArray(aPosition);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void SDL_Surface::blitReverse(const SDL_Surface *source, int x, int y) {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, source->getTexture());
    glUseProgram(FFNGSurface::programScaled);

    glBindFramebuffer(GL_FRAMEBUFFER, FFNGSurface::framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    glViewport(x, y, source->width, source->height);

    int uTexture = glGetUniformLocation(FFNGSurface::programScaled, "uSrcTexture");
    glUniform1i(uTexture, 1);

    int uScale = glGetUniformLocation(FFNGSurface::programScaled, "uScale");
    int uOffset = glGetUniformLocation(FFNGSurface::programScaled, "uOffset");
    glUniform2f(uScale, -1.0f, 1.0f);
    glUniform2f(uOffset, 1.0f, 0.0f);

    GLuint aPosition = (GLuint) glGetAttribLocation(FFNGSurface::programScaled, "aPosition");
    glVertexAttribPointer(aPosition, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), FFNGSurface::square);
    glEnableVertexAttribArray(aPosition);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

constexpr int ZX1 = 1;  //red
constexpr int ZX2 = 2;  //cyan
constexpr int ZX3 = 3;  //blue
constexpr int ZX4 = 4;  //yellow
constexpr float zxColors[] = {
        0.5f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.5f, 0.5f, 1.0f,
        0.0f, 0.0f, 0.5f, 1.0f,
        0.5f, 0.5f, 0.0f, 1.0f
};
void SDL_Surface::blitZX(const SDL_Surface *source, int x, int y, int zx, int countHeight,
                         int stripeHeight) {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, source->getTexture());
    glUseProgram(FFNGSurface::programZX);

    glBindFramebuffer(GL_FRAMEBUFFER, FFNGSurface::framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    glViewport(x, y, source->width, source->height);

    int uTexture = glGetUniformLocation(FFNGSurface::programZX, "uSrcTexture");
    glUniform1i(uTexture, 1);

    int uMaskColor = glGetUniformLocation(FFNGSurface::programZX, "uMaskColor");
    glUniform4f(uMaskColor, 0.5f, 0.5f, 0.5f, 1.0f);

    int uColors = glGetUniformLocation(FFNGSurface::programZX, "uColors");
    glUniform4fv(uColors, 2, zxColors + (zx == ZX1 || zx == ZX2 ? 0 : 8));

    int uStartY = glGetUniformLocation(FFNGSurface::programZX, "uStartY");
    int uPeriodY = glGetUniformLocation(FFNGSurface::programZX, "uPeriodY");
    if(zx == ZX2 || zx == ZX4)
        glUniform1f(uStartY, (float) (countHeight + stripeHeight) / height);
    else
        glUniform1f(uStartY, (float) countHeight / height);
    glUniform1f(uPeriodY, 2 * (float) stripeHeight / height);

    GLuint aPosition = (GLuint) glGetAttribLocation(FFNGSurface::programZX, "aPosition");
    glVertexAttribPointer(aPosition, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), FFNGSurface::square);
    glEnableVertexAttribArray(aPosition);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void SDL_Surface::line(int x1, int y1, int x2, int y2, Uint32 colorRGBA) {

}

void SDL_Surface::fillRect(const SDL_Rect *dstRect, Uint32 pixel) {
    int x_ = 0,
            y_ = 0,
            w_ = width,
            h_ = height;
    if(dstRect) {
        x_ = dstRect->x;
        y_ = dstRect->y;
        w_ = dstRect->w;
        h_ = dstRect->h;
    }
}

void SDL_Surface::filledCircleColor(int x, int y, int radius, Uint32 colorRGBA) {

}

Uint32 SDL_Surface::getPixel(int x, int y) const {
    glBindFramebuffer(GL_FRAMEBUFFER, FFNGSurface::framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    eglSwapBuffers(FFNGSurface::dpy, FFNGSurface::sfc);

    unsigned char pixels[4];
    glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    return (pixels[0] << 16) | (pixels[1] << 8) | (pixels[2] << 0) | (pixels[3] << 24);
}

int SDL_Surface::getWidth() const {
    return width;
}

int SDL_Surface::getHeight() const {
    return height;
}

SDL_Rect::SDL_Rect() {
    SDL_Rect(0, 0, 0, 0);
}

SDL_Rect::SDL_Rect(int x_, int y_, int w_, int h_)
        : x(x_), y(y_), w(w_), h(h_) {
}

SDL_Surface *FFNGSurface::imgLoad(const char *file) {
    return new SDL_Surface(file);
}

void FFNGSurface::freeSurface(SDL_Surface *surface) {
    if(surface)
        delete surface;
}

void
FFNGSurface::blitSurface(SDL_Surface *srcSurface, const SDL_Rect *srcRect, SDL_Surface *dstSurface,
                         const SDL_Rect *dstRect) {
    int dstx, dsty, srcx, srcy, srcw, srch;

    if(dstRect) {
        dstx = dstRect->x;
        dsty = dstRect->y;
    } else {
        dstx = 0;
        dsty = 0;
    }

    if(srcRect) {
        srcx = srcRect->x;
        srcy = srcRect->y;
        srcw = srcRect->w;
        srch = srcRect->h;
    } else {
        srcx = 0;
        srcy = 0;
        srcw = srcSurface->getWidth();
        srch = srcSurface->getHeight();
    }

    dstSurface->blit(dstx, dsty, srcSurface, srcx, srcy, srcw, srch);
}

Uint32 FFNGSurface::getPixel(SDL_Surface *surface, int x, int y) {
    return surface->getPixel(x, y);
}

SDL_Surface *FFNGSurface::createSurface(int width, int height, int) {
    SDL_Surface *surface = new SDL_Surface(width, height, 0);
    return surface;
}

void FFNGSurface::fillRect(SDL_Surface *surface, const SDL_Rect *dstRect, Uint32 pixel) {
    surface->fillRect(dstRect, pixel);
}

void
FFNGSurface::filledCircleColor(SDL_Surface *screen, int x, int y, int radius, Uint32 colorRGBA) {
    screen->filledCircleColor(x, y, radius, colorRGBA);
}

void FFNGSurface::lineColor(SDL_Surface *screen, int x1, int y1, int x2, int y2, Uint32 colorRGBA) {
    screen->line(x1, y1, x2, y2, colorRGBA);
}

void FFNGSurface::initEGL() {
    dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(dpy, nullptr, nullptr);
    eglBindAPI(EGL_OPENGL_ES_API);

    EGLConfig cfg = []() -> EGLConfig {
        EGLint attribs[] = {
                EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
                EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                EGL_RED_SIZE, 8,
                EGL_GREEN_SIZE, 8,
                EGL_BLUE_SIZE, 8,
                EGL_ALPHA_SIZE, 8,
                EGL_NONE
        };
        int cfgCount;
        EGLConfig cfg_;
        eglChooseConfig(dpy, attribs, &cfg_, 1, &cfgCount);
        if(cfgCount != 1)
            ;
        return cfg_;
    }();

    ctx = [cfg]() -> EGLContext {
        EGLint attribs[] = {
                EGL_CONTEXT_CLIENT_VERSION, 2,
                EGL_NONE
        };
        return eglCreateContext(dpy, cfg, nullptr, attribs);
    }();

    sfc = [cfg]() -> EGLSurface {
        EGLint attribs[] = {
                EGL_WIDTH, MaxWidth,
                EGL_HEIGHT, MaxHeight,
                EGL_NONE
        };
        return eglCreatePbufferSurface(dpy, cfg, attribs);
    }();

    if(!eglMakeCurrent(dpy, sfc, sfc, ctx))
        ;

    glClearColor(1.f, 1.f, 0.f, 1.f);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
    glEnable(GL_BLEND);

    glGenFramebuffers(1, &FFNGSurface::framebuffer);
}

GLuint loadShader(GLenum type, std::string code) {
    GLuint handle = glCreateShader(type);
    if(handle == GL_FALSE)
        ;

    const char *source = code.c_str();
    glShaderSource(handle, 1, &source, nullptr);
    glCompileShader(handle);

    int status;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &status);

    if(status == GL_FALSE) {
        int loglen, actlen;
        glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &loglen);
        std::string error(loglen, '\0');
        glGetShaderInfoLog(handle, loglen, &actlen, &(error[0]));
        glDeleteShader(handle);
        //throw new RuntimeException("Error compiling shader: " + error);
        __android_log_print(ANDROID_LOG_DEBUG, "FFNG", "%s", error.c_str());
        return 0;
    } else
        return handle;
}

GLuint createProgram(GLuint vertexShader, GLuint fragmentShader) {
    GLuint handle = glCreateProgram();

    if(handle == GL_FALSE)
        ;
    //throw new RuntimeException("Error creating program!");

    glAttachShader(handle, vertexShader);
    glAttachShader(handle, fragmentShader);
    glLinkProgram(handle);

    int linkStatus;
    glGetProgramiv(handle, GL_LINK_STATUS, &linkStatus);

    if(linkStatus == GL_FALSE) {
        int loglen, actlen;
        glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &loglen);
        std::string error(loglen, '\0');
        glGetProgramInfoLog(handle, loglen, &actlen, &(error[0]));
        glDeleteProgram(handle);
        //throw new RuntimeException("Error in program linking: " + error);
        __android_log_print(ANDROID_LOG_DEBUG, "FFNG", "%s", error.c_str());
        return 0;
    } else
        return handle;
}

void FFNGSurface::initShaders() {
    std::string vertexCopySource = R"(
attribute vec2 aPosition;
varying vec2 vTexPos;
mat4 fxd = mat4(2.0, 0.0, 0.0, 0.0,  0.0, 2.0, 0.0, 0.0,  0.0, 0.0, 1.0, 0.0,  -1.0, -1.0, 0.0, 1.0);
void main() {
  vTexPos = aPosition;
  gl_Position = fxd * vec4(aPosition.xy, 0.0, 1.0);
})";

    std::string vertexScaledSource = R"(
attribute vec2 aPosition;
uniform vec2 uScale;
uniform vec2 uOffset;
varying vec2 vTexPos;
mat4 fxd = mat4(2.0, 0.0, 0.0, 0.0,  0.0, 2.0, 0.0, 0.0,  0.0, 0.0, 1.0, 0.0,  -1.0, -1.0, 0.0, 1.0);
void main() {
  vTexPos = uScale * aPosition + uOffset;
  gl_Position = fxd * vec4(aPosition.xy, 0.0, 1.0);
})";

    std::string vertexMirrorSource = R"(
attribute vec2 aPosition;
uniform vec2 uScale;
uniform vec3 uOffset;
varying vec2 vTexPos;
varying vec2 vMirrorTgtTexPos;
mat4 fxd = mat4(2.0, 0.0, 0.0, 0.0,  0.0, 2.0, 0.0, 0.0,  0.0, 0.0, 1.0, 0.0,  -1.0, -1.0, 0.0, 1.0);
void main() {
  vTexPos = aPosition;
  vMirrorTgtTexPos = vec2(-1.0, 1.0) * uScale * aPosition.xy + uOffset.xy + vec2(uOffset.z, 0.0);
  gl_Position = fxd * vec4(uScale * aPosition.xy + uOffset.xy, 0.0, 1.0);
})";


    std::string fragmentCopySource = R"(
precision mediump float;
uniform sampler2D uSrcTexture;
varying vec2 vTexPos;
void main(void)
{
  gl_FragColor = texture2D(uSrcTexture, vTexPos);
})";

    std::string fragmentUniformSource = R"(
precision mediump float;
uniform vec4 uColor;
void main(void)
{
  gl_FragColor = uColor;
})";

    std::string fragmentMaskedSource = R"(
precision mediump float;
uniform sampler2D uSrcTexture;
uniform sampler2D uMaskTexture;
uniform vec4 uMaskColor;
varying vec2 vTexPos;
void main(void)
{
  gl_FragColor = distance(texture2D(uMaskTexture, vTexPos), uMaskColor) < 0.1 ? texture2D(uSrcTexture, vTexPos) : vec4(0.0);
})";

    std::string fragmentWavySource = R"(
precision mediump float;
uniform sampler2D uSrcTexture;
uniform float uAmplitude;
uniform float uShift;
uniform float uPeriod;
varying vec2 vTexPos;
void main(void)
{
  gl_FragColor = texture2D(uSrcTexture, fract(vTexPos + uAmplitude * vec2(sin(vTexPos.y / uPeriod + uShift), 0.0)));
})";

    std::string fragmentDisintegrateSource = R"(
precision mediump float;
uniform sampler2D uSrcTexture;
uniform int uWidth;
uniform int uHeight;
uniform int uPhase;
varying vec2 vTexPos;
float noise(int x, int y) {
  return fract(sin(mod(float(x) + 1.618 * float(y), 6.28)) * 100.0);
})
void main(void)
{
  gl_FragColor = noise(int(vTexPos.x * float(uWidth)), int(vTexPos.y * float(uHeight))) < float(uPhase) / 255.0 ? texture2D(uSrcTexture, vTexPos) : vec4(0.0);
})";

    std::string fragmentMirrorSource = R"(
precision mediump float;
uniform sampler2D uSrcTexture;
uniform sampler2D uTgtTexture;
uniform vec4 uMaskColor;
varying vec2 vTexPos;
varying vec2 vMirrorTgtTexPos;
void main(void)
{
  gl_FragColor = distance(texture2D(uSrcTexture, vTexPos), uMaskColor) < 0.1 ? texture2D(uTgtTexture, vMirrorTgtTexPos) : texture2D(uSrcTexture, vTexPos);
})";

    std::string fragmentCircleSource = R"(
precision mediump float;
varying vec2 vTexPos;
uniform vec4 uColor;
void main(void)
{
  gl_FragColor = distance(vTexPos, vec2(0.5, 0.5)) < 0.5 ? uColor : vec4(0.0);
})";

    std::string fragmentZXSource = R"(
precision mediump float;
uniform sampler2D uSrcTexture;
uniform vec4 uMaskColor;
uniform vec4 uColors[2];
uniform float uStartY;
uniform float uPeriodY;
varying vec2 vTexPos;
void main(void)
{
  gl_FragColor = distance(texture2D(uSrcTexture, vTexPos), uMaskColor) < 0.1
    ? (fract((vTexPos.y + uStartY) / uPeriodY) < 0.5 ? uColors[0] : uColors[1])
    : vec4(0.0);
})";

    GLuint vertexCopy{loadShader(GL_VERTEX_SHADER, vertexCopySource)};
    GLuint vertexScaled{loadShader(GL_VERTEX_SHADER, vertexScaledSource)};
    GLuint vertexMirror{loadShader(GL_VERTEX_SHADER, vertexMirrorSource)};

    GLuint fragmentCopy{loadShader(GL_FRAGMENT_SHADER, fragmentCopySource)};
    GLuint fragmentUniform{loadShader(GL_FRAGMENT_SHADER, fragmentUniformSource)};
    GLuint fragmentMasked{loadShader(GL_FRAGMENT_SHADER, fragmentMaskedSource)};
    GLuint fragmentWavy{loadShader(GL_FRAGMENT_SHADER, fragmentWavySource)};
    GLuint fragmentDisintegrate{loadShader(GL_FRAGMENT_SHADER, fragmentDisintegrateSource)};
    GLuint fragmentMirror{loadShader(GL_FRAGMENT_SHADER, fragmentMirrorSource)};
    GLuint fragmentCircle{loadShader(GL_FRAGMENT_SHADER, fragmentCircleSource)};
    GLuint fragmentZX{loadShader(GL_FRAGMENT_SHADER, fragmentZXSource)};

    programCopy = createProgram(vertexCopy, fragmentCopy);
    programUniform = createProgram(vertexCopy, fragmentUniform);
    programScaled = createProgram(vertexScaled, fragmentCopy);
    programMasked = createProgram(vertexCopy, fragmentMasked);
    programWavy = createProgram(vertexCopy, fragmentWavy);
    programDisintegrate = createProgram(vertexCopy, fragmentDisintegrate);
    programMirror = createProgram(vertexMirror, fragmentMirror);
    programCircle = createProgram(vertexCopy, fragmentCircle);
    programZX = createProgram(vertexCopy, fragmentZX);
}
