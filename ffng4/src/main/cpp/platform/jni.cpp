#include "platform/jni.h"

namespace jni {
    Env::Env(android_app* app) :
            m_vm(app->activity->vm),
            m_env(),
            m_obj(app->activity->clazz) // android NDK misnomer
    {
        m_vm->AttachCurrentThread(&m_env, nullptr);
        m_class = m_env->GetObjectClass(m_obj);
        m_methods["loadBitmap"] = getMethodID("loadBitmap", "(Ljava/lang/String;)Landroid/graphics/Bitmap;");
        m_methods["playIntro"] = getMethodID("playIntro", "()V");
        m_methods["text"] = getMethodID("text", "(Ljava/lang/String;Ljava/lang/String;I)Landroid/graphics/Bitmap;");
    }

    Env::~Env() {
        m_env->DeleteLocalRef(m_class);
        m_vm->DetachCurrentThread();
    }

    jmethodID Env::getMethodID(const char *name, const char *sig) {
        auto ret = m_env->GetMethodID(m_class, name, sig);
        assert(ret);
        return ret;
    }

    jmethodID Env::method(const std::string &name) const {
        return m_methods.at(name);
    }
}