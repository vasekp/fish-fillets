#include "game/screens/screenmanager.h"
#include "ainstance.h"

void ScreenManager::playIntro() {
    auto& jni = dynamic_cast<AndroidInstance&>(m_instance).jni;
    jni->CallVoidMethod(jni.object(), jni.method("playIntro"));
}