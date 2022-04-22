#ifndef HEADER_WISTATUSBAR_H
#define HEADER_WISTATUSBAR_H

class Font;

#include "IWidget.h"

//FFNG #include "SDL.h"
#include "Color.h"
#include <string>

/**
 * Statusbar with fixed width.
 */
class WiStatusBar : public IWidget {
    private:
        Font *m_font;
        Color m_color;
        int m_w;
        std::string m_label;
    public:
        WiStatusBar(Font *new_font, const Color &color, int width=0);
        virtual ~WiStatusBar();
        virtual int getW() const { return m_w; }
        virtual int getH() const;
        void setLabel(const std::string &label) { m_label = label; }

        virtual void drawOn(SDL_Surface *screen);
};

#endif
