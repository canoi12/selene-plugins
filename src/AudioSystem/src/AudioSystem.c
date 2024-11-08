#include "AudioSystem.h"

static const char* _default_options =
"return { sampleRate = 44100, channels = 2, samples = 4096 }";

extern MODULE_FUNCTION(AudioDeviceID, create);

static MODULE_FUNCTION(AudioSystem, create) {
    INIT_ARG();
    if (lua_type(L, arg) != LUA_TTABLE || lua_type(L, arg) != LUA_TNIL)
        return luaL_argerror(L, arg, "must be a table");

    lua_pushcfunction(L, l_AudioDeviceID_create);
    lua_pushnil(L);
    lua_pushboolean(L, 0);
    lua_pushvalue(L, arg);
    if (lua_pcall(L, 3, 2, 0) != LUA_OK)
        return luaL_error(L, "[selene] failed to create AudioDeviceID");
    SDL_AudioDeviceID* id = lua_touserdata(L, -1);
    if (!id) {
        return luaL_error(L, "[selene] failed to open audio device %s", SDL_GetError());
    }
    lua_rawsetp(L, LUA_REGISTRYINDEX, id);

    struct AudioSystem* as = lua_newuserdata(L, sizeof(*as));
    luaL_setmetatable(L, "AudioSystem");
    as->devId = id;
    return 1;
}

static META_FUNCTION(AudioSystem, destroy) {
    CHECK_META(AudioSystem);
    if (self->devId) {
        lua_pushnil(L);
        lua_rawsetp(L, LUA_REGISTRYINDEX, self->devId);
        SDL_PauseAudioDevice(*(self->devId), 0);
        SDL_CloseAudioDevice(*(self->devId));
    }
    return 0;
}

BEGIN_META(AudioSystem) {
    BEGIN_REG()
        REG_FIELD(AudioSystem, create),
    END_REG()
    BEGIN_REG(_index)
        REG_META_FIELD(AudioSystem, destroy),
    END_REG()
    NEW_META(AudioSystem, _reg, _index_reg);
    return 1;
}