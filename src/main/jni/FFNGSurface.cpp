#include <assert.h>
#include "FFNGSurface.h"
#include <android/log.h>
#include <android/bitmap.h>

EGLContext FFNGSurface::ctx;
EGLDisplay FFNGSurface::dpy;
EGLSurface FFNGSurface::sfc;

GLuint FFNGSurface::framebuffer;

GLuint FFNGSurface::programCopy;
GLuint FFNGSurface::programMasked;
GLuint FFNGSurface::programReverse;
GLuint FFNGSurface::programMirror;
GLuint FFNGSurface::programWavy;
GLuint FFNGSurface::programDisintegrate;
GLuint FFNGSurface::programZX;

GLuint FFNGSurface::programUniform;
GLuint FFNGSurface::programCircle;

#define SQUARE(x, y, w, h) {\
  {(float)(x), (float)(y)}, \
  {(float)(x + w), (float)(y)}, \
  {(float)(x), (float)(y + h)}, \
  {(float)(x + w), (float)(y + h)} \
}

SDL_Surface::SDL_Surface() : width(0), height(0) {
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
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

    if(javaEnv != JNI::getInstance()->getJavaEnv()) {
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
                         int outlineWidth) : SDL_Surface() {
    static JNIEnv *javaEnv = NULL;
    static jclass cls = NULL;
    static jmethodID mid = NULL;

    if(javaEnv != JNI::getInstance()->getJavaEnv()) {
        javaEnv = JNI::getInstance()->getJavaEnv();
        cls = javaEnv->FindClass("cz/ger/ffng/FFNGSurface");
        mid = javaEnv->GetStaticMethodID(cls, "newSurface",
                                         "(Lcz/ger/ffng/FFNGFont;Ljava/lang/String;III)Landroid/graphics/Bitmap;");
    }
    //__android_log_print(ANDROID_LOG_DEBUG, "FFNG", "SDL_Surface::SDL_Surface 1 %p %p %p", javaEnv, cls, mid);

    jstring textString = javaEnv->NewStringUTF(text);

    jobject jBitmap = javaEnv->CallStaticObjectMethod(cls, mid, font, textString, frontColor, bgColor,
                                              outlineWidth);

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

    javaEnv->DeleteLocalRef(textString);
    javaEnv->DeleteLocalRef(jBitmap);
}

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
    
    GLuint program = FFNGSurface::programCopy;
    glUseProgram(program);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    glViewport(0, 0, width, height);

    int uTexture = glGetUniformLocation(program, "uSrcTexture");
    glUniform1i(uTexture, 1);

    int uSrcSize = glGetUniformLocation(program, "uSrcSize");
    int uDstSize = glGetUniformLocation(program, "uDstSize");
    int uSrcOffset = glGetUniformLocation(program, "uSrcOffset");
    int uDstOffset = glGetUniformLocation(program, "uDstOffset");
    glUniform2f(uSrcSize, (float) source->getWidth(), (float) source->getHeight());
    glUniform2f(uDstSize, (float) width, (float) height);
    glUniform2f(uSrcOffset, (float) srcx, (float) srcy);
    glUniform2f(uDstOffset, (float) dstx, (float) dsty);

    float points[4][2] = SQUARE(dstx, dsty, srcw, srch);
    GLuint aPosition = (GLuint) glGetAttribLocation(program, "aPosition");
    glVertexAttribPointer(aPosition, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), &points[0][0]);
    glEnableVertexAttribArray(aPosition);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void SDL_Surface::blitMasked(int dstx, int dsty, const SDL_Surface *mask, Uint32 color,
                             const SDL_Surface *source) {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, source->getTexture());
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, mask->getTexture());

    GLuint program = FFNGSurface::programMasked;
    glUseProgram(program);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    glViewport(0, 0, width, height);

    int uTexture = glGetUniformLocation(program, "uSrcTexture");
    int uMask = glGetUniformLocation(program, "uMaskTexture");
    glUniform1i(uTexture, 1);
    glUniform1i(uMask, 2);

    int uSrcSize = glGetUniformLocation(program, "uSrcSize");
    int uDstSize = glGetUniformLocation(program, "uDstSize");
    int uSrcOffset = glGetUniformLocation(program, "uSrcOffset");
    int uDstOffset = glGetUniformLocation(program, "uDstOffset");
    glUniform2f(uSrcSize, (float) source->getWidth(), (float) source->getHeight());
    glUniform2f(uDstSize, (float) width, (float) height);
    glUniform2f(uSrcOffset, 0.f, 0.f);
    glUniform2f(uDstOffset, (float) dstx, (float) dsty);

    int uMaskColor = glGetUniformLocation(program, "uMaskColor");
    glUniform4f(uMaskColor, ((color >> 16) & 255) / 255.0f, ((color >> 8) & 255) / 255.0f,
                ((color >> 0) & 255) / 255.0f, ((color >> 24) & 255) / 255.0f);

    float points[4][2] = SQUARE(dstx, dsty, source->getWidth(), source->getHeight());
    GLuint aPosition = (GLuint) glGetAttribLocation(program, "aPosition");
    glVertexAttribPointer(aPosition, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), &points[0][0]);
    glEnableVertexAttribArray(aPosition);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void SDL_Surface::blitWavy(const SDL_Surface *source, int x, int y, float amp, float period,
                           float shift) {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, source->getTexture());

    GLuint program = FFNGSurface::programWavy;
    glUseProgram(program);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    glViewport(0, 0, width, height);

    int uTexture = glGetUniformLocation(program, "uSrcTexture");
    glUniform1i(uTexture, 1);

    int uSrcSize = glGetUniformLocation(program, "uSrcSize");
    int uDstSize = glGetUniformLocation(program, "uDstSize");
    int uSrcOffset = glGetUniformLocation(program, "uSrcOffset");
    int uDstOffset = glGetUniformLocation(program, "uDstOffset");
    glUniform2f(uSrcSize, (float) source->getWidth(), (float) source->getHeight());
    glUniform2f(uDstSize, (float) width, (float) height);
    glUniform2f(uSrcOffset, 0.f, 0.f);
    glUniform2f(uDstOffset, (float) x, (float) y);

    float points[4][2] = SQUARE(x, y, source->getWidth(), source->getHeight());
    GLuint aPosition = (GLuint) glGetAttribLocation(program, "aPosition");
    glVertexAttribPointer(aPosition, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), &points[0][0]);
    glEnableVertexAttribArray(aPosition);

    int uAmplitude = glGetUniformLocation(program, "uAmplitude");
    int uPeriod = glGetUniformLocation(program, "uPeriod");
    int uShift = glGetUniformLocation(program, "uShift");
    glUniform1f(uAmplitude, amp);
    glUniform1f(uPeriod, period);
    glUniform1f(uShift, shift);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void SDL_Surface::blitDisintegrate(const SDL_Surface *source, int x, int y, int disint) {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, source->getTexture());

    GLuint program = FFNGSurface::programDisintegrate;
    glUseProgram(program);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    glViewport(0, 0, width, height);

    int uTexture = glGetUniformLocation(program, "uSrcTexture");
    glUniform1i(uTexture, 1);

    int uSrcSize = glGetUniformLocation(program, "uSrcSize");
    int uDstSize = glGetUniformLocation(program, "uDstSize");
    int uSrcOffset = glGetUniformLocation(program, "uSrcOffset");
    int uDstOffset = glGetUniformLocation(program, "uDstOffset");
    glUniform2f(uSrcSize, (float) source->getWidth(), (float) source->getHeight());
    glUniform2f(uDstSize, (float) width, (float) height);
    glUniform2f(uSrcOffset, 0.f, 0.f);
    glUniform2f(uDstOffset, (float) x, (float) y);

    int uPhase = glGetUniformLocation(program, "uPhase");
    glUniform1f(uPhase, ((float) disint) / 255.0f);

    float points[4][2] = SQUARE(x, y, source->getWidth(), source->getHeight());
    GLuint aPosition = (GLuint) glGetAttribLocation(program, "aPosition");
    glVertexAttribPointer(aPosition, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), &points[0][0]);
    glEnableVertexAttribArray(aPosition);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void SDL_Surface::blitMirror(const SDL_Surface *source, int x, int y, int border) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, source->getTexture());

    GLuint program = FFNGSurface::programMirror;
    glUseProgram(program);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    glViewport(0, 0, width, height);

    int uTexture = glGetUniformLocation(program, "uSrcTexture");
    int uTarget = glGetUniformLocation(program, "uDstTexture");
    glUniform1i(uTexture, 1);
    glUniform1i(uTarget, 0);

    int uSrcSize = glGetUniformLocation(program, "uSrcSize");
    int uDstSize = glGetUniformLocation(program, "uDstSize");
    int uSrcOffset = glGetUniformLocation(program, "uSrcOffset");
    int uDstOffset = glGetUniformLocation(program, "uDstOffset");
    glUniform2f(uSrcSize, (float) source->getWidth(), (float) source->getHeight());
    glUniform2f(uDstSize, (float) width, (float) height);
    glUniform2f(uSrcOffset, 0.f, 0.f);
    glUniform2f(uDstOffset, (float) x, (float) y);

    int uBorder = glGetUniformLocation(program, "uBorder");
    glUniform1f(uBorder, (float) border);

    float points[4][2] = SQUARE(x, y, source->getWidth(), source->getHeight());
    GLuint aPosition = (GLuint) glGetAttribLocation(program, "aPosition");
    glVertexAttribPointer(aPosition, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), &points[0][0]);
    glEnableVertexAttribArray(aPosition);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void SDL_Surface::blitReverse(const SDL_Surface *source, int x, int y) {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, source->getTexture());

    GLuint program = FFNGSurface::programReverse;
    glUseProgram(program);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    glViewport(0, 0, width, height);

    int uTexture = glGetUniformLocation(program, "uSrcTexture");
    glUniform1i(uTexture, 1);

    int uSrcSize = glGetUniformLocation(program, "uSrcSize");
    int uDstSize = glGetUniformLocation(program, "uDstSize");
    int uSrcOffset = glGetUniformLocation(program, "uSrcOffset");
    int uDstOffset = glGetUniformLocation(program, "uDstOffset");
    glUniform2f(uSrcSize, (float) source->getWidth(), (float) source->getHeight());
    glUniform2f(uDstSize, (float) width, (float) height);
    glUniform2f(uSrcOffset, 0.f, 0.f);
    glUniform2f(uDstOffset, (float) x, (float) y);

    float points[4][2] = SQUARE(x, y, source->getWidth(), source->getHeight());
    GLuint aPosition = (GLuint) glGetAttribLocation(program, "aPosition");
    glVertexAttribPointer(aPosition, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), &points[0][0]);
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

    GLuint program = FFNGSurface::programZX;
    glUseProgram(program);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    glViewport(0, 0, width, height);

    int uTexture = glGetUniformLocation(program, "uSrcTexture");
    glUniform1i(uTexture, 1);

    int uSrcSize = glGetUniformLocation(program, "uSrcSize");
    int uDstSize = glGetUniformLocation(program, "uDstSize");
    int uSrcOffset = glGetUniformLocation(program, "uSrcOffset");
    int uDstOffset = glGetUniformLocation(program, "uDstOffset");
    glUniform2f(uSrcSize, (float) source->getWidth(), (float) source->getHeight());
    glUniform2f(uDstSize, (float) width, (float) height);
    glUniform2f(uSrcOffset, 0.f, 0.f);
    glUniform2f(uDstOffset, (float) x, (float) y);

    int uMaskColor = glGetUniformLocation(program, "uMaskColor");
    glUniform4f(uMaskColor, 0.5f, 0.5f, 0.5f, 1.0f);

    int uColors = glGetUniformLocation(program, "uColors");
    glUniform4fv(uColors, 2, zxColors + (zx == ZX1 || zx == ZX2 ? 0 : 8));

    int uStartY = glGetUniformLocation(program, "uStartY");
    int uPeriodY = glGetUniformLocation(program, "uPeriodY");
    if(zx == ZX2 || zx == ZX4)
        glUniform1f(uStartY, (float) (countHeight + stripeHeight));
    else
        glUniform1f(uStartY, (float) countHeight);
    glUniform1f(uPeriodY, 2 * (float) stripeHeight);

    float points[4][2] = SQUARE(x, y, source->getWidth(), source->getHeight());
    GLuint aPosition = (GLuint) glGetAttribLocation(program, "aPosition");
    glVertexAttribPointer(aPosition, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), &points[0][0]);
    glEnableVertexAttribArray(aPosition);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void SDL_Surface::line(int x1, int y1, int x2, int y2, Uint32 color) {
    GLuint program = FFNGSurface::programUniform;
    glUseProgram(program);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    glViewport(0, 0, width, height);

    int uTexture = glGetUniformLocation(program, "uSrcTexture");
    glUniform1i(uTexture, 1);

    int uDstSize = glGetUniformLocation(program, "uDstSize");
    int uSrcOffset = glGetUniformLocation(program, "uSrcOffset");
    int uDstOffset = glGetUniformLocation(program, "uDstOffset");
    glUniform2f(uDstSize, (float) width, (float) height);
    glUniform2f(uSrcOffset, 0.f, 0.f);
    glUniform2f(uDstOffset, 0.f, 0.f);

    int uColor = glGetUniformLocation(program, "uColor");
    glUniform4f(uColor, ((color >> 16) & 255) / 255.0f, ((color >> 8) & 255) / 255.0f,
                ((color >> 0) & 255) / 255.0f, ((color >> 24) & 255) / 255.0f);

    float points[2][2] = {{(float)x1, (float)y1}, {(float)x2, (float)y2}};
    GLuint aPosition = (GLuint) glGetAttribLocation(program, "aPosition");
    glVertexAttribPointer(aPosition, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), &points[0][0]);
    glEnableVertexAttribArray(aPosition);

    glDrawArrays(GL_LINES, 0, 2);
}

