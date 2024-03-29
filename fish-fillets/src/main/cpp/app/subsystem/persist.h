#ifndef FISH_FILLETS_PERSIST_H
#define FISH_FILLETS_PERSIST_H

#include "common.h"
#include "instance.h"
#include "subsystem/script.h"
#include <thread>
#include <mutex>
#include <condition_variable>

class Persist : public ScriptReferrer {
public:
    Persist(Instance& instance);
    ~Persist();

    template<typename T>
    void set(const std::string& name, T value);
    template<typename T>
    T get(const std::string& name, T defValue);

private:
    Instance& m_instance;
    std::map<std::string, std::variant<std::string, int>> m_records;

    std::thread m_thread;
    std::mutex m_mutex;
    std::condition_variable m_cond;
    bool m_changed;
    bool m_startstop;

    void worker();
    void save();
};

#endif //FISH_FILLETS_PERSIST_H
