#ifndef HEADER_RADIOBOX_H
#define HEADER_RADIOBOX_H

class File;

#include "WiContainer.h"

/**
 * Radio box with picture background.
 */
class RadioBox : public WiContainer {
    private:
        static const int BORDER = 4;
        std::string m_param;
        std::string m_value;
    protected:
        virtual void own_mouseButton(const MouseStroke &stroke);
    public:
        RadioBox(const std::string &param, const std::string &value,
                const File &picture);

        virtual void drawOn(SDL_Surface *screen);
};

#endif
