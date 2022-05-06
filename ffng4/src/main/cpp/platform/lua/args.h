#ifndef FISH_FILLETS_LUA_ARGS_H
#define FISH_FILLETS_LUA_ARGS_H

namespace lua {

    template<typename T>
    struct lua_accessor;

    template<>
    struct lua_accessor<types::string> {
        static const char* get(lua_State* L, std::size_t index) {
            return luaL_checkstring(L, index);
        }
    };

    template<>
    struct lua_accessor<types::integer> {
        static int get(lua_State* L, std::size_t index) {
            return (int)luaL_checkinteger(L, index);
        }
    };

    template<>
    struct lua_accessor<types::number> {
        static float get(lua_State* L, std::size_t index) {
            return (float)luaL_checknumber(L, index);
        }
    };

    template<>
    struct lua_accessor<types::boolean> {
        static bool get(lua_State* L, std::size_t index) {
            return lua_toboolean(L, index);
        }
    };

    template<types::string V>
    struct lua_accessor<optional<types::string, V>> {
        static const char* get(lua_State* L, std::size_t index) {
            static_assert(V == nullptr);
            return luaL_optstring(L, index, "");
        }
    };

    template<types::integer V>
    struct lua_accessor<optional<types::integer, V>> {
        static int get(lua_State* L, std::size_t index) {
            return (int)luaL_optinteger(L, index, V);
        }
    };

    template<types::boolean V>
    struct lua_accessor<optional<types::boolean, V>> {
        static bool get(lua_State* L, std::size_t index) {
            static_assert(V == false);
            return lua_toboolean(L, index);
        }
    };

template<typename...>
    struct args {
        static constexpr std::size_t size = 0;
        lua_State* m_state;

        args(lua_State* L) : m_state(L) { }
        auto state() { return m_state; }
    };

    template<typename T, typename... Ts>
    struct args<T, Ts...> : args<Ts...> {
        using type = T;
        using args<Ts...>::state;
        static constexpr std::size_t size = 1 + args<Ts...>::size;

        args(lua_State* L) : args<Ts...>(L) { }

        template<std::size_t index>
        auto get(std::size_t orig_index = index) {
            if constexpr(index == 0)
                return lua_accessor<T>::get(state(), orig_index + 1);
            else
                return args<Ts...>::template get<index - 1>(orig_index);
        }
    };

}

namespace std {

    template<typename... Ts>
    struct tuple_size<lua::args<Ts...>> {
        static constexpr std::size_t value = lua::args<Ts...>::size;
    };

    template<std::size_t I, typename... Ts>
    struct tuple_element<I, lua::args<Ts...>> {
        using type = decltype(std::declval<lua::args<Ts...>>().template get<I>());
    };

}

#endif //FISH_FILLETS_LUA_ARGS_H