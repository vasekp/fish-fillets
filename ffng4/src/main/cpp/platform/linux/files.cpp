#include "subsystem/files.h"
#include "./files.h"

SystemFile Files::system(const std::string& path) const {
    return {path, ""};
}

UserFile Files::user(const std::string& path) const {
    return {path, ""};
}
