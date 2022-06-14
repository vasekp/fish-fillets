#include "font.h"
#include "subsystem/graphics.h"
#include "subsystem/files.h"
#include <locale>
#include <codecvt>

static FT_F26Dot6 to266(float x) {
    return (FT_F26Dot6)(x * 64.f);
}

static float from266(FT_F26Dot6 x) {
    return (float)x / 64.f;
}

FFont::FFont(Instance& instance, const std::string& filename) :
        m_instance(instance),
        m_fontSize(), m_outline()
{
    auto fnFull = instance.files().system(filename).fullPath();
    if(FT_Init_FreeType(&m_ft) != 0)
        Log::fatal("Can't initiate FreeType.");

    if(FT_New_Face(m_ft, fnFull.c_str(), 0, &m_face) != 0)
        Log::fatal("Error loading font ", fnFull);
}

void FFont::setSizes(float fontSize, float outline /* TODO dpi */) {
    m_fontSize = fontSize;
    m_outline = outline;
    if(FT_Set_Char_Size(m_face, to266(/* TODO arbitrary upscale */ 2.f * fontSize), 0, 90, 0) != 0)
        Log::fatal("FT_Set_Char_Size failed");
}

std::vector<std::string> FFont::breakLines(const std::string& text, float width) {
    // TODO
    return {text};
}

ogl::Texture FFont::renderText(const std::string& text) const {
    float asc = from266(m_face->size->metrics.ascender);
    float desc = -from266(m_face->size->metrics.descender);
    Log::debug("EM ", m_face->size->metrics.y_ppem, " asc ", asc, " desc ", desc);

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter{};
    auto wtext = converter.from_bytes(text);
    float fwidth = 0.f;

    for(auto i = 0u; i < wtext.size(); i++) {
        if(FT_Load_Char(m_face, wtext[i], 0) != 0) {
            Log::error("Can't load character ", converter.to_bytes(wtext[i]));
            continue;
        }
        auto slot = m_face->glyph;
        fwidth += from266(slot->advance.x);
    }

    auto width = (int)fwidth + 2;
    auto height = (int)(asc + desc) + 2;
    auto pixels = std::make_unique<std::array<std::uint8_t, 4>[]>(width * height);
    auto data = pixels.get();

    auto blend = [&](const FT_Bitmap& bitmap, int x0, int y0) {
        int x, y, sx, sy;
        for(y = std::max(y0, 0), sy = -std::min(y0, 0); y < height && sy < (int)bitmap.rows; y++, sy++)
            for(x = std::max(x0, 0), sx = -std::min(x0, 0); x < width && sx < (int)bitmap.width; x++, sx++) {
                auto& out = data[y * width + x];
                for(int i = 0; i < 4; i++)
                    out[i] = std::max(out[i], bitmap.buffer[sy * bitmap.pitch + sx]);
            }
    };

    FCoords pen{1.f, asc + 1.f};
    for(auto i = 0u; i < wtext.size(); i++) {
        if(FT_Load_Char(m_face, wtext[i], FT_LOAD_RENDER) != 0) {
            Log::error("Can't load character ", converter.to_bytes(wtext[i]));
            continue;
        }
        auto slot = m_face->glyph;
        Log::verbose("glyph ", converter.to_bytes(wtext[i]),
                " at ", pen, " ", slot->bitmap_left, " ", slot->bitmap_top);
        blend(slot->bitmap, pen.x() + slot->bitmap_left, pen.y() - slot->bitmap_top);
        pen += FCoords{from266(slot->advance.x), from266(slot->advance.y)};
    }

    return ogl::Texture::fromImageData(m_instance.graphics().system().ref(), width, height, 4 * width, (void*)data);
}
