#ifndef FISH_FILLETS_NDK_H
#define FISH_FILLETS_NDK_H

#include <android/asset_manager_jni.h>
#include <memory>

namespace ndk {
    class Asset {
        AAsset *m_asset;

    public:
        Asset() = default;

        Asset(AAssetManager *assets, const char *filename, int mode) :
                m_asset(AAssetManager_open(assets, filename, mode)) {}

        Asset(const Asset &) = delete;

        Asset &operator=(const Asset &) = delete;

        Asset(Asset &&other) : m_asset(other.m_asset) {
            other.m_asset = nullptr;
        }

        Asset &operator=(Asset &&other) {
            std::swap(m_asset, other.m_asset);
            return *this;
        }

        ~Asset() {
            if (m_asset)
                AAsset_close(m_asset);
        }

        operator AAsset *() { return m_asset; }
        operator bool() { return m_asset != nullptr; }
    };
}

#endif //FISH_FILLETS_NDK_H
