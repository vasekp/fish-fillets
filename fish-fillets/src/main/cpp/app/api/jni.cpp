#include "jni.h"

namespace jni {
    Env::Env(android_app* app) :
            m_vm(app->activity->vm),
            m_env(),
            m_obj(app->activity->clazz) // android NDK misnomer
    {
        m_vm->AttachCurrentThread(&m_env, nullptr);
        m_class = m_env->GetObjectClass(m_obj);
    }

    Env::~Env() {
        m_env->DeleteLocalRef(m_class);
        m_vm->DetachCurrentThread();
    }

    void Env::addMethod(const char* name, const char* sig) {
        auto ret = m_env->GetMethodID(m_class, name, sig);
        if(!ret)
            Log::fatal("JNI: method ", name, " not found!");
        m_methods[name] = ret;
    }

    jmethodID Env::getMethod(const std::string& name) const {
        return m_methods.at(name);
    }
}
