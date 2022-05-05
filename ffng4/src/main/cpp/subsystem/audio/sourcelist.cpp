#include "subsystem/audio.h"

AudioSourceList::AudioSourceList() :
        m_sources_thread(std::make_unique<Sources>()),
        m_sources_local(),
        m_sources_lock(false)
{ }

AudioSourceList::SourcesGuard::SourcesGuard(AudioSourceList& parent) : m_parent(parent) {
    while(m_parent.m_sources_lock.exchange(true, std::memory_order_acquire))
        ;
    if(!m_parent.m_sources_local)
        m_parent.m_sources_local = std::make_unique<Sources>(*m_parent.m_sources_thread);
}

AudioSourceList::SourcesGuard::~SourcesGuard() {
    m_parent.m_sources_lock.store(false, std::memory_order_release);
}

AudioSourceList::SourcesGuard AudioSourceList::local() {
    return {*this};
}

AudioSourceList::Sources& AudioSourceList::thread() {
    if(!m_sources_lock.exchange(true, std::memory_order_acquire)) {
        if(m_sources_local)
            m_sources_thread = std::move(m_sources_local);
        m_sources_lock.store(false, std::memory_order_release);
    }
    return *m_sources_thread;
}