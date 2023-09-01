#include "iosinstance.h"
#include "game/screens/screenmanager.h"
#include "glue.h"

IOSInstance::IOSInstance(Glue& glue) :
    Instance(std::make_unique<IOSFiles>()),
    live(false),
    m_glue(glue)
//    m_input(*this),
//    m_sink(*this)
{
    init();
}

//IOSInstance& IOSInstance::get(android_app* app) {
//    return *static_cast<IOSInstance*>(app->userData);
//}

void IOSInstance::own_run() {
//    oboe().start();
}

void IOSInstance::own_pause() {
//    oboe().stop();
}

void IOSInstance::own_quit() {
//    m_glue.closeActivity();
}

std::string IOSInstance::lang() {
    return "cs"; // TODO
}
