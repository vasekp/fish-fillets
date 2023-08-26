#ifndef FF_ANDROID_INSTANCE_H
#define FF_ANDROID_INSTANCE_H

#include "common.h"
#include "instance.h"
#include "api/jni.h"
#include "files.h"
#include "input.h"
#include "oboesink.h"
#include <thread>
#include <android/asset_manager_jni.h>

struct android_app;
class Glue;

class AndroidInstance : public Instance {
public:
    jni::Env& jni;
    bool live;

    AndroidInstance(Glue& glue, jni::Env& env, jobject jAMgr, std::string&& userPath);
    virtual ~AndroidInstance() = default;

    static AndroidInstance& get(android_app* app);

    AndroidInput& inputSource() override { return m_input; }
    std::string lang() override;

    OboeSink& oboe() { return m_sink; }

private:
    Glue& m_glue;
    AndroidInput m_input;
    OboeSink m_sink;

    void own_run() override;
    void own_pause() override;
    void own_quit() override;
};

#endif //FF_ANDROID_INSTANCE_H
