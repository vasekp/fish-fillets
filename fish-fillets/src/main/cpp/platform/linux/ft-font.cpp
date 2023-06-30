#include "ft-font.h"
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

FTFont::FTFont(Instance& instance, const std::string& filename) :
        m_instance(instance),
        m_fontSize(), m_outline()
{
    auto fnFull = dynamic_cast<SystemFile&>(*instance.files().system(filename)).fullPath();
    if(FT_Init_FreeType(&m_ft) != 0)
        Log::fatal("Can't initiate FreeType.");

    if(FT_New_Face(m_ft, fnFull.c_str(), 0, &m_face) != 0)
        Log::fatal("Error loading font ", fnFull);
}

void FTFont::setSizes(float fontSize, float outline) {
    m_fontSize = fontSize;
    m_outline = outline;
    if(FT_Set_Char_Size(m_face, to266(fontSize), 0, 72, 0) != 0)
        Log::fatal("FT_Set_Char_Size failed");
    m_fontSize = fontSize;
    m_outline = outline;
    Log::debug("fontSize ", m_fontSize, " outline ", m_outline);
}

std::vector<std::string> FTFont::breakLines(const std::string& text, float width) {
    std::vector<std::size_t> breaks{};
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter{};
    auto wtext = converter.from_bytes(text);

    float fwidth = 0.f;
    std::size_t lastBreak = 0;
    float lastBreakX = 0;

    for(auto i = 0u; i < wtext.size(); i++) {
        auto c = wtext[i];
        if(FT_Load_Char(m_face, wtext[i], 0) != 0) {
            Log::error("Can't load character ", converter.to_bytes(wtext[i]));
            continue;
        }
        auto slot = m_face->glyph;
        fwidth += from266(slot->advance.x);
        if(c == L' ' || c == '\n') {
            lastBreak = i;
            lastBreakX = fwidth;
        }
        if(c == '\n' || fwidth > width) {
            breaks.push_back(lastBreak);
            fwidth -= lastBreakX;
        }
    }

    std::vector<std::string> ret{};
    std::size_t prev = 0;
    for(auto brk : breaks) {
        ret.push_back(converter.to_bytes(wtext.substr(prev, brk - prev)));
        prev = brk + 1;
    }
    ret.push_back(converter.to_bytes(wtext.substr(prev)));
    return ret;
}

ogl::Texture FTFont::renderText(const std::string& text) const {
    float asc = from266(m_face->size->metrics.ascender);
    float desc = -from266(m_face->size->metrics.descender);
    Log::debug("EM ", m_face->size->metrics.y_ppem, " asc ", asc, " desc ", desc);

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter{};
    auto wtext = converter.from_bytes(text);
    float fwidth = 0.f;

    for(auto c : wtext) {
        if(FT_Load_Char(m_face, c, 0) != 0) {
            Log::error("Can't load character ", converter.to_bytes(c));
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

        for(auto c : wtext) {
            if(FT_Load_Char(m_face, c, 0) != 0) {
                Log::error("Can't load character ", converter.to_bytes(c));
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
            Log::verbose("glyph ", converter.to_bytes(c),
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
        for(auto c : wtext) {
            if(FT_Load_Char(m_face, c, FT_LOAD_RENDER) != 0) {
                Log::error("Can't load character ", converter.to_bytes(c));
                continue;
            }
            auto slot = m_face->glyph;
            blend(slot->bitmap, pen.x() + slot->bitmap_left, pen.y() - slot->bitmap_top, true);
            pen += FCoords{from266(slot->advance.x), from266(slot->advance.y)};
        }
    }

    return ogl::Texture::fromImageData(m_instance.graphics().system().ref(), width, height, 4 * width, (void*)data);
}
