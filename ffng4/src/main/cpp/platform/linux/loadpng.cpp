#include "subsystem/graphics.h"
#include "subsystem/files.h"
#include <png.h>
#include <cstdio>
#include <csetjmp>

ogl::Texture Graphics::loadPNG(const std::string& filename) const {
    auto file = m_instance.files().system(filename);
    std::array<png_byte, 8> header;

    std::FILE* fp = fopen(file.fullPath().c_str(), "rb");
    if(!fp)
        Log::error("Error opening file ", file.fullPath());

    std::fread(header.data(), 1, 8, fp);
    if(png_sig_cmp(header.data(), 0, 8))
        Log::error("Wrong PNG header: ", file.fullPath());

    auto png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if(!png_ptr)
        Log::error("png_create_read_struct failed");

    auto info_ptr = png_create_info_struct(png_ptr);
    if(!info_ptr)
        Log::error("png_create_info_struct failed");

    if(setjmp(png_jmpbuf(png_ptr)))
        Log::error("libpng: error during init_io");

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);
    png_set_palette_to_rgb(png_ptr);
    png_set_gray_to_rgb(png_ptr);
    png_set_add_alpha(png_ptr, 0xFF, PNG_FILLER_AFTER);
    png_read_update_info(png_ptr, info_ptr);

    auto width = png_get_image_width(png_ptr, info_ptr);
    auto height = png_get_image_height(png_ptr, info_ptr);

    if(setjmp(png_jmpbuf(png_ptr)))
        Log::error("libpng: error during png_read_image");

    auto stride = png_get_rowbytes(png_ptr, info_ptr);
    if(stride != 4 * width)
        Log::error(filename, ": width = ", width, " but stride = ", stride);
    auto data = std::make_unique<png_byte[]>(height * stride);

    auto row_pointers = std::make_unique<png_bytep[]>(height);
    for(auto y = 0u; y < height; y++)
        row_pointers[y] = data.get() + y * stride;

    png_read_image(png_ptr, row_pointers.get());
    std::fclose(fp);

    // Premultiply all pixels by alpha.
    auto data_rgba = reinterpret_cast<std::array<std::uint8_t, 4>*>(data.get());
    for(auto p = 0u; p < width * height; p++) {
        auto [r, g, b, a] = data_rgba[p];
        r = (std::uint8_t)(r * (int)a / 255);
        g = (std::uint8_t)(g * (int)a / 255);
        b = (std::uint8_t)(b * (int)a / 255);
        data_rgba[p] = std::array{r, g, b, a};
    }

    return ogl::Texture::fromImageData(m_system->ref(), width, height, stride, data.get());
}
