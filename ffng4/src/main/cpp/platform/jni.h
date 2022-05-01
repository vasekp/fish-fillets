#ifndef FISH_FILLETS_JNI_H
#define FISH_FILLETS_JNI_H

#include "common.h"
#include <jni.h>
#include <android_native_app_glue.h>

namespace jni {
    class Env {
        JavaVM *vm;
        ::JNIEnv *env;
        jclass clazz;
        jobject obj;
        std::map<std::string, jmethodID> methods;

    public:
        Env(android_app* app);
        Env(const Env&) = delete;
        Env& operator=(const Env&) = delete;
        ~Env();

        operator ::JNIEnv *() const { return env; }
        ::JNIEnv *operator->() const { return env; }

        jobject object() const { return obj; }
        jmethodID method(const std::string& name) const;

    private:
        jmethodID getMethodID(const char* name, const char* sig);
    };
}

#endif //FISH_FILLETS_JNI_H