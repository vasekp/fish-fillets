#include "key.h"

namespace Input {

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
