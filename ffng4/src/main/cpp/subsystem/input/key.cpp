#include "key.h"
#include <android/keycodes.h>
#include <stdexcept>

namespace Input {

    Key AndroidKeymap(unsigned int code) {
        switch (code) {
            case AKEYCODE_DPAD_UP:
                return Key::up;
            case AKEYCODE_DPAD_DOWN:
                return Key::down;
            case AKEYCODE_DPAD_LEFT:
                return Key::left;
            case AKEYCODE_DPAD_RIGHT:
                return Key::right;
            case AKEYCODE_SPACE:
            case AKEYCODE_DPAD_CENTER:
                return Key::space;
            case AKEYCODE_ESCAPE:
            case AKEYCODE_BACK:
                return Key::exit;
            case AKEYCODE_F2:
            case AKEYCODE_S:
                return Key::save;
            case AKEYCODE_F3:
            case AKEYCODE_L:
                return Key::load;
            case AKEYCODE_DEL:
            case AKEYCODE_R:
                return Key::restart;
            default:
                return Key::none;
        }
    }

    Key CharKeymap(char code) {
        switch (code) {
            case 'u':
                return Key::smallUp;
            case 'd':
                return Key::smallDown;
            case 'l':
                return Key::smallLeft;
            case 'r':
                return Key::smallRight;
            case 'U':
                return Key::bigUp;
            case 'D':
                return Key::bigDown;
            case 'L':
                return Key::bigLeft;
            case 'R':
                return Key::bigRight;
            case ' ':
                return Key::space;
            default:
                return Key::none;
        }
    }

    Key toKey(ICoords dir) {
        if(dir == Direction::up)
            return Key::up;
        else if(dir == Direction::down)
            return Key::down;
        else if(dir == Direction::left)
            return Key::left;
        else if(dir == Direction::right)
            return Key::right;
        else
            return Key::none;
    }

}