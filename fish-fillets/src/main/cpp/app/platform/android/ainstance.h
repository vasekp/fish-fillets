#ifndef FF_ANDROID_INSTANCE_H
#define FF_ANDROID_INSTANCE_H

#include "instance.h"
#include "api/jni.h"
#include "files.h"
#include "input.h"
#include "oboesink.h"

struct android_app;

class AndroidInstance : public Instance {
public:
    android_app* app;
    jni::Env jni;
    bool live;

    AndroidInstance(android_app* androidApp);

    static AndroidInstance& get(android_app* app);

    AndroidInput& inputSource() override { return m_input; }
    std::string lang() override;

    OboeSink& oboe() { return m_sink; }

private:
    AndroidInput m_input;
    OboeSink m_sink;

    void own_run() override;
    void own_pause() override;
    void own_quit() override;
};

#endif //FF_ANDROID_INSTANCE_H
