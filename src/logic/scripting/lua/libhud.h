#ifndef LOGIC_SCRIPTING_API_LIBHUD_H_
#define LOGIC_SCRIPTING_API_LIBHUD_H_

#include "lua_commons.h"

extern int l_hud_open_inventory(lua_State* L);
extern int l_hud_close_inventory(lua_State* L);
extern int l_hud_open_block(lua_State* L);
extern int l_hud_open_permanent(lua_State* L);
extern int l_hud_close(lua_State* L);

static const luaL_Reg hudlib [] = {
    {"open_inventory", lua_wrap_errors<l_hud_open_inventory>},
    {"close_inventory", lua_wrap_errors<l_hud_close_inventory>},
    {"open_block", lua_wrap_errors<l_hud_open_block>},
    {"open_permanent", lua_wrap_errors<l_hud_open_permanent>},
    {"close", lua_wrap_errors<l_hud_close>},
    {NULL, NULL}
};

#endif // LOGIC_SCRIPTING_API_LIBHUD_H_
