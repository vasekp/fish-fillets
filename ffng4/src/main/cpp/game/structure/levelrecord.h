#ifndef FISH_FILLETS_LEVELRECORD_H
#define FISH_FILLETS_LEVELRECORD_H

enum class LevelState {
    locked,
    open,
    solved
};

struct LevelRecord {
    LevelRecord* parent;
    std::string codename;
    std::string script_filename;
    std::string script_ending;
    int depth;
    bool solved;
    FCoords coords;
    int maskColor;

    std::map<std::string, std::string> description;
    struct {
        std::string name;
        int moves;
    } best;

    LevelRecord(LevelRecord* parent_, std::string codename_, std::string filename_,
            std::string ending_, int depth_, bool solved_, FCoords coords_, int maskColor_);
    LevelState state() const;
    bool visible() const;
    std::filesystem::path saveFilename() const;
    std::filesystem::path solveFilename() const;

    constexpr static int noColor = -1;
    constexpr static int depthEnding = 16;
};

#endif //FISH_FILLETS_LEVELRECORD_H