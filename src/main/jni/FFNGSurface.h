#ifndef FFNGSURFACE_H_
#define FFNGSURFACE_H_

#include <string>
#include "jnix.h"
#include "FFNGTypes.h"
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <vector>
#include <array>
#include <math.h>

constexpr int MaxWidth = 900;
constexpr int MaxHeight = 900;

struct SDL_Rect {
    int x, y;
    int w, h;

    SDL_Rect();

    SDL_Rect(int x, int y, int w, int h);
};

struct CoordPair {
    float x;
    float y;

    constexpr CoordPair() : x(0), y(0) { }
    constexpr CoordPair(float x_, float y_): x(x_), y(y_) { }

    CoordPair operator*(float scalar) const { return {x * scalar, y * scalar}; }
    CoordPair operator/(float scalar) const { return {x / scalar, y / scalar}; }
    friend CoordPair operator*(float scalar, const CoordPair& cp) { return cp * scalar; }
    CoordPair& operator*=(float scalar) { x *= scalar; y *= scalar; return *this; }
    CoordPair& operator/=(float scalar) { x /= scalar; y /= scalar; return *this; }
    CoordPair operator+(const CoordPair& other) const { return {x + other.x, y + other.y}; }
    CoordPair operator-(const CoordPair& other) const { return {x - other.x, y - other.y}; }
    CoordPair& operator+=(const CoordPair& other) { x += other.x; y += other.y; return *this; }
    CoordPair& operator-=(const CoordPair& other) { x -= other.x; y -= other.y; return *this; }

    CoordPair perp() const { return {y, -x}; }
    float norm() const { return hypot(x, y); }
    CoordPair normal() const { return *this / norm(); }
    CoordPair& normalize() { *this /= norm(); return *this; }
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

    SDL_Surface(jobject font, const char *text, int frontColor, int bgColor, float outlineWidth = 0.0f);

    ~SDL_Surface();

    GLuint getTexture() const;

    void blit(int dstx, int dsty, SDL_Surface *source, int srcx, int srcy, int srcw, int srch);

    void
    blitMasked(int dstx, int dsty, const SDL_Surface *mask, Uint32 color, const SDL_Surface *source);

    void blitWavy(const SDL_Surface *source, int x, int y, float amp, float period, float shift);

    void blitWavyText(const SDL_Surface *source, int x, int y, float amp, float period, float shift);

    void blitDisintegrate(const SDL_Surface *source, int x, int y, int disint);

    void blitMirror(const SDL_Surface *source, int x, int y, int border);

    void blitReverse(const SDL_Surface *source, int x, int y);

    void blitZX(const SDL_Surface *source, int x, int y, int zx, int countHeight, int stripeHeight);

    void line(int x1, int y1, int x2, int y2, Uint32 color);

    void fillRect(const SDL_Rect *dstRect, Uint32 pixel);

    void filledCircleColor(int x, int y, int radius, Uint32 colorRGBA);

    void curve(std::vector<CoordPair> points, float width, std::array<float, 3> dirLight, Uint32 colorLow, Uint32 colorHigh);

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

    /* trivial shader */
    static GLuint programCopy;
    /* effects */
    static GLuint programMasked;
    static GLuint programReverse;
    static GLuint programMirror;
    static GLuint programWavy;
    static GLuint programWavyText;
    static GLuint programDisintegrate;
    static GLuint programZX;
    /* geometrical shapes */
    static GLuint programUniform;
    static GLuint programCircle;
    static GLuint programCurve;

    static void initEGL();
    static void initShaders();
    static void termEGL();

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
