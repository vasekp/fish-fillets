#ifndef FISH_FILLETS_AUDIO_SOURCELIST_H
#define FISH_FILLETS_AUDIO_SOURCELIST_H

class AudioSourceList {
    using Sources = std::vector<std::shared_ptr<AudioSource>>;
    std::unique_ptr<Sources> m_sources_thread;
    std::unique_ptr<Sources> m_sources_local;
    std::atomic<bool> m_sources_lock;

    class SourcesGuard {
        AudioSourceList& m_parent;
    public:
        SourcesGuard(AudioSourceList& parent);
        ~SourcesGuard();

        Sources* operator->() { return m_parent.m_sources_local.get(); }
    };

public:
    AudioSourceList();

    SourcesGuard local();
    Sources& thread();
};

#endif //FISH_FILLETS_AUDIO_SOURCELIST_H
