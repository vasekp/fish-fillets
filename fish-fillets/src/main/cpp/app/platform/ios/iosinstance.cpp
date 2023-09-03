#include "iosinstance.h"
#include "game/screens/screenmanager.h"
#include "glue.h"

IOSInstance::IOSInstance(Glue& glue) :
    Instance(std::make_unique<IOSFiles>()),
    live(false),
    m_glue(glue),
    m_input(*this)
{
    init();
}

std::string IOSInstance::lang() {
    return "cs"; // TODO
}
