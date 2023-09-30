#include "subsystem/audio.h"

AudioSourceList::AudioSourceList() :
        m_sources_thread(std::make_unique<Sources>()),
        m_sources_local(),
        m_sources_lock(false),
        m_dirty(false),
        m_dialogs(false)
{ }

AudioSourceList::SourcesGuard::SourcesGuard(AudioSourceList& parent) : m_parent(parent) {
    while(m_parent.m_sources_lock.exchange(true, std::memory_order_acquire))
        ;
    if(!m_parent.m_sources_local)
        m_parent.m_sources_local = std::make_unique<Sources>(*m_parent.m_sources_thread);
    m_startSize = m_parent.m_sources_local->size();
}

AudioSourceList::SourcesGuard::~SourcesGuard() {
    checkDialogs();
    m_parent.m_dirty |= (m_parent.m_sources_local->size() != m_startSize);
    m_parent.m_sources_lock.store(false, std::memory_order_release);
}

AudioSourceList::Sources& AudioSourceList::SourcesGuard::vector() {
    return *m_parent.m_sources_local;
}

AudioSourceList::Sources* AudioSourceList::SourcesGuard::operator->() {
    return m_parent.m_sources_local.get();
}

void AudioSourceList::SourcesGuard::checkDialogs() {
    m_parent.m_dialogs = std::any_of(m_parent.m_sources_local->begin(), m_parent.m_sources_local->end(),
            [](const auto& source) { return source->isDialog(); });
}

AudioSourceList::SourcesGuard AudioSourceList::local() {
    return SourcesGuard{*this};
}

AudioSourceList::Sources& AudioSourceList::thread() {
    if(auto lock = !m_sources_lock.exchange(true, std::memory_order_acquire)) {
        if(m_dirty) {
            m_sources_thread = std::move(m_sources_local);
            m_dirty = false;
        }
        m_sources_lock.store(false, std::memory_order_release);
    }
    return *m_sources_thread;
}

bool AudioSourceList::hasDialogs() const {
    return m_dialogs;
}
