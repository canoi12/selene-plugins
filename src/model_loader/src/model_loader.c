#include "model_loader.h"

extern MODULE_FUNCTION(Model, meta);

int luaopen_model_loader(lua_State* L) {
    lua_newtable(L);
    LOAD_META(Model);
    return 1;
}

int seleneopen_model_loader(lua_State* L) {
  lua_getglobal(L, "package");
  lua_getfield(L, -1, "preload");
  lua_pushcfunction(L, luaopen_model_loader);
  lua_setfield(L, -2, "model_loader");
  lua_pop(L, 2);
  return 0;
}