#ifndef FISH_FILLETS_IFONT_H
#define FISH_FILLETS_IFONT_H

class IFont {
protected:
    float m_fontSize;
    float m_outline;
    float m_scale;

public:
    virtual ~IFont() = 0;

    void setSizes(float fontSize, float outline, float scale);
    std::vector<std::string> breakLines(const std::string& text, float width);
    Texture renderText(Instance& instance, const std::string& text) const;

private:
    virtual void own_setSizes() = 0;
    virtual std::vector<std::string> own_breakLines(const std::string& text, float width) = 0;
    virtual ImageData own_renderText(const std::string& text) const = 0;
};

inline IFont::~IFont() { }

#endif //FISH_FILLETS_IFONT_H
