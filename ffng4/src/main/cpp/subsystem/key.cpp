#include "key.h"
#include <android/keycodes.h>

Key AndroidKeymap(unsigned int code) {
    switch(code) {
        case AKEYCODE_DPAD_UP:
            return Key::up;
        case AKEYCODE_DPAD_DOWN:
            return Key::down;
        case AKEYCODE_DPAD_LEFT:
            return Key::left;
        case AKEYCODE_DPAD_RIGHT:
            return Key::right;
        case AKEYCODE_SPACE:
            return Key::space;
        case AKEYCODE_ESCAPE:
            return Key::exit;
        default:
            return Key::none;
    }
}