#include "persist.h"
#include <sstream>

Persist::Persist(Instance& instance) :
    m_instance(instance),
    m_quit(false)
{
    auto file = instance.files().user(filename);
    if(file->exists()) {
        Script script(instance, *this);
        script.registerFn("options_setInt", lua::wrap<&Persist::set<int>>);
        script.registerFn("options_setString", lua::wrap<&Persist::set<const std::string&>>);
        script.loadFile(file.get());
    }
    m_changed = false; // don't save values that we just loaded from the file
    m_thread = std::thread([this] { worker(); });
}

/* This CAN NOT be left to the destructor. Instead, Persist::quit() is called from Instance::quit().
 * At the time of ~Persist, Instance::files() (called by save()) is pure virtual. */
void Persist::quit() {
    m_quit = true;
    m_cond.notify_one();
    m_thread.join();
}

template<typename T>
void Persist::set(const std::string& name, T value) {
    std::lock_guard lock(m_mutex);
    m_records.insert_or_assign(name, std::move(value));
    m_changed = true;
}

template<typename T>
T Persist::get(const std::string& name, T defValue) {
    std::lock_guard lock(m_mutex);
    if(m_records.contains(name))
        return std::get<T>(m_records.at(name));
    else
        return defValue;
}

template void Persist::set(const std::string& name, std::string value);
template void Persist::set(const std::string& name, int value);
template std::string Persist::get(const std::string& name, std::string defValue);
template int Persist::get(const std::string& name, int defValue);

void Persist::save() {
    std::ostringstream oss{};
    for(const auto& [key, value] : m_records) {
        if(std::holds_alternative<std::string>(value))
            oss << "options_setString(\"" << key << "\", \"" << std::get<std::string>(value) << "\")\n";
        else
            oss << "options_setInt(\"" << key << "\", " << std::get<int>(value) << ")\n";
    }
    m_instance.files().user(filename)->write(oss.str());
    Log::debug("settings saved");
    m_changed = false;
}

void Persist::worker() {
    Log::debug("settings thread started");
    std::unique_lock lock(m_mutex);
    while(true) {
        m_cond.wait_for(lock, interval);
        if(m_changed)
            save();
        if(m_quit)
            break;
    }
    Log::debug("settings thread exiting");
}