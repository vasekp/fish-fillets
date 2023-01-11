#ifndef FISH_FILLETS_LINUX_FONT_H
#define FISH_FILLETS_LINUX_FONT_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_STROKER_H

class Instance;

class Font {
    Instance& m_instance;
    FT_Library m_ft;
    FT_Face m_face;
    float m_fontSize;
    float m_outline;

public:
    Font(Instance& instance, const std::string& filename);

    void setSizes(float fontSize, float outline);
    std::vector<std::string> breakLines(const std::string& text, float width);
    ogl::Texture renderText(const std::string& text) const;
};

#endif //FISH_FILLETS_LINUX_FONT_H
