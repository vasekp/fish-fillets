#ifndef FISH_FILLETS_LUA_FUNCTION_H
#define FISH_FILLETS_LUA_FUNCTION_H

class DelayedFunction {
    lua_State* m_state;
    int m_ref;
    int m_tries;

public:
    DelayedFunction() = delete;
    DelayedFunction(const DelayedFunction&) = delete;
    DelayedFunction& operator=(const DelayedFunction&) = delete;
    DelayedFunction(DelayedFunction&& other) noexcept;
    DelayedFunction& operator=(DelayedFunction&& other) noexcept;
    ~DelayedFunction();

    static DelayedFunction from(lua_State* L);

    bool call();

private:
    DelayedFunction(lua_State* L, int ref) : m_state(L), m_ref(ref), m_tries(0) { }
};

#endif //FISH_FILLETS_LUA_FUNCTION_H