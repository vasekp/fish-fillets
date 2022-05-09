#ifndef FISH_FILLETS_MODELANIM_H
#define FISH_FILLETS_MODELANIM_H

#include "subsystem/graphics.h"

class ModelAnim {
    std::map<std::string, std::array<std::vector<Image>, 2>> m_images;
    std::vector<Image> m_current;
    std::string m_name;
    int m_phase;
    int m_length;
    bool m_running;

public:
    void add(const std::string& name, int dir, const Image& image) {
        m_images[name][dir].push_back(image);
    }

    void set(const std::string& name, int phase, bool running) {
        if(m_name == name && running && m_running)
            return;
        m_name = name;
        m_phase = phase;
        m_current = m_images[name][0];
        m_length = m_current.size();
        m_running = running;
    }

    const Image& get() const {
        return m_current[m_phase];
    }

    void update() {
        if(m_running) {
            m_phase++;
            if(m_phase >= m_length)
                m_phase = 0;
        }
    }
};

#endif //FISH_FILLETS_MODELANIM_H