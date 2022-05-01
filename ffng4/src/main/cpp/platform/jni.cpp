#include "platform/jni.h"

namespace jni {
    Env::Env(android_app* app) :
        vm(app->activity->vm),
        env(),
        obj(app->activity->clazz) // android NDK misnomer
    {
        vm->AttachCurrentThread(&env, nullptr);
        clazz = env->GetObjectClass(obj);
        methods["loadBitmap"] = getMethodID("loadBitmap", "(Ljava/lang/String;)Landroid/graphics/Bitmap;");
    }

    Env::~Env() {
        env->DeleteLocalRef(clazz);
        vm->DetachCurrentThread();
    }

    jmethodID Env::getMethodID(const char *name, const char *sig) {
        auto ret = env->GetMethodID(clazz, name, sig);
        assert(ret);
        return ret;
    }

    jmethodID Env::method(const std::string &name) const {
        return methods.at(name);
    }
}