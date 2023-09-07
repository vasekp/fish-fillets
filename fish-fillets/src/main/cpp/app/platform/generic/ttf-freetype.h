#ifndef FISH_FILLETS_LINUX_FONT_H
#define FISH_FILLETS_LINUX_FONT_H

#include "common.h"
#include "subsystem/graphics.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_STROKER_H

class FTFont : public IFont {
public:
    FTFont(Instance& instance, const std::string& filename);
    ~FTFont();

private:
    FT_Library m_ft;
    FT_Face m_face;
    FT_Stroker m_stroker;
    using ManagedGlyph = std::unique_ptr<FT_GlyphRec, void(*)(FT_GlyphRec*)>;
    mutable std::map<wchar_t, std::pair<ManagedGlyph, ManagedGlyph>> m_cache;

    void own_setSizes() override;
    std::vector<std::string> own_breakLines(const std::string& text, float width) override;
    ImageData own_renderText(const std::string& text) const override;
    std::pair<FT_Glyph, FT_Glyph> getGlyphs(wchar_t c) const;
};

namespace decoders {
    std::unique_ptr<IFont> ttf(Instance& instance, const std::string& filename) {
        return std::make_unique<FTFont>(instance, filename);
    }
}

#endif //FISH_FILLETS_LINUX_FONT_H
