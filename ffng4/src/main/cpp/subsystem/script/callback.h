#ifndef FISH_FILLETS_SCRIPT_QUEUED_FUNCTION_H
#define FISH_FILLETS_SCRIPT_QUEUED_FUNCTION_H

#include <functional>
#include <variant>

class LuaCallback {
    lua_State* m_state;
    int m_ref;
    int m_tries;

public:
    LuaCallback() = delete;
    LuaCallback(const LuaCallback&) = delete;
    LuaCallback& operator=(const LuaCallback&) = delete;
    LuaCallback(LuaCallback&& other) noexcept;
    LuaCallback& operator=(LuaCallback&& other) noexcept;
    ~LuaCallback();

    static LuaCallback from(lua_State* L);

    bool operator()();

private:
    LuaCallback(lua_State* L, int ref) : m_state(L), m_ref(ref), m_tries(0) { }
};

class Callback : public std::variant<std::function<bool()>, LuaCallback> {
public:
    using std::variant<std::function<bool()>, LuaCallback>::variant;
    bool operator()();
};

#endif //FISH_FILLETS_SCRIPT_QUEUED_FUNCTION_H