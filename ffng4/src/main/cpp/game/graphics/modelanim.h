#ifndef FISH_FILLETS_MODELANIM_H
#define FISH_FILLETS_MODELANIM_H

#include "subsystem/graphics.h"
#include <optional>

class ModelAnim {
    class Layer {
        const std::array<std::vector<const Image*>, 2>& m_set;
        int m_phase;
        int m_length;

    public:
        Layer(const std::array<std::vector<const Image*>, 2>& set, int phase) : m_set(set), m_phase(phase), m_length(set[0].size()) { }

        void advance() {
            m_phase++;
            if(m_phase >= m_length)
                m_phase = 0;
        }
        const Image* get(int dir) const {
            return m_set[dir][m_phase];
        }
    };

    std::map<std::string, std::array<std::vector<const Image*>, 2>> m_images;
    std::string m_curName;
    bool m_running;
    std::optional<Layer> m_main;
    std::optional<Layer> m_extra;

public:
    void add(const std::string& name, int dir, const Image* image) {
        m_images[name][dir].push_back(image);
    }

    void set(const std::string& name, int phase, bool running) {
        if(name == m_curName && running && m_running)
            return;
        m_main.emplace(m_images[name], phase);
        m_curName = name;
        m_running = running;
    }

    void setExtra(const std::string& name, int phase) {
        if(!name.empty())
            m_extra.emplace(m_images[name], phase);
        else
            removeExtra();
    }

    void removeExtra() {
        m_extra.reset();
    }

    std::vector<const Image*>
    get(int dir) const {
        assert(m_main);
        if(m_extra)
            return {m_main->get(dir), m_extra->get(dir)};
        else
            return {m_main->get(dir)};
    }

    void update() {
        if(!m_running)
            return;
        m_main->advance();
        if(m_extra)
            m_extra->advance();
    }
};

#endif //FISH_FILLETS_MODELANIM_H