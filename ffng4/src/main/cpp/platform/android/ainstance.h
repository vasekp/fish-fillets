#ifndef FF_ANDROID_INSTANCE_H
#define FF_ANDROID_INSTANCE_H

#include "instance.h"
#include "api/jni.h"
#include "files.h"
#include "input.h"
#include "oboesink.h"

struct android_app;

class AndroidInstance : public Instance {
    AndroidFiles m_files;
    AndroidInput m_input;
    OboeSink m_sink;

public:
    android_app* app;
    jni::Env jni;
    bool live;

    AndroidInstance(android_app* androidApp);

    static AndroidInstance& get(android_app* app);

    void quit() override;
    void* window() override { return app->window; }
    OboeSink& oboe() { return m_sink; }
    AndroidFiles& files() override { return m_files; }
    AndroidInput& inputSource() override { return m_input; }

    void startstop();
};

#endif //FF_ANDROID_INSTANCE_H
