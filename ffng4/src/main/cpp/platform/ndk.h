#ifndef FISH_FILLETS_NDK_H
#define FISH_FILLETS_NDK_H

#include <android/asset_manager_jni.h>

namespace ndk {
    class Asset {
        AAsset *asset;

    public:
        Asset() = default;

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
            if (asset)
                AAsset_close(asset);
        }

        operator AAsset *() { return asset; }
        operator bool() { return asset != nullptr; }
    };
}

#endif //FISH_FILLETS_NDK_H