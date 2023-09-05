#ifndef FISH_FILLETS_AUDIO_SOURCELIST_H
#define FISH_FILLETS_AUDIO_SOURCELIST_H

// always included from audio.h

class AudioSourceList {
    using Sources = std::vector<AudioSource::Ref>;

    class SourcesGuard {
    public:
        SourcesGuard(AudioSourceList& parent);
        ~SourcesGuard();

        Sources& vector();
        Sources* operator->();
    private:
        AudioSourceList& m_parent;
        std::size_t m_startSize;

        void checkDialogs();
    };

public:
    AudioSourceList();

    SourcesGuard local();
    Sources& thread();

    void setDialogsThread(bool dialogs);
    bool hasDialogs() const;

private:
    std::unique_ptr<Sources> m_sources_thread;
    std::unique_ptr<Sources> m_sources_local;
    std::atomic<bool> m_sources_lock;

    bool m_dirty;
    std::atomic<bool> m_dialogsLocal;
    std::atomic<bool> m_dialogsThread;
};

#endif //FISH_FILLETS_AUDIO_SOURCELIST_H
