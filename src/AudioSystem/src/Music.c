#include "AudioSystem.h"

extern MODULE_FUNCTION(Data, create);
extern MODULE_FUNCTION(Decoder, create);

struct Music {
    char playing;
    char looping;
    Decoder* decoder;
    AudioSpec spec;
    SDL_AudioStream* stream;
    Data* chunk;
};

static MODULE_FUNCTION(Music, create) {
    INIT_ARG();
    CHECK_UDATA(AudioSystem, as);
    CHECK_UDATA(Decoder, dec);

    NEW_UDATA(Music, music);
    music->playing = 0;
    music->looping = 0;
    music->decoder = dec;
    // music->stream = SDL_NewAudioStream()
    music->stream = SDL_NewAudioStream(
        AUDIO_S16SYS,
        dec->info.channels,
        dec->info.sample_rate,
        as->spec.format,
        as->spec.channels,
        as->spec.sampleRate
    );
    lua_pushcfunction(L, l_Data_create);
    lua_pushinteger(L, as->spec.chunkSize);
    if (lua_pcall(L, 1, 1, 0) != LUA_OK)
        return luaL_error(L, "[selene] failed to create Data for audio chunk");
    Data* d = lua_touserdata(L, -1);
    music->chunk = d;
    lua_rawsetp(L, LUA_REGISTRYINDEX, d);

    return 1;
}

static MODULE_FUNCTION(Music, load) {
    INIT_ARG();
    CHECK_UDATA(AudioSystem, as);
    CHECK_STRING(path);
    lua_pushcfunction(L, l_Decoder_create);
    lua_pushvalue(L, 2);
    if (lua_pcall(L, 1, 1, 0) != LUA_OK)
        return luaL_error(L, "[selene] failed to create Decoder for music");
    lua_pushcfunction(L, l_Music_create);
    lua_pushvalue(L, 1);
    lua_pushvalue(L, -3);
    if (lua_pcall(L, 2, 1, 0) != LUA_OK)
        return luaL_error(L, "[selene] failed to create Music");

    lua_pushvalue(L, -2);
    lua_remove(L, -3);
    return 2;
}

static META_FUNCTION(Music, destroy) {
    CHECK_META(Music);
    lua_pushnil(L);
    lua_rawsetp(L, LUA_REGISTRYINDEX, self->chunk);
    SDL_FreeAudioStream(self->stream);
    return 0;
}

BEGIN_META(Music) {
    BEGIN_REG()
        REG_FIELD(Music, create),
        REG_FIELD(Music, load),
    END_REG()
    BEGIN_REG(_index)
        REG_META_FIELD(Music, destroy),
    END_REG()
    NEW_META(Music, _reg, _index_reg);
    return 1;
}