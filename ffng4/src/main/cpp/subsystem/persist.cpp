#include "persist.h"
#include <sstream>

Persist::Persist(Instance& instance) :
    m_instance(instance)
{
    auto file = instance.files().user(filename);
    if(file->exists()) {
        Script script(instance, *this);
        script.registerFn("options_setInt", lua::wrap<&Persist::set<int>>);
        script.registerFn("options_setString", lua::wrap<&Persist::set<const std::string&>>);
        script.loadFile(file.get());
    }
}

template<typename T>
void Persist::set(const std::string& name, T value) {
    m_records.insert_or_assign(name, std::move(value));
    save();
}

template<typename T>
T Persist::get(const std::string& name, T defValue) {
    if(m_records.contains(name))
        return std::get<T>(m_records.at(name));
    else
        return defValue;
}

void Persist::save() {
    std::ostringstream oss{};
    for(const auto& [key, value] : m_records) {
        if(std::holds_alternative<std::string>(value))
            oss << "options_setString(\"" << key << "\", \"" << std::get<std::string>(value) << "\")\n";
        else
            oss << "options_setInt(\"" << key << "\", " << std::get<int>(value) << ")\n";
    }
    m_instance.files().user(filename)->write(oss.str());
}

template void Persist::set(const std::string& name, std::string value);
template void Persist::set(const std::string& name, int value);
template std::string Persist::get(const std::string& name, std::string defValue);
template int Persist::get(const std::string& name, int defValue);