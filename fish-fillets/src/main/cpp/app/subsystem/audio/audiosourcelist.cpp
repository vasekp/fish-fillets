#include "subsystem/audio.h"

AudioSourceList::AudioSourceList() :
        m_sources_thread(std::make_unique<Sources>()),
        m_sources_local(),
        m_sources_lock(false),
        m_dialogsLocal(false),
        m_dialogsThread(false)
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

AudioSourceList::Sources& AudioSourceList::SourcesGuard::vector() {
    return *m_parent.m_sources_local;
}

AudioSourceList::Sources* AudioSourceList::SourcesGuard::operator->() {
    return m_parent.m_sources_local.get();
}

void AudioSourceList::SourcesGuard::checkDialogs() {
    bool dialogs = std::any_of(m_parent.m_sources_local->begin(), m_parent.m_sources_local->end(),
            [](const auto& source) { return source->isDialog(); });
    m_parent.m_dialogsLocal.store(dialogs, std::memory_order::release);
}

AudioSourceList::SoftGuard::SoftGuard(AudioSourceList& parent) : m_parent(parent) {
    locked = !m_parent.m_sources_lock.exchange(true, std::memory_order_acquire);
}

AudioSourceList::SoftGuard::~SoftGuard() {
    if(locked)
        m_parent.m_sources_lock.store(false, std::memory_order_release);
}

AudioSourceList::SoftGuard::operator bool() const {
    return locked;
}

AudioSourceList::SourcesGuard AudioSourceList::local() {
    return SourcesGuard{*this};
}

AudioSourceList::SoftGuard AudioSourceList::threadGuard() {
    return SoftGuard{*this};
}

AudioSourceList::Sources& AudioSourceList::thread() {
    if(auto lock = SoftGuard(*this)) {
        if(m_sources_local) {
            m_sources_thread = std::move(m_sources_local);
            m_dialogsThread.store(m_dialogsLocal.load(std::memory_order::acquire), std::memory_order::release);
            m_dialogsLocal.store(false, std::memory_order::release);
        }
    }
    return *m_sources_thread;
}

void AudioSourceList::setDialogsThread(bool dialogs) {
    m_dialogsThread.store(dialogs, std::memory_order::release);
}

bool AudioSourceList::hasDialogs() const {
    return m_dialogsLocal.load(std::memory_order::acquire) || m_dialogsThread.load(std::memory_order::acquire);
}
