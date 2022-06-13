#ifndef FISH_FILLETS_ANDROID_FONT_H
#define FISH_FILLETS_ANDROID_FONT_H

#include "api/ogl.h"

class Instance;

class Font {
    Instance& m_instance;
    std::string m_filename;
    float m_fontSize;
    float m_outline;

public:
    Font(Instance& instance, const std::string& filename);

    void setSizes(float fontSize, float outline);
    std::vector<std::string> breakLines(const std::string& text, float width);
    ogl::Texture renderText(const std::string& text) const;
};

#endif //FISH_FILLETS_ANDROID_FONT_H
