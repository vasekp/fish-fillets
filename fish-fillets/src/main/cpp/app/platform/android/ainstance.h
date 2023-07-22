#ifndef FF_ANDROID_INSTANCE_H
#define FF_ANDROID_INSTANCE_H

#include "instance.h"
#include "api/jni.h"
#include "files.h"
#include "input.h"
#include "oboesink.h"

struct android_app;

class AndroidInstance : public Instance {
    AndroidInput m_input;
    OboeSink m_sink;

public:
    android_app* app;
    jni::Env jni;
    bool live;

    AndroidInstance(android_app* androidApp);

    static AndroidInstance& get(android_app* app);

    AndroidInput& inputSource() override { return m_input; }
    std::string lang() override;
    void own_quit() override;

    void startstop();
    OboeSink& oboe() { return m_sink; }
};

#endif //FF_ANDROID_INSTANCE_H
