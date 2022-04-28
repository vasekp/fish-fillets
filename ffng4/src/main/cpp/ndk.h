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
    class Asset {
        AAsset *asset;

    public:
        Asset() : asset(nullptr) {}

        Asset(AAssetManager *assets, const char *filename, int mode) :
                asset(AAssetManager_open(assets, filename, mode)) {}

        Asset(const Asset &) = delete;

        Asset &operator=(const Asset &) = delete;

        Asset(Asset &&other) : asset(other.asset) {
            other.asset = nullptr;
        }

        Asset &operator=(Asset &&other) {
            std::swap(asset, other.asset);
            return *this;
        }

        ~Asset() {
            if (asset != nullptr)
                AAsset_close(asset);
        }

        bool valid() const {
            return asset != nullptr;
        }

        AAsset *operator*() {
            return asset;
        }
    };
}

#endif //FISH_FILLETS_NDK_H