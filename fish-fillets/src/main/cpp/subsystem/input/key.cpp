#include "key.h"

namespace Input {

    Key toKey(Direction dir) {
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
