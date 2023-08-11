#ifndef FISH_FILLETS_GRAPHICS_IMAGEDATA_H
#define FISH_FILLETS_GRAPHICS_IMAGEDATA_H

struct ImageData {
    USize size;
    std::unique_ptr<std::uint8_t[]> data;
};

#endif //FISH_FILLETS_GRAPHICS_IMAGEDATA_H
