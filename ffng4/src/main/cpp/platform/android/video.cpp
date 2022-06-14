#include "game/screens/screenmanager.h"
#include "ainstance.h"

void ScreenManager::playIntro() {
    auto& jni = m_instance.platform().jni;
    jni->CallVoidMethod(jni.object(), jni.method("playIntro"));
}