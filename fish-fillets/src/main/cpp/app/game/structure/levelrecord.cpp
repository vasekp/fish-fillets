#include "levelrecord.h"

LevelState LevelRecord::state() const {
    return solved
           ? LevelState::solved
           : !parent
             ? LevelState::open
             : parent->solved
               ? LevelState::open
               : LevelState::locked;
}

bool LevelRecord::final() const {
    return !script_ending.empty() && depth < depthEnding;
}

bool LevelRecord::ending() const {
    return depth == depthEnding;
}

std::filesystem::path LevelRecord::saveFilename() const {
    return {"saves/"s + codename + ".lua"};
}

std::filesystem::path LevelRecord::solveFilename() const {
    return {"solved/"s + codename + ".lua"};
}
