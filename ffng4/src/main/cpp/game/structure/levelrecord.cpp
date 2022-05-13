#include "gametree.h"

LevelRecord::LevelRecord(std::shared_ptr<LevelRecord> parent_, std::string filename_, std::string ending_,
                         int depth_, bool solved_, Coords coords_, int maskColor_) :
        parent(std::move(parent_)),
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