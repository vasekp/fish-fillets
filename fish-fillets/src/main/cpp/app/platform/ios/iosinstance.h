#ifndef FF_IOS_INSTANCE_H
#define FF_IOS_INSTANCE_H

#include "common.h"
#include "instance.h"
#include "files.h"
#include "subsystem/input.h"
#include "input.h"
#include <thread>

class Glue;

class IOSInstance : public Instance {
public:
    bool live;

    IOSInstance(Glue& glue);
    virtual ~IOSInstance() = default;

    IOSInput& inputSource() override { return m_input; }
    std::string lang() override;

private:
    Glue& m_glue;
    IOSInput m_input;
};

#endif //FF_IOS_INSTANCE_H
