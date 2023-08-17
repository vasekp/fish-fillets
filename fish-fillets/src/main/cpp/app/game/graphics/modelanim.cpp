#include "modelanim.h"

ModelAnim::Layer::Layer(const std::array<std::vector<const Image*>, 2>& set, std::size_t phase) : m_set(set), m_length(set[0].size()), m_phase(phase) { }

void ModelAnim::Layer::advance() {
    m_phase++;
    if(m_phase >= m_length)
        m_phase = 0;
}

const Image* ModelAnim::Layer::get(int dir) const {
    return m_set[dir][m_phase];
}

void ModelAnim::add(const std::string& name, int dir, const Image* image) {
    m_images[name][dir].push_back(image);
}

void ModelAnim::set(const std::string& name, std::size_t phase, bool running) {
    if(name == m_curName && running && m_running)
        return;
    m_main.emplace(m_images[name], phase);
    m_curName = name;
    m_running = running;
}

void ModelAnim::setExtra(const std::string& name, std::size_t phase) {
    if(!name.empty())
        m_extra.emplace(m_images[name], phase);
    else
        removeExtra();
}

void ModelAnim::removeExtra() {
    m_extra.reset();
}

std::pair<const Image*, std::optional<const Image*>> ModelAnim::get(int dir) const {
    assert(m_main);
    if(m_extra)
        return {m_main->get(dir), {m_extra->get(dir)}};
    else
        return {m_main->get(dir), {}};
}

void ModelAnim::update() {
    if(!m_running)
        return;
    m_main->advance();
    if(m_extra)
        m_extra->advance();
}
