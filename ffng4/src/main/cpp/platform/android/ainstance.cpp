#include "ainstance.h"
#include "game/screens/screenmanager.h"

AndroidInstance::AndroidInstance(android_app* androidApp):
        m_files(androidApp),
        m_input(*this),
        m_sink(*this),
        app(androidApp),
        jni(androidApp),
        live(false)
{
    app->userData = this;
    init();
}

AndroidInstance& AndroidInstance::get(android_app* app) {
    return *static_cast<AndroidInstance*>(app->userData);
}

void AndroidInstance::quit() {
    Instance::quit();
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