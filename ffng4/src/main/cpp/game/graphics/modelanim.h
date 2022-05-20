#ifndef FISH_FILLETS_MODELANIM_H
#define FISH_FILLETS_MODELANIM_H

#include "subsystem/graphics.h"
#include <optional>

class ModelAnim {
    class Layer {
        const std::array<std::vector<const Image*>, 2>& m_set;
        std::size_t m_length;
        int m_phase;

    public:
        Layer(const std::array<std::vector<const Image*>, 2>& set, int phase);

        void advance();
        const Image* get(int dir) const;
    };

    std::map<std::string, std::array<std::vector<const Image*>, 2>> m_images;
    std::string m_curName;
    bool m_running;
    std::optional<Layer> m_main;
    std::optional<Layer> m_extra;

public:
    void add(const std::string& name, int dir, const Image* image);
    void set(const std::string& name, int phase, bool running);
    void setExtra(const std::string& name, int phase);
    void removeExtra();

    void update();
    std::vector<const Image*> get(int dir) const;
};

#endif //FISH_FILLETS_MODELANIM_H