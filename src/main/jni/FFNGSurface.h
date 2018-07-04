#ifndef FFNGSURFACE_H_
#define FFNGSURFACE_H_

#include <string>
#include "jnix.h"
#include "FFNGTypes.h"
#include <EGL/egl.h>
#include <GLES2/gl2.h>

constexpr int MaxWidth = 900;
constexpr int MaxHeight = 900;

struct SDL_Rect {
    int x, y;
    int w, h;

    SDL_Rect();

    SDL_Rect(int x, int y, int w, int h);
};

struct SDL_Surface {
    int width, height;
    GLuint texture;

private:
    SDL_Surface();

public:
    SDL_Surface(int width, int height, Uint32 colorkey = 0x000000ff, bool transparent = false);

    SDL_Surface(const char *path);

    SDL_Surface(const std::string &path);

    SDL_Surface(jobject font, const char *text, int frontColor, int bgColor, int outlineWidth = 0);

    ~SDL_Surface();

    GLuint getTexture() const;

    void blit(int dstx, int dsty, SDL_Surface *source, int srcx, int srcy, int srcw, int srch);

    void
    blitMasked(int dstx, int dsty, const SDL_Surface *mask, Uint32 color, const SDL_Surface *source);

    void blitWavy(const SDL_Surface *source, int x, int y, float amp, float period, float shift);

    void blitDisintegrate(const SDL_Surface *source, int x, int y, int disint);

    void blitMirror(const SDL_Surface *source, int x, int y, int border);

    void blitReverse(const SDL_Surface *source, int x, int y);

    void blitZX(const SDL_Surface *source, int x, int y, int zx, int countHeight, int stripeHeight);

    void line(int x1, int y1, int x2, int y2, Uint32 color);

    void fillRect(const SDL_Rect *dstRect, Uint32 pixel);

    void filledCircleColor(int x, int y, int radius, Uint32 colorRGBA);

    Uint32 getPixel(int x, int y) const;

    int getWidth() const;

    int getHeight() const;
};

class FFNGSurface {
public:
    static EGLContext ctx;
    static EGLDisplay dpy;
    static EGLSurface sfc;

    static GLuint framebuffer;
    static GLuint programUniform;
    static GLuint programCircle;

    static GLuint programUCopy;
    static GLuint programUMasked;
    static GLuint programUReverse;
    static GLuint programUMirror;
    static GLuint programUWavy;
    static GLuint programUDisintegrate;
    static GLuint programUZX;

    static void initEGL();

    static void initShaders();

    static SDL_Surface *imgLoad(const char *file);

    static void freeSurface(SDL_Surface *surface);

    static void
    blitSurface(SDL_Surface *srcSurface, const SDL_Rect *srcRect, SDL_Surface *dstSurface,
                const SDL_Rect *dstRect);

    static Uint32 getPixel(SDL_Surface *surface, int x, int y);

    static SDL_Surface *createSurface(int width, int height, int colorkey = 0xff000000);

    static void fillRect(SDL_Surface *surface, const SDL_Rect *dstRect, Uint32 pixel);

    static void filledCircleColor(SDL_Surface *screen, int x, int y, int radius, Uint32 colorRGBA);

    static void lineColor(SDL_Surface *screen, int x1, int y1, int x2, int y2, Uint32 colorRGBA);
};


#endif /* FFNGSURFACE_H_ */
