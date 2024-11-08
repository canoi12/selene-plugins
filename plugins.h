#ifndef SELENE_PLUGINS_H_
#define SELENE_PLUGINS_H_

#include "platforms.h"
#include "selene.h"

#if defined(BUILD_PLUGINS_AS_DLL)
    #if defined(OS_WIN)
        #define SELENE_PLUGINS_API __declspec(dllexport)
    #else
        #define SELENE_PLUGINS_API extern
    #endif
#else
    #define SELENE_PLUGINS_API
#endif


SELENE_PLUGINS_API int seleneopen_ldtk(lua_State* L);
SELENE_PLUGINS_API int seleneopen_graphics(lua_State* L);
SELENE_PLUGINS_API int seleneopen_gl_helper(lua_State* L);
SELENE_PLUGINS_API int seleneopen_model_loader(lua_State* L);
SELENE_PLUGINS_API int seleneopen_cube(lua_State* L);
SELENE_PLUGINS_API int seleneopen_runner(lua_State* L);
SELENE_PLUGINS_API int seleneopen_json(lua_State* L);
SELENE_PLUGINS_API int seleneopen_AudioSystem(lua_State* L);

const lua_CFunction plugins_list[] = {
        seleneopen_ldtk,
        seleneopen_graphics,
        seleneopen_gl_helper,
        seleneopen_model_loader,
        seleneopen_cube,
        seleneopen_runner,
        seleneopen_json,
        seleneopen_AudioSystem,
        NULL
};

SELENE_PLUGINS_API int luaopen_plugins(lua_State* L);

#endif /* SELENE_PLUGINS_H_ */
