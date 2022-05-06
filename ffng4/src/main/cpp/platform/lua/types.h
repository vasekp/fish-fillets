#ifndef FISH_FILLETS_LUA_TYPES_H
#define FISH_FILLETS_LUA_TYPES_H

namespace lua {

    namespace types {
        using string = const char *;
        using integer = lua_Integer;
        using number = double;
        using boolean = bool;
    }

    /* Declaration only */
    /* V currently only used for lua::types::integer */
    template<typename T, T V = T()>
    struct optional;

}

#endif //FISH_FILLETS_LUA_TYPES_H