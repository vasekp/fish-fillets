#ifndef FISH_FILLETS_NDK_H
#define FISH_FILLETS_NDK_H

#include <jni.h>
#include <android/asset_manager_jni.h>

namespace ndk {
    class JNIEnv {
        JavaVM* vm;
        ::JNIEnv* env;

    public:
        JNIEnv(JavaVM* _vm) : vm(_vm) { vm->AttachCurrentThread(&env, nullptr); }
        ~JNIEnv() { vm->DetachCurrentThread(); }

        operator ::JNIEnv*() const { return env; }
        ::JNIEnv* operator->() const { return env; };
    };

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