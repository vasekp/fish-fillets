#ifndef FISH_FILLETS_MODELANIM_H
#define FISH_FILLETS_MODELANIM_H

#include "common.h"

class Image;

class ModelAnim {
public:
    void add(const std::string& name, int dir, const Image* image);
    void set(const std::string& name, std::size_t phase, bool running);
    void setExtra(const std::string& name, std::size_t phase);
    void removeExtra();

    void update();
    std::pair<const Image*, std::optional<const Image*>> get(int dir) const;

    static constexpr int framesTurn = 3; // bound to the animation
    static constexpr int framesActivate = 4;
    static constexpr int framesDeath = 15; // 1.5 seconds; bound to disintegrate.frag
    static constexpr int framesRestart = 30;

private:
    class Layer {
    public:
        Layer(const std::array<std::vector<const Image*>, 2>& set, std::size_t phase);

        void advance();
        const Image* get(int dir) const;

    private:
        const std::array<std::vector<const Image*>, 2>& m_set;
        std::size_t m_length;
        std::size_t m_phase;
    };

    std::map<std::string, std::array<std::vector<const Image*>, 2>> m_images;
    std::string m_curName;
    bool m_running;
    std::optional<Layer> m_main;
    std::optional<Layer> m_extra;
};

#endif //FISH_FILLETS_MODELANIM_H
