#ifndef FISH_FILLETS_SCRIPT_QUEUED_FUNCTION_H
#define FISH_FILLETS_SCRIPT_QUEUED_FUNCTION_H

class QueuedFunction {
    lua_State* m_state;
    int m_ref;
    int m_tries;

public:
    QueuedFunction() = delete;
    QueuedFunction(const QueuedFunction&) = delete;
    QueuedFunction& operator=(const QueuedFunction&) = delete;
    QueuedFunction(QueuedFunction&& other) noexcept;
    QueuedFunction& operator=(QueuedFunction&& other) noexcept;
    ~QueuedFunction();

    static QueuedFunction from(lua_State* L);

    bool call();

private:
    QueuedFunction(lua_State* L, int ref) : m_state(L), m_ref(ref), m_tries(0) { }
};

#endif //FISH_FILLETS_SCRIPT_QUEUED_FUNCTION_H