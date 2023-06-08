#ifndef FISH_FILLETS_AUDIO_SOURCELIST_H
#define FISH_FILLETS_AUDIO_SOURCELIST_H

#include <atomic>

class AudioSourceList {
    using Sources = std::vector<AudioSourceBase::Ref>;
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

    class SoftGuard {
        AudioSourceList& m_parent;
        bool locked;

    public:
        SoftGuard(AudioSourceList& parent);
        ~SoftGuard();

        operator bool() const;
    };

public:
    AudioSourceList();

    SourcesGuard local();
    Sources& thread();
    SoftGuard threadGuard();

    void setDialogsThread(bool dialogs);
    bool hasDialogs() const;
};

#endif //FISH_FILLETS_AUDIO_SOURCELIST_H
