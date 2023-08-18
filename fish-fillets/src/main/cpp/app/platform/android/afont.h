#ifndef FISH_FILLETS_ANDROID_FONT_H
#define FISH_FILLETS_ANDROID_FONT_H

#include "common.h"
#include "subsystem/graphics.h"

class Instance;

class AndroidFont : public IFont {
public:
    AndroidFont(Instance& instance, const std::string& filename);

private:
    std::string m_filename;

    void own_setSizes() override;
    std::vector<std::string> own_breakLines(const std::string& text, float width) override;
    ImageData own_renderText(const std::string& text) const override;
};

namespace decoders {
    std::unique_ptr<IFont> ttf(Instance& instance, const std::string& filename) {
        return std::make_unique<AndroidFont>(instance, filename);
    }
}

#endif //FISH_FILLETS_ANDROID_FONT_H
