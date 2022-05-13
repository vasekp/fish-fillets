#ifndef FISH_FILLETS_LEVELRECORD_H
#define FISH_FILLETS_LEVELRECORD_H

enum class LevelState {
    locked,
    open,
    solved
};

struct LevelRecord {
    std::shared_ptr<LevelRecord> parent;
    std::string script_filename;
    std::string script_ending;
    int depth;
    bool solved;
    Coords coords;
    int maskColor;

    std::map<std::string, std::string> description;
    struct {
        std::string name;
        int moves;
    } best;

    LevelRecord(std::shared_ptr<LevelRecord> parent_, std::string filename_, std::string ending_,
                int depth_, bool solved_, Coords coords_, int maskColor_);
    LevelState state() const;

    constexpr static int no_color = -1;
};

#endif //FISH_FILLETS_LEVELRECORD_H