#ifndef FISH_FILLETS_SCRIPT_H
#define FISH_FILLETS_SCRIPT_H

#include "instance.h"
#include "platform/lua.h"

class ScriptReferrer {
public:
    virtual ~ScriptReferrer() { }
};

class Script {
    Instance& m_instance;
    ScriptReferrer& m_ref;
    lua::Env m_env;

public:
    Script(Instance& instance, ScriptReferrer& ref);
    Script(const Script&) = delete;
    Script& operator=(const Script&) = delete;
    static Script& from(lua_State*);

    ScriptReferrer& ref() const { return m_ref; }

    void registerFn(const std::string& name, lua_CFunction function);
    void doString(const std::string& string);
    void loadFile(const std::string& filename);

    void file_include(const std::string& filename);
    bool file_exists(const std::string& filename);
    void sendMsg(const std::string& target, const std::string& text);
};

namespace lua::internal {
    template<typename M, typename R, typename... Ts>
    struct args<R(M::*)(Ts...)> : args<R(*)(Ts...)> {
        using args<R(*)(Ts...)>::state;
        static constexpr std::size_t size = 1 + args<R(*)(Ts...)>::size;

        args(lua_State *L) : args<R(*)(Ts...)>(L) {}

        template<std::size_t index>
        auto get(std::size_t orig_index = index) const {
            if constexpr(index == 0)
                return dynamic_cast<M*>(&Script::from(state()).ref());
            else
                return args<R(*)(Ts...)>::template get<index - 1>(orig_index);
        }
    };

    template<typename M, typename R, typename... Ts>
    struct args<R(M::*)(Ts...) const> : args<R(*)(Ts...)> {
        using args<R(*)(Ts...)>::state;
        static constexpr std::size_t size = 1 + args<R(*)(Ts...)>::size;

        args(lua_State *L) : args<R(*)(Ts...)>(L) {}

        template<std::size_t index>
        auto get(std::size_t orig_index = index) const {
            if constexpr(index == 0)
                return dynamic_cast<const M*>(&Script::from(state()).ref());
            else
                return args<R(*)(Ts...)>::template get<index - 1>(orig_index);
        }
    };

    template<typename R, typename... Ts>
    struct args<R(Script::*)(Ts...)> : args<R(*)(Ts...)> {
        using args<R(*)(Ts...)>::state;
        static constexpr std::size_t size = 1 + args<R(*)(Ts...)>::size;

        args(lua_State *L) : args<R(*)(Ts...)>(L) {}

        template<std::size_t index>
        auto get(std::size_t orig_index = index) const {
            if constexpr(index == 0)
                return &Script::from(state());
            else
                return args<R(*)(Ts...)>::template get<index - 1>(orig_index);
        }
    };
}

#endif //FISH_FILLETS_SCRIPT_H