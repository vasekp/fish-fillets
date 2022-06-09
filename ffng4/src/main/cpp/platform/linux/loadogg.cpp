#include "subsystem/audio.h"

AudioData::Ref Audio::loadOGG(const std::string& filename) const {
    return AudioData::create(filename, 1000);
}