void SDL_Surface::fillRect(const SDL_Rect *dstRect, Uint32 color) {
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

    GLuint program = FFNGSurface::programUniform;
    glUseProgram(program);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    glViewport(0, 0, width, height);

    int uTexture = glGetUniformLocation(program, "uSrcTexture");
    glUniform1i(uTexture, 1);

    int uDstSize = glGetUniformLocation(program, "uDstSize");
    int uSrcOffset = glGetUniformLocation(program, "uSrcOffset");
    int uDstOffset = glGetUniformLocation(program, "uDstOffset");
    glUniform2f(uDstSize, (float) width, (float) height);
    glUniform2f(uSrcOffset, (float)x_, (float)y_);
    glUniform2f(uDstOffset, (float)x_, (float)y_);

    int uColor = glGetUniformLocation(program, "uColor");
    glUniform4f(uColor, ((color >> 16) & 255) / 255.0f, ((color >> 8) & 255) / 255.0f,
                ((color >> 0) & 255) / 255.0f, ((color >> 24) & 255) / 255.0f);

    float points[4][2] = SQUARE(x_, y_, w_, h_);
    GLuint aPosition = (GLuint) glGetAttribLocation(program, "aPosition");
    glVertexAttribPointer(aPosition, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), &points[0][0]);
    glEnableVertexAttribArray(aPosition);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void SDL_Surface::filledCircleColor(int x, int y, int radius, Uint32 color) {
    GLuint program = FFNGSurface::programCircle;
    glUseProgram(program);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    glViewport(0, 0, width, height);

    int uTexture = glGetUniformLocation(program, "uSrcTexture");
    glUniform1i(uTexture, 1);

    int uDstSize = glGetUniformLocation(program, "uDstSize");
    int uSrcOffset = glGetUniformLocation(program, "uSrcOffset");
    int uDstOffset = glGetUniformLocation(program, "uDstOffset");
    glUniform2f(uDstSize, (float) width, (float) height);
    glUniform2f(uSrcOffset, (float)(x - radius), (float)(y - radius));
    glUniform2f(uDstOffset, (float)(x - radius), (float)(y - radius));

    int uCenter = glGetUniformLocation(program, "uCenter");
    int uRadius = glGetUniformLocation(program, "uRadius");
    int uColor = glGetUniformLocation(program, "uColor");
    glUniform2f(uCenter, (float)x, (float)y);
    glUniform1f(uRadius, (float)radius);
    glUniform4f(uColor, ((color >> 16) & 255) / 255.0f, ((color >> 8) & 255) / 255.0f,
                ((color >> 0) & 255) / 255.0f, ((color >> 24) & 255) / 255.0f);

    float points[4][2] = SQUARE(x - radius, y - radius, 2*radius, 2*radius);
    GLuint aPosition = (GLuint) glGetAttribLocation(program, "aPosition");
    glVertexAttribPointer(aPosition, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), &points[0][0]);
    glEnableVertexAttribArray(aPosition);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

Uint32 SDL_Surface::getPixel(int x, int y) const {
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

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
    glBindFramebuffer(GL_FRAMEBUFFER, FFNGSurface::framebuffer);
}

void FFNGSurface::termEGL() {
    eglMakeCurrent(FFNGSurface::dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglTerminate(FFNGSurface::dpy);
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
    std::string vertexUnitedSource = R"(
precision highp float;

attribute vec2 aPosition;

uniform vec2 uSrcOffset;
uniform vec2 uDstOffset;
uniform vec2 uDstSize;

varying vec2 vPixCoords;

void main() {
  vPixCoords = aPosition - uDstOffset + uSrcOffset;
  vec2 lClipCoords = aPosition / uDstSize * 2.0 - vec2(1.0, 1.0);
  gl_Position = vec4(lClipCoords, 0.0, 1.0);
})";

    std::string fragmentCopySource = R"(
precision highp float;

uniform sampler2D uSrcTexture;
uniform vec2 uSrcSize;

varying vec2 vPixCoords;

void main(void)
{
  gl_FragColor = texture2D(uSrcTexture, vPixCoords / uSrcSize);
})";

    std::string fragmentReverseSource = R"(
precision highp float;

uniform sampler2D uSrcTexture;
uniform vec2 uSrcSize;

