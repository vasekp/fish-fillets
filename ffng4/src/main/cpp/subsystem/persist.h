#ifndef FISH_FILLETS_PERSIST_H
#define FISH_FILLETS_PERSIST_H

#include "instance.h"
#include "subsystem/script.h"
#include "subsystem/files.h"

class Persist : public ScriptReferrer {
    Instance& m_instance;
    std::map<std::string, std::variant<std::string, int>> m_records;

public:
    Persist(Instance& instance);

    template<typename T>
    void set(const std::string& name, T value);
    template<typename T>
    T get(const std::string& name, T defValue);

private:
    void save();

    constexpr static const char* filename = "./persist.lua";
};

#endif //FISH_FILLETS_PERSIST_H