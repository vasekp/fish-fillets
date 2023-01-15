#include "subsystem/graphics.h"
#include "ainstance.h"

float Graphics::dpi() const {
    //return (float) AConfiguration_getDensity(m_instance.platform().app->config);
    return 72;
}