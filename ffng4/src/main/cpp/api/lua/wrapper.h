#ifndef FISH_FILLETS_LUA_ARGS_H
#define FISH_FILLETS_LUA_ARGS_H

namespace lua {

    namespace internal {

        template<typename T>
        inline T read(lua_State *, std::size_t);

        template<>
        inline lua_State *read(lua_State *L, std::size_t) {
            return L;
        }

        template<>
        inline std::string read(lua_State *L, std::size_t index) {
            return luaL_checkstring(L, index);
        }

        template<>
        inline int read(lua_State *L, std::size_t index) {
            return (int) luaL_checkinteger(L, index);
        }

        template<>
        inline float read(lua_State *L, std::size_t index) {
            return (float) luaL_checknumber(L, index);
        }

        template<>
        inline bool read(lua_State *L, std::size_t index) {
            return lua_toboolean(L, index);
        }

        template<>
        inline std::optional<std::string> read(lua_State *L, std::size_t index) {
            return lua_isnoneornil(L, index) ? std::optional<std::string>{} : luaL_checkstring(L, index);
        }

        template<>
        inline std::optional<int> read(lua_State *L, std::size_t index) {
            return lua_isnoneornil(L, index) ? std::optional<int>{} : (int)luaL_checkinteger(L, index);
        }


        template<typename T>
        inline int write(lua_State *L, T value);

        template<>
        inline int write(lua_State *L, std::string value) {
            lua_pushstring(L, value.c_str());
            return 1;
        }

        template<>
        inline int write(lua_State *L, int value) {
            lua_pushinteger(L, value);
            return 1;
        }

        template<>
        inline int write(lua_State *L, unsigned value) {
            lua_pushinteger(L, value);
            return 1;
        }

        template<>
        inline int write(lua_State *L, bool value) {
            lua_pushboolean(L, value);
            return 1;
        }

        template<typename T, typename S>
        inline int write(lua_State *L, const std::pair<T, S> &pair) {
            write(L, pair.first);
            write(L, pair.second);
            return 2;
        }


        template<typename>
        struct args {
            static constexpr std::size_t size = 0;
            lua_State *m_state;

            args(lua_State *L) : m_state(L) {}

            auto state() const { return m_state; }
        };

        template<typename R, typename T, typename... Ts>
        struct args<R(*)(T, Ts...)> : args<R(*)(Ts...)> {
            using args<R(*)(Ts...)>::state;
            static constexpr std::size_t size = 1 + args<R(*)(Ts...)>::size;

            args(lua_State *L) : args<R(*)(Ts...)>(L) {}

            template<std::size_t index>
            auto get(std::size_t orig_index = index) const {
                if constexpr(index == 0)
                    return read<std::decay_t<T>>(state(), orig_index);
                else
                    return args<R(*)(Ts...)>::template get<index - 1>(orig_index);
            }
        };

        template<typename T, typename S, std::size_t... Is>
        auto apply_impl(T f, S args, std::index_sequence<Is...>) {
            return std::invoke(f, args.template get<Is>()...);
        }

        template<typename T, typename S, std::size_t... Is>
        auto apply(T f, S args) {
            return apply_impl(f, args, std::make_index_sequence<S::size>());
        }

    }

    template<auto F>
    int wrap(lua_State *L) {
        try {
            auto args = internal::args<decltype(F)>(L);
            if constexpr(std::is_same<decltype(internal::apply(F, args)), void>::value) {
                internal::apply(F, args);
                return 0;
            } else
                return internal::write(L, internal::apply(F, args));
        } catch(std::exception& e) {
            Log::error("Exception on C++/C boundary: ", e.what());
            return 0;
        }
    }

}

#endif //FISH_FILLETS_LUA_ARGS_H
