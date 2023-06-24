#ifndef FISH_FILLETS_PERSIST_H
#define FISH_FILLETS_PERSIST_H

#include "instance.h"
#include "subsystem/script.h"
#include "subsystem/files.h"
#include <thread>
#include <mutex>
#include <condition_variable>

class Persist : public ScriptReferrer {
    Instance& m_instance;
    std::map<std::string, std::variant<std::string, int>> m_records;

    std::thread m_thread;
    std::mutex m_mutex;
    std::condition_variable m_cond;
    bool m_changed;
    bool m_startstop;

public:
    Persist(Instance& instance);
    ~Persist();

    void quit();

    template<typename T>
    void set(const std::string& name, T value);
    template<typename T>
    T get(const std::string& name, T defValue);

private:
    void worker();
    void save();

    constexpr static const char* filename = "./persist.lua";
    constexpr static std::chrono::duration interval = 3000ms;
};

#endif //FISH_FILLETS_PERSIST_H
