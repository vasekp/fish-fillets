#ifndef FISH_FILLETS_WRAPPER_ADDITIONS_H
#define FISH_FILLETS_WRAPPER_ADDITIONS_H

namespace lua::internal {
    template<>
    inline DelayedFunction read(lua_State *L, std::size_t index) {
        return DelayedFunction::from(L);
    }

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

#endif //FISH_FILLETS_WRAPPER_ADDITIONS_H