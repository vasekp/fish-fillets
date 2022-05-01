#ifndef FISH_FILLETS_JNI_H
#define FISH_FILLETS_JNI_H

#include "common.h"
#include <jni.h>
#include <android_native_app_glue.h>

namespace jni {
    class Env {
        JavaVM *m_vm;
        ::JNIEnv *m_env;
        jclass m_class;
        jobject m_obj;
        std::map<std::string, jmethodID> m_methods;

    public:
        Env(android_app* app);
        Env(const Env&) = delete;
        Env& operator=(const Env&) = delete;
        ~Env();

        operator ::JNIEnv *() const { return m_env; }
        ::JNIEnv *operator->() const { return m_env; }

        jobject object() const { return m_obj; }
        jmethodID method(const std::string& name) const;

    private:
        jmethodID getMethodID(const char* name, const char* sig);
    };
}

#endif //FISH_FILLETS_JNI_H