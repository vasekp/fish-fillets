#include "ainstance.h"
#include "game/screens/screenmanager.h"
#include "glue.h"

AndroidInstance::AndroidInstance(Glue& glue, jni::Env& env, jobject jAMgr, std::string&& userPath) :
    Instance(std::make_unique<AndroidFiles>(env, jAMgr, std::move(userPath))),
    jni(env),
    live(false),
    m_glue(glue),
    m_input(*this),
    m_sink(*this)
{
    jni.addMethod("loadBitmap", "(Ljava/lang/String;)Landroid/graphics/Bitmap;");
    jni.addMethod("breakLines", "(Ljava/lang/String;Ljava/lang/String;FI)[Ljava/lang/String;");
    jni.addMethod("renderText", "(Ljava/lang/String;Ljava/lang/String;FF)Landroid/graphics/Bitmap;");
    jni.addMethod("getLang", "()Ljava/lang/String;");

    init();
}

AndroidInstance& AndroidInstance::get(android_app* app) {
    return *static_cast<AndroidInstance*>(app->userData);
}

void AndroidInstance::own_run() {
    oboe().start();
}

void AndroidInstance::own_pause() {
    oboe().stop();
}

void AndroidInstance::own_quit() {
    m_glue.closeActivity();
}

std::string AndroidInstance::lang() {
    auto jString = (jstring)jni->CallObjectMethod(jni.object(), jni.getMethod("getLang"));
    auto chars = jni->GetStringUTFChars(jString, nullptr);
    std::string ret{chars};
    jni->ReleaseStringUTFChars(jString, chars);
    jni->DeleteLocalRef(jString);
    return ret;
}