varying vec2 vPixCoords;

void main(void)
{
  vec2 lTexCoords = vPixCoords / uSrcSize;
  gl_FragColor = texture2D(uSrcTexture, vec2(1.0 - lTexCoords.x, lTexCoords.y));
})";

    std::string fragmentMaskedSource = R"(
precision highp float;

uniform sampler2D uSrcTexture;
uniform sampler2D uMaskTexture;
uniform vec2 uSrcSize;
uniform vec4 uMaskColor;

varying vec2 vPixCoords;

void main(void)
{
  vec2 lTexCoords = vPixCoords / uSrcSize;
  gl_FragColor = distance(texture2D(uMaskTexture, lTexCoords), uMaskColor) < 0.1 ? texture2D(uSrcTexture, lTexCoords) : vec4(0.0);
})";

    std::string fragmentMirrorSource = R"(
precision highp float;

uniform sampler2D uSrcTexture;
uniform sampler2D uDstTexture;
uniform vec2 uDstOffset;
uniform vec2 uSrcSize;
uniform vec2 uDstSize;
uniform float uBorder;

varying vec2 vPixCoords;

void main(void)
{
  vec2 lPixMirrored = uDstOffset + vec2(-1.0, 1.0) * vPixCoords + vec2(uBorder, 0.0);
  vec2 lTexCoords = vPixCoords / uSrcSize;
  vec4 lTexColor = texture2D(uSrcTexture, lTexCoords);
  vec4 lMaskColor = texture2D(uSrcTexture, vec2(0.5, 0.5));
  gl_FragColor = distance(lTexColor, lMaskColor) < 0.1
    ? texture2D(uDstTexture, lPixMirrored / uDstSize)
    : lTexColor;
})";

    std::string fragmentWavySource = R"(
precision highp float;

uniform sampler2D uSrcTexture;
uniform vec2 uSrcSize;
uniform float uAmplitude;
uniform float uShift;
uniform float uPeriod;

varying vec2 vPixCoords;

void main(void)
{
  float dx = uAmplitude * sin(vPixCoords.y / uPeriod + uShift);
  gl_FragColor = texture2D(uSrcTexture, (vPixCoords + vec2(dx, 0.0)) / uSrcSize);
})";

    std::string fragmentDisintegrateSource = R"(
precision highp float;

uniform sampler2D uSrcTexture;
uniform vec2 uSrcSize;
uniform float uPhase;

varying vec2 vPixCoords;

float noise(float x, float y) {
  return fract(sin(mod(x + 1.618 * y, 6.28)) * 100.0);
}

void main(void)
{
  gl_FragColor = noise(vPixCoords.x, vPixCoords.y) < uPhase
    ? texture2D(uSrcTexture, vPixCoords / uSrcSize)
    : vec4(0.0);
})";

    std::string fragmentZXSource = R"(
precision highp float;

uniform sampler2D uSrcTexture;
uniform vec2 uSrcSize;
uniform vec4 uMaskColor;
uniform vec4 uColors[2];
uniform float uStartY;
uniform float uPeriodY;

varying vec2 vPixCoords;

void main(void)
{
  gl_FragColor = distance(texture2D(uSrcTexture, vPixCoords / uSrcSize), uMaskColor) < 0.1
    ? (fract((vPixCoords.y + uStartY) / uPeriodY) < 0.5 ? uColors[0] : uColors[1])
    : vec4(0.0);
})";

    std::string fragmentUniformSource = R"(
precision highp float;

uniform vec4 uColor;

void main(void)
{
  gl_FragColor = uColor;
})";

    std::string fragmentCircleSource = R"(
precision highp float;

uniform vec2 uCenter;
uniform float uRadius;
uniform vec4 uColor;

varying vec2 vPixCoords;

void main(void)
{
  gl_FragColor = distance(vPixCoords, uCenter) < uRadius
    ? uColor
    : vec4(0.0);
})";

    GLuint vertexCommon{loadShader(GL_VERTEX_SHADER, vertexUnitedSource)};

    GLuint fragmentCopy{loadShader(GL_FRAGMENT_SHADER, fragmentCopySource)};
    GLuint fragmentMasked{loadShader(GL_FRAGMENT_SHADER, fragmentMaskedSource)};
    GLuint fragmentReverse{loadShader(GL_FRAGMENT_SHADER, fragmentReverseSource)};
    GLuint fragmentMirror{loadShader(GL_FRAGMENT_SHADER, fragmentMirrorSource)};
    GLuint fragmentWavy{loadShader(GL_FRAGMENT_SHADER, fragmentWavySource)};
    GLuint fragmentDisintegrate{loadShader(GL_FRAGMENT_SHADER, fragmentDisintegrateSource)};
    GLuint fragmentZX{loadShader(GL_FRAGMENT_SHADER, fragmentZXSource)};

    GLuint fragmentUniform{loadShader(GL_FRAGMENT_SHADER, fragmentUniformSource)};
    GLuint fragmentCircle{loadShader(GL_FRAGMENT_SHADER, fragmentCircleSource)};

    programCopy = createProgram(vertexCommon, fragmentCopy);

    programMasked = createProgram(vertexCommon, fragmentMasked);
    programReverse = createProgram(vertexCommon, fragmentReverse);
    programMirror = createProgram(vertexCommon, fragmentMirror);
    programWavy = createProgram(vertexCommon, fragmentWavy);
    programDisintegrate = createProgram(vertexCommon, fragmentDisintegrate);
    programZX = createProgram(vertexCommon, fragmentZX);

    programUniform = createProgram(vertexCommon, fragmentUniform);
    programCircle = createProgram(vertexCommon, fragmentCircle);
}
