#include <assert.h>
#include "FFNGSurface.h"
#include <android/log.h>
#include <android/bitmap.h>

EGLContext FFNGSurface::ctx;
EGLDisplay FFNGSurface::dpy;
EGLSurface FFNGSurface::sfc;

GLuint FFNGSurface::framebuffer;
GLuint FFNGSurface::program;

constexpr float FFNGSurface::square[8];

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
        ;

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

GLuint SDL_Surface::getTexture() const {
    return texture;
}

void
SDL_Surface::blit(int dstx, int dsty, SDL_Surface *source, int srcx, int srcy, int srcw, int srch) {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, source->getTexture());

    //bool full = srcx == 0 && srcy == 0 && srcw == source->getWidth() && srch == source->getHeight();

    //int program = full ? programCopy : programScaled;
    glUseProgram(FFNGSurface::program);

    glBindFramebuffer(GL_FRAMEBUFFER, FFNGSurface::framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    glViewport(dstx, dsty, srcw, srch);

    int uTexture = glGetUniformLocation(FFNGSurface::program, "uSrcTexture");
    glUniform1i(uTexture, 1);

    /*if(!full) {
        int uScale = glGetUniformLocation(program, "uScale");
        int uOffset = glGetUniformLocation(program, "uOffset");
        glUniform2f(uScale, (float)srcw / source.width, (float)srch / source.height);
        glUniform2f(uOffset, (float)srcx / source.width, (float)srcy / source.height);
    }*/

    GLuint aPosition = (GLuint)glGetAttribLocation(FFNGSurface::program, "aPosition");
    glVertexAttribPointer(aPosition, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), FFNGSurface::square);
    glEnableVertexAttribArray(aPosition);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
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
        srcw = srcSurface->w;
        srch = srcSurface->h;
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
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
    glEnable(GL_BLEND);

    glGenFramebuffers(1, &framebuffer);
}

GLuint loadShader(GLenum type, std::string code) {
    GLuint handle = glCreateShader(type);
    if (handle == GL_FALSE)
        ;

    const char* source = code.c_str();
    glShaderSource(handle, 1, &source, nullptr);
    glCompileShader(handle);

    int status;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &status);

    if (status == GL_FALSE) {
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

    if (handle == GL_FALSE)
        ;
        //throw new RuntimeException("Error creating program!");

    glAttachShader(handle, vertexShader);
    glAttachShader(handle, fragmentShader);
    glLinkProgram(handle);

    int linkStatus;
    glGetProgramiv(handle, GL_LINK_STATUS, &linkStatus);

    if (linkStatus == GL_FALSE) {
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

    std::string fragmentCopySource = R"(
precision mediump float;
uniform sampler2D uSrcTexture;
varying vec2 vTexPos;
void main(void)
{
  gl_FragColor = texture2D(uSrcTexture, vTexPos);
})";

    GLuint vertexCopy{loadShader(GL_VERTEX_SHADER, vertexCopySource)};
    GLuint fragmentCopy{loadShader(GL_FRAGMENT_SHADER, fragmentCopySource)};
    program = createProgram(vertexCopy, fragmentCopy);
}
