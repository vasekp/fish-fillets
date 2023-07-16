#include "ainstance.h"
#include "game/screens/screenmanager.h"

AndroidInstance::AndroidInstance(android_app* androidApp):
        Instance(std::make_unique<AndroidFiles>(androidApp)),
        m_input(*this),
        m_sink(*this),
        app(androidApp),
        jni(androidApp),
        live(false)
{
    app->userData = this;

    jni.addMethod("loadBitmap", "(Ljava/lang/String;)Landroid/graphics/Bitmap;");
    jni.addMethod("breakLines", "(Ljava/lang/String;Ljava/lang/String;FI)[Ljava/lang/String;");
    jni.addMethod("renderText", "(Ljava/lang/String;Ljava/lang/String;FF)Landroid/graphics/Bitmap;");
    jni.addMethod("showUI", "()V");
    jni.addMethod("hideUI", "()V");
    jni.addMethod("getLang", "()Ljava/lang/String;");

    init();
}

AndroidInstance& AndroidInstance::get(android_app* app) {
    return *static_cast<AndroidInstance*>(app->userData);
}

void AndroidInstance::own_quit() {
    ANativeActivity_finish(app->activity);
}

void AndroidInstance::startstop() {
    if(!live)
        return;
    if(!running) {
        screens().pause();
        oboe().stop();
    } else {
        oboe().start();
        screens().resume();
    }
}

std::string AndroidInstance::lang() {
    auto jString = (jstring)jni->CallObjectMethod(jni.object(), jni.getMethod("getLang"));
    auto chars = jni->GetStringUTFChars(jString, nullptr);
    std::string ret{chars};
    jni->ReleaseStringUTFChars(jString, chars);
    jni->DeleteLocalRef(jString);
    return ret;
}
