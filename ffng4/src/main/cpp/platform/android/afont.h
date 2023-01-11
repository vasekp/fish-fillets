#ifndef FISH_FILLETS_ANDROID_FONT_H
#define FISH_FILLETS_ANDROID_FONT_H

#include "subsystem/graphics.h"
#include "api/ogl.h"

class AndroidFont : public IFont {
    Instance& m_instance;
    std::string m_filename;
    float m_fontSize;
    float m_outline;

public:
    AndroidFont(Instance& instance, const std::string& filename);

    void setSizes(float fontSize, float outline) override;
    std::vector<std::string> breakLines(const std::string& text, float width) override;
    ogl::Texture renderText(const std::string& text) const override;
};

namespace decoders {
    std::unique_ptr<IFont> ttf(Instance& instance, const std::string& filename) {
        return std::make_unique<AndroidFont>(instance, filename);
    }
}

#endif //FISH_FILLETS_ANDROID_FONT_H