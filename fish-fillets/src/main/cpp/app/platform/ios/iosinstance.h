#ifndef FF_IOS_INSTANCE_H
#define FF_IOS_INSTANCE_H

#include "common.h"
#include "instance.h"
#include "files.h"
#include "subsystem/input.h"
#include "input.h"
//#include "oboesink.h"
#include <thread>

class Glue;

class IOSInstance : public Instance {
public:
    bool live;

    IOSInstance(Glue& glue);
    virtual ~IOSInstance() = default;

    IOSInput& inputSource() override { return m_input; }
    std::string lang() override;

//    OboeSink& oboe() { return m_sink; }

private:
    Glue& m_glue;
    IOSInput m_input;
//    OboeSink m_sink;

    void own_run() override;
    void own_pause() override;
    void own_quit() override;
};

#endif //FF_IOS_INSTANCE_H
