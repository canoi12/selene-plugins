#include "plugins.h"

/**
 * Setup lua plugins
 */
static int l_plugins_setup(lua_State* L) {
    int i = 0;
    for (;plugins_list[i] != NULL; i++) {
        plugins_list[i](L);
    }
    return 0;
}

/**
 * Open plugin lib
 */
int luaopen_plugins(lua_State* L) {
    lua_newtable(L);
    lua_pushcfunction(L, l_plugins_setup);
    lua_setfield(L, -2, "setup");
    return 1;
}