#ifndef FISH_FILLETS_AUDIO_SOURCELIST_H
#define FISH_FILLETS_AUDIO_SOURCELIST_H

class AudioSourceList {
    using Sources = std::vector<AudioSource::Ref>;
    std::unique_ptr<Sources> m_sources_thread;
    std::unique_ptr<Sources> m_sources_local;
    std::atomic<bool> m_sources_lock;

    std::atomic<bool> m_dialogsLocal;
    std::atomic<bool> m_dialogsThread;

    class SourcesGuard {
        AudioSourceList& m_parent;
    public:
        SourcesGuard(AudioSourceList& parent);
        ~SourcesGuard();

        Sources& vector();
        Sources* operator->();

        void checkDialogs();
    };

public:
    AudioSourceList();

    SourcesGuard local();
    Sources& thread();

    void setDialogsThread(bool dialogs);
    bool hasDialogs() const;
};

#endif //FISH_FILLETS_AUDIO_SOURCELIST_H