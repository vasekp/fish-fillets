#ifndef FISH_FILLETS_PLATFORM_FILES_H
#define FISH_FILLETS_PLATFORM_FILES_H

#ifdef __ANDROID__
#include "platform/android/files.h"
#elifdef __linux__
#include "platform/linux/files.h"
#endif

#endif //FISH_FILLETS_PLATFORM_FILES_H
