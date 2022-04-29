#ifndef FISH_FILLETS_NDK_H
#define FISH_FILLETS_NDK_H

#include <android/asset_manager_jni.h>

struct Image {
    std::uint32_t width;
    std::uint32_t height;
    std::size_t stride;
    std::unique_ptr<std::byte[]> data;
};

namespace ndk {
    template<typename T>
    struct Deleter;

    template<>
    struct Deleter<AAsset> {
        void operator() (AAsset* asset) {
            AAsset_close(asset);
        }
    };

    struct Asset : public std::unique_ptr<AAsset, Deleter<AAsset>> {
        Asset(AAssetManager *assets, const char *filename, int mode) :
            std::unique_ptr<AAsset, Deleter<AAsset>>{AAssetManager_open(assets, filename, mode)}
        { }

        operator AAsset*() const {
            return get();
        }
    };
}

#endif //FISH_FILLETS_NDK_H