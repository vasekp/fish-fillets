#include <assert.h>
#include "FFNGSurface.h"
#include <android/log.h>
#include <android/bitmap.h>

EGLContext FFNGSurface::ctx;
EGLDisplay FFNGSurface::dpy;
EGLSurface FFNGSurface::sfc;

SDL_Surface::SDL_Surface() : w(0), h(0) {
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

SDL_Surface::SDL_Surface(int width, int height, Uint32, bool) : SDL_Surface() {
    w = width;
    h = height;
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
    w = info.width;
    h = info.height;
    if(info.format != ANDROID_BITMAP_FORMAT_RGBA_8888)
        ;//

    void* pixels;
    AndroidBitmap_lockPixels(javaEnv, jBitmap, &pixels);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    AndroidBitmap_unlockPixels(javaEnv, jBitmap);

    javaEnv->DeleteLocalRef(pathString);
    javaEnv->DeleteLocalRef(jBitmap);

    w = getWidth();
    h = getHeight();
}

SDL_Surface::SDL_Surface(const std::string &path) : SDL_Surface(path.c_str()) { }

SDL_Surface::SDL_Surface(jobject font, const char *text, int frontColor, int bgColor,
                         int outlineWidth) : SDL_Surface() { }
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

GLuint SDL_Surface::getSurface() const {
    return texture;
}

void
SDL_Surface::blit(int dstx, int dsty, SDL_Surface *source, int srcx, int srcy, int srcw, int srch) {
}

void SDL_Surface::blitMasked(int dstx, int dsty, const SDL_Surface *mask, Uint32 color,
                             const SDL_Surface *layer) {
}

void SDL_Surface::blitWavy(const SDL_Surface *source, int x, int y, float amp, float periode,
                           float speed) {
}

void SDL_Surface::blitDisintegrate(const SDL_Surface *source, int x, int y, int disint) {
}

void SDL_Surface::blitMirror(const SDL_Surface *source, int x, int y, int border) {

}

void SDL_Surface::blitReverse(const SDL_Surface *source, int x, int y) {

}

void SDL_Surface::blitZX(const SDL_Surface *source, int x, int y, int zx, int countHeight,
                         int stripeHeight) {

}

void SDL_Surface::line(int x1, int y1, int x2, int y2, Uint32 colorRGBA) {

}

void SDL_Surface::fillRect(const SDL_Rect *dstRect, Uint32 pixel) {
    int x_ = 0,
            y_ = 0,
            w_ = w,
            h_ = h;
    if(dstRect) {
        x_ = dstRect->x;
        y_ = dstRect->y;
        w_ = dstRect->w;
        h_ = dstRect->h;
    }
}

void SDL_Surface::filledCircleColor(int x, int y, int radius, Uint32 colorRGBA) {

}

Uint32 SDL_Surface::getPixel(int x, int y) {
    return 0;
}

int SDL_Surface::getWidth() const {
    return w;
}

int SDL_Surface::getHeight() const {
    return h;
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
/*    int dstx, dsty, srcx, srcy, srcw, srch;

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
        srcw = srcSurface->w;
        srch = srcSurface->h;
    }

    dstSurface->blit(dstx, dsty, srcSurface, srcx, srcy, srcw, srch);*/
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
        EGLConfig cfg;
        eglChooseConfig(dpy, attribs, &cfg, 1, &cfgCount);
        if(cfgCount != 1)
            ;
        return cfg;
    }();

    ctx = [&cfg]() -> EGLContext {
        EGLint attribs[] = {
                EGL_CONTEXT_CLIENT_VERSION, 2,
                EGL_NONE
        };
        return eglCreateContext(dpy, cfg, nullptr, attribs);
    }();

    sfc = [&cfg]() -> EGLSurface {
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
    glClear(GL_COLOR_BUFFER_BIT);
}