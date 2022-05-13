#ifndef FISH_FILLETS_MODELANIM_H
#define FISH_FILLETS_MODELANIM_H

#include "subsystem/graphics.h"
#include <variant>

class ModelAnim {
    class Layer {
        std::vector<Image>* m_set;
        int m_phase;
        int m_length;

    public:
        Layer() = default;

        Layer(std::vector<Image>& set, int phase) : m_set(&set), m_phase(phase), m_length(set.size()) { }
        void advance() {
            m_phase++;
            if(m_phase >= m_length)
                m_phase = 0;
        }
        const Image& get() const {
            return (*m_set)[m_phase];
        }
    };

    std::map<std::string, std::array<std::vector<Image>, 2>> m_images;
    std::string m_curName;
    bool m_running;
    Layer m_main;
    std::optional<Layer> m_extra;

public:
    void add(const std::string& name, int dir, const Image& image) {
        m_images[name][dir].push_back(image);
    }

    void set(const std::string& name, int dir, int phase, bool running) {
        if(name == m_curName && running && m_running)
            return;
        m_main = {m_images[name][dir], phase};
        m_curName = name;
        m_running = running;
    }

    void setExtra(const std::string& name, int dir, int phase) {
        if(!name.empty())
            m_extra = {m_images[name][dir], phase};
        else
            m_extra = {};
    }

    std::vector<Image>
    get() const {
        if(m_extra)
            return {m_main.get(), m_extra->get()};
        else
            return {m_main.get()};
    }

    void update() {
        if(!m_running)
            return;
        m_main.advance();
        if(m_extra)
            m_extra->advance();
    }
};

#endif //FISH_FILLETS_MODELANIM_H