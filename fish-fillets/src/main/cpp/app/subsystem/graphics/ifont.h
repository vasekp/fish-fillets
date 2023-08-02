#ifndef FISH_FILLETS_IFONT_H
#define FISH_FILLETS_IFONT_H

class IFont {
public:
    virtual ~IFont() = 0;

    virtual void setSizes(float fontSize, float outline) = 0;
    virtual std::vector<std::string> breakLines(const std::string& text, float width) = 0;
    virtual ImageData renderText(const std::string& text) const = 0;
};

inline IFont::~IFont() { }

#endif //FISH_FILLETS_IFONT_H
