#ifndef FISH_FILLETS_LINUX_FONT_H
#define FISH_FILLETS_LINUX_FONT_H

#include "subsystem/graphics.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_STROKER_H

class FTFont : public IFont {
    Instance& m_instance;
    FT_Library m_ft;
    FT_Face m_face;
    float m_fontSize;
    float m_outline;

public:
    FTFont(Instance& instance, const std::string& filename);

    void setSizes(float fontSize, float outline) override;
    std::vector<std::string> breakLines(const std::string& text, float width) override;
    ogl::Texture renderText(const std::string& text) const override;
};

namespace decoders {
    std::unique_ptr<IFont> ttf(Instance& instance, const std::string& filename) {
        return std::make_unique<FTFont>(instance, filename);
    }
}

#endif //FISH_FILLETS_LINUX_FONT_H
