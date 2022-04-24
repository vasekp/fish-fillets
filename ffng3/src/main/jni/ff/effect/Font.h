#ifndef HEADER_FONT_H
#define HEADER_FONT_H

class File;

#include "NoCopy.h"
#include "Color.h"

//FFNG #include "SDL.h"
//FFNG #include "SDL_ttf.h"
#include "FFNGSurface.h"
#include "FFNGFont.h"

#include <string>

/**
 * TrueType UTF-8 font.
 */
class Font : public NoCopy {
    private:
        TTF_Font *m_ttfont;
        Color m_bg;
    private:
        static std::string biditize(const std::string &text);
    public:
        Font(const File &file_ttf, int height);
        ~Font();
        static void init();
        static void shutdown();

        int getHeight() { return /* FFNG TTF_FontHeight(m_ttfont)*/ m_ttfont->fontHeight();  }
        int calcTextWidth(const std::string &text);
        SDL_Surface *renderText(const std::string &text,
                const Color &color) const;
        SDL_Surface *renderTextOutlined(const std::string &text,
                const Color &color, float outlineWidth=1.0f) const;
};

#endif
