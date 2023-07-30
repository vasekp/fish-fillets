#ifndef FISH_FILLETS_TEXTURE_TYPE_H
#define FISH_FILLETS_TEXTURE_TYPE_H

struct TextureType {
    enum class Type {
        image,
        mask,
        channelY,
        channelCb,
        channelCr
    } type;

    using enum Type;

    TextureType(Type t) : type(t) { }
    operator Type() const { return type; }

    int channels() const {
        switch(type) {
            case image:
            case mask:
                return 4;
            case channelY:
            case channelCb:
            case channelCr:
                return 1;
            default:
                std::unreachable();
        }
    }
};

#endif //FISH_FILLETS_TEXTURE_TYPE_H
