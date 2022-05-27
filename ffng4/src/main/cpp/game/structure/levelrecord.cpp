#include "gametree.h"
#include "levelrecord.h"

LevelRecord::LevelRecord(LevelRecord* parent_, std::string codename_, std::string filename_,
        std::string ending_, int depth_, bool solved_, FCoords coords_, int maskColor_) :
        parent(parent_),
        codename(std::move(codename_)),
        script_filename(std::move(filename_)),
        script_ending(std::move(ending_)),
        depth(depth_),
        solved(solved_),
        coords(coords_),
        maskColor(maskColor_)
{ }

LevelState LevelRecord::state() const {
    return solved
           ? LevelState::solved
           : !parent
             ? LevelState::open
             : parent->solved
               ? LevelState::open
               : LevelState::locked;
}

bool LevelRecord::visible() const {
    return depth < depthEnding;
}

std::filesystem::path LevelRecord::saveFilename() const {
    return {"saves/"s + codename + ".lua"};
}

std::filesystem::path LevelRecord::solveFilename() const {
    return {"solved/"s + codename + ".lua"};
}
