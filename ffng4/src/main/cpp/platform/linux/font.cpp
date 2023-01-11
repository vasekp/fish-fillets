#include "subsystem/graphics.h"
#include "subsystem/files.h"
#include "./files.h"
#include <locale>
#include <codecvt>

static FT_F26Dot6 to266(float x) {
    return (FT_F26Dot6)(x * 64.f);
}

static float from266(FT_F26Dot6 x) {
    return (float)x / 64.f;
}

Font::Font(Instance& instance, const std::string& filename) :
        m_instance(instance),
        m_fontSize(), m_outline()
{
    auto fnFull = dynamic_cast<SystemFile&>(*instance.files().system(filename)).fullPath();
    if(FT_Init_FreeType(&m_ft) != 0)
        Log::fatal("Can't initiate FreeType.");

    if(FT_New_Face(m_ft, fnFull.c_str(), 0, &m_face) != 0)
        Log::fatal("Error loading font ", fnFull);
}

void Font::setSizes(float fontSize, float outline /* TODO dpi */) {
    m_fontSize = fontSize;
    m_outline = outline;
    if(FT_Set_Char_Size(m_face, to266(/* TODO arbitrary upscale */ 2.f * fontSize), 0, 90, 0) != 0)
        Log::fatal("FT_Set_Char_Size failed");
    m_fontSize = 2.f * fontSize * 90.f / 72.f;
    m_outline = 2.f * outline * 90.f / 72.f;
    Log::debug("fontSize ", m_fontSize, " outline ", m_outline);
}

std::vector<std::string> Font::breakLines(const std::string& text, float width) {
    // TODO
    return {text};
}

ogl::Texture Font::renderText(const std::string& text) const {
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

    auto width = (int)(fwidth + 2.f * m_outline) + 2;
    auto height = (int)(asc + desc + 2.f * m_outline) + 2;
    Log::debug("bitmap size ", width, "x", height);
    auto pixels = std::make_unique<std::array<std::uint8_t, 4>[]>(width * height);
    auto data = pixels.get();

    auto blend = [&](const FT_Bitmap& bitmap, int x0, int y0, bool white) {
        int x, y, sx, sy;
        for(y = std::max(y0, 1), sy = -std::min(y0, 0); y < height - 1 && sy < (int)bitmap.rows; y++, sy++)
            for(x = std::max(x0, 1), sx = -std::min(x0, 0); x < width - 1 && sx < (int)bitmap.width; x++, sx++) {
                auto& out = data[y * width + x];
                if(white)
                    for(int i = 0; i < 4; i++)
                        out[i] = std::max(out[i], bitmap.buffer[sy * bitmap.pitch + sx]);
                else
                    out[3] = std::max(out[3], bitmap.buffer[sy * bitmap.pitch + sx]);
            }
    };

    FCoords origin{m_outline + 1.f, m_outline + asc + 1.f};
    if(m_outline) {
        auto pen = origin;
        FT_Stroker stroker;
        FT_Stroker_New(m_ft, &stroker);
        FT_Stroker_Set(stroker, to266(m_outline), FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);

        for(auto i = 0u; i < wtext.size(); i++) {
            if(FT_Load_Char(m_face, wtext[i], 0) != 0) {
                Log::error("Can't load character ", converter.to_bytes(wtext[i]));
                continue;
            }
            auto slot = m_face->glyph;
            FT_Glyph glyph;
            if(FT_Get_Glyph(slot, &glyph) != 0)
                Log::error("FT_Get_Glyph");
            if(FT_Glyph_StrokeBorder(&glyph, stroker, 0, 1) != 0)
                Log::error("FT_Glyph_StrokeBorder");
            FT_Vector vector{};
            if(FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, &vector, 1) != 0)
                Log::error("FT_Glyph_To_Bitmap");
            auto bmpGlyph = reinterpret_cast<FT_BitmapGlyph>(glyph);
            Log::debug("glyph ", converter.to_bytes(wtext[i]),
                    " at ", pen, " ", bmpGlyph->left, " ", bmpGlyph->top,
                    " ", bmpGlyph->bitmap.width, "x", bmpGlyph->bitmap.rows,
                    " advance ", from266(slot->advance.x), ", ", from266(slot->advance.y));
            blend(bmpGlyph->bitmap, pen.x() + bmpGlyph->left, pen.y() - bmpGlyph->top, false);
            FT_Done_Glyph(glyph);
            pen += FCoords{from266(slot->advance.x), from266(slot->advance.y)};
        }
        FT_Stroker_Done(stroker);
    }

    {
        auto pen = origin;
        for(auto i = 0u; i < wtext.size(); i++) {
            if(FT_Load_Char(m_face, wtext[i], FT_LOAD_RENDER) != 0) {
                Log::error("Can't load character ", converter.to_bytes(wtext[i]));
                continue;
            }
            auto slot = m_face->glyph;
            Log::verbose("glyph ", converter.to_bytes(wtext[i]),
                    " at ", pen, " ", slot->bitmap_left, " ", slot->bitmap_top);
            blend(slot->bitmap, pen.x() + slot->bitmap_left, pen.y() - slot->bitmap_top, true);
            pen += FCoords{from266(slot->advance.x), from266(slot->advance.y)};
        }
    }

    return ogl::Texture::fromImageData(m_instance.graphics().system().ref(), width, height, 4 * width, (void*)data);
}
