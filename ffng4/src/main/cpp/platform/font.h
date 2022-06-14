#ifndef FISH_FILLETS_PLATFORM_FONT_H
#define FISH_FILLETS_PLATFORM_FONT_H

#ifdef __ANDROID__
#include "platform/android/font.h"
#elifdef __linux__
#include "platform/linux/font.h"
#endif

#endif //FISH_FILLETS_PLATFORM_FONT_H
