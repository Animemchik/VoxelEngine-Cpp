#ifndef LOGIC_SCRIPTING_LUA_H_
#define LOGIC_SCRIPTING_LUA_H_

#include <lua.hpp>
#include <exception>

namespace lua {
    using luaint = lua_Integer;
    using luanumber = lua_Number;
}

template <lua_CFunction func> int lua_wrap_errors(lua_State *L) {
    int result = 0;
    try {
        result = func(L);
    }
    // transform exception with description into lua_error
    catch (std::exception &e) {
        luaL_error(L, e.what());
    }
    // Rethrow any other exception (lua error for example)
    catch (...) {
        throw;
    }
    return result;
}

#endif // LOGIC_SCRIPTING_LUA_H_
