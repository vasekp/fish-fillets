#ifndef FISH_FILLETS_LEVELRECORD_H
#define FISH_FILLETS_LEVELRECORD_H

#include "common.h"

enum class LevelState {
    locked,
    open,
    solved
};

struct LevelRecord {
    std::string codename;
    std::map<std::string, std::string> title;
    LevelRecord* parent;
    FCoords coords;
    int depth;
    std::string script_filename;
    std::string script_ending;
    int maskColor;
    bool solved = false;

    LevelState state() const;
    bool final() const;
    bool ending() const;
    std::filesystem::path saveFilename() const;
    std::filesystem::path solveFilename() const;

    constexpr static int noColor = -1;
    constexpr static int depthEnding = 16;
};

#endif //FISH_FILLETS_LEVELRECORD_H
