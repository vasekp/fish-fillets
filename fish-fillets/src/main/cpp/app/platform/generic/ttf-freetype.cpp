#include "ttf-freetype.h"
#include "subsystem/files.h"
#include <codecvt>

static FT_F26Dot6 to266(float x) {
    return (FT_F26Dot6)(x * 64.f);
}

static float from266(FT_F26Dot6 x) {
    return (float)x / 64.f;
}

static float from1616(FT_F26Dot6 x) {
    return (float)x / 65536.f;
}

FTFont::FTFont(Instance& instance, const std::string& filename) :
    IFont(instance),
    m_stroker()
{
    auto fnFull = instance.files().system(filename)->fullPath();
    if(FT_Init_FreeType(&m_ft) != 0)
        Log::fatal("Can't initiate FreeType.");

    if(FT_New_Face(m_ft, fnFull.c_str(), 0, &m_face) != 0)
        Log::fatal("Error loading font ", fnFull);

    FT_Stroker_New(m_ft, &m_stroker);
}

FTFont::~FTFont() {
    m_cache.clear();
    FT_Stroker_Done(m_stroker);
    FT_Done_Face(m_face);
    FT_Done_FreeType(m_ft);
}

void FTFont::own_setSizes() {
    if(FT_Set_Char_Size(m_face, to266(m_fontSize), 0, 72, 0) != 0)
        Log::fatal("FT_Set_Char_Size failed");
    Log::debug<Log::graphics>("EM ", m_face->size->metrics.y_ppem,
            " asc ", from266(m_face->size->metrics.ascender),
            " desc ", -from266(m_face->size->metrics.descender));
    if(m_outline)
        FT_Stroker_Set(m_stroker, to266(m_outline), FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);
    m_cache.clear();
}

std::pair<FT_Glyph, FT_Glyph> FTFont::getGlyphs(wchar_t c) const {
    if(auto it = m_cache.find(c); it != m_cache.end()) {
        return {it->second.first.get(), it->second.second.get()};
    }
    auto mgdGlyph = [](FT_Glyph g) {
        return ManagedGlyph{g, &FT_Done_Glyph};
    };
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter{};
    if(FT_Load_Char(m_face, c, FT_LOAD_DEFAULT) != 0) {
        Log::error("Can't load character ", converter.to_bytes(c));
        m_cache.insert_or_assign(c, std::pair{mgdGlyph(nullptr), mgdGlyph(nullptr)});
        return {nullptr, nullptr};
    }
    FT_Glyph glyph, outGlyph = nullptr;
    FT_Vector vector{};
    if(FT_Get_Glyph(m_face->glyph, &glyph) != 0)
        Log::error("FT_Get_Glyph");
    if(m_outline) {
        outGlyph = glyph;
        if(FT_Glyph_StrokeBorder(&outGlyph, m_stroker, 0, 0) != 0)
            Log::error("FT_Glyph_StrokeBorder");
        if(FT_Glyph_To_Bitmap(&outGlyph, FT_RENDER_MODE_NORMAL, &vector, 1) != 0)
            Log::error("FT_Glyph_To_Bitmap");
    }
    if(FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, &vector, 1) != 0)
        Log::error("FT_Glyph_To_Bitmap");
    auto it = m_cache.insert_or_assign(c, std::pair{mgdGlyph(glyph), mgdGlyph(outGlyph)}).first;
    return {it->second.first.get(), it->second.second.get()};
}

std::vector<std::string> FTFont::own_breakLines(const std::string& text, float width) {
    std::vector<std::size_t> breaks{};
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter{};
    auto wtext = converter.from_bytes(text);

    width *= 0.98; // leave some legroom

    float fwidth = 0.f;
    std::size_t lastBreak = 0;
    float lastBreakX = 0;

    for(auto i = 0u; i < wtext.size(); i++) {
        auto c = wtext[i];
        auto glyph = getGlyphs(c).first;
        if(!glyph)
            continue;
        fwidth += from1616(glyph->advance.x);
        if(c == L' ' || c == L'\n') {
            lastBreak = i;
            lastBreakX = fwidth;
        }
        if(c == L'\n' || fwidth > width) {
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

ImageData FTFont::own_renderText(const std::string& text) const {
    float asc = from266(m_face->size->metrics.ascender);
    float desc = -from266(m_face->size->metrics.descender);

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter{};
    auto wtext = converter.from_bytes(text);
    float fwidth = 0.f;

    for(auto c : wtext) {
        auto glyph = getGlyphs(c).first;
        if(glyph)
            fwidth += from1616(glyph->advance.x);
    }

    auto width = (unsigned)(fwidth + 2.f * m_outline) + 2;
    auto height = (unsigned)(asc + desc + 2.f * m_outline) + 2;
    Log::debug<Log::graphics>("bitmap size ", width, "x", height);
    auto data = std::make_unique<std::uint8_t[]>(width * height * 4);
    auto pixels = reinterpret_cast<std::array<std::uint8_t, 4>*>(data.get());

    auto blend = [&](const FT_Bitmap& bitmap, int x0, int y0, bool white) {
        for(unsigned y = std::max(y0, 1), sy = -std::min(y0, 0); y < height - 1 && sy < bitmap.rows; y++, sy++)
            for(unsigned x = std::max(x0, 1), sx = -std::min(x0, 0); x < width - 1 && sx < bitmap.width; x++, sx++) {
                auto& out = pixels[y * width + x];
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
        for(auto c : wtext) {
            auto glyph = getGlyphs(c).second;
            if(!glyph)
                continue;
            auto bmp = reinterpret_cast<FT_BitmapGlyph>(glyph);
            Log::verbose<Log::graphics>("glyph ", converter.to_bytes(c),
                    " at ", pen, " ", bmp->left, " ", bmp->top,
                    " ", bmp->bitmap.width, "x", bmp->bitmap.rows,
                    " advance ", from1616(glyph->advance.x), ", ", from1616(glyph->advance.y));
            blend(bmp->bitmap, (int)pen.x + bmp->left, (int)pen.y - bmp->top, false);
            pen += FCoords{from1616(glyph->advance.x), from1616(glyph->advance.y)};
        }
    }

    {
        auto pen = origin;
        for(auto c : wtext) {
            auto glyph = getGlyphs(c).first;
            if(!glyph)
                continue;
            auto bmp = reinterpret_cast<FT_BitmapGlyph>(glyph);
            Log::verbose<Log::graphics>("glyph ", converter.to_bytes(c),
                    " at ", pen, " ", bmp->left, " ", bmp->top,
                    " ", bmp->bitmap.width, "x", bmp->bitmap.rows,
                    " advance ", from1616(glyph->advance.x), ", ", from1616(glyph->advance.y));
            blend(bmp->bitmap, (int)pen.x + bmp->left, (int)pen.y - bmp->top, true);
            pen += FCoords{from1616(glyph->advance.x), from1616(glyph->advance.y)};
        }
    }

    return {{width, height}, std::move(data)};
}
