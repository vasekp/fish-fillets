#include "subsystem/files.h"
#include "./files.h"

SystemFile Files::system(const std::string& path) const {
    return {path, "/home/vasek/prog/android/fish-fillets/ffng4/src/main/assets/"};
}

UserFile Files::user(const std::string& path) const {
    return {path, ""};
}
