#ifndef FISH_FILLETS_JNI_H
#define FISH_FILLETS_JNI_H

#include "common.h"
#include <jni.h>

namespace jni {
    class Env {
    public:
        Env(JavaVM* vm, jobject obj);
        Env(const Env&) = delete;
        Env& operator=(const Env&) = delete;
        ~Env();

        operator JNIEnv*() const { return m_env; }
        JNIEnv* operator->() const { return m_env; }

        jobject object() const { return m_obj; }
        void addMethod(const char* name, const char* sig);
        jmethodID getMethod(const std::string& name) const;

    private:
        JavaVM* m_vm;
        JNIEnv* m_env;
        jclass m_class;
        jobject m_obj;
        std::map<std::string, jmethodID> m_methods;
    };
}

#endif //FISH_FILLETS_JNI_H
