#include "selSDL.h"

static MODULE_FUNCTION(Font, create) {
    INIT_ARG();
    CHECK_UDATA(sdlRenderer, renderer);
    CHECK_UDATA(FontData, font_data);
    ImageData* imgd = (ImageData*)font_data;
    NEW_UDATA(sdlFont, font);
    NEW_UDATA(sdlTexture, tex);
    // lua_rawsetp(L, LUA_REGISTRYINDEX, font);
    font->texture = tex;
    font->font_data = font_data;
    *tex = SDL_CreateTexture(*renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, imgd->width, imgd->height);
    if (*tex == NULL)
        return luaL_error(L, "[selene] failed to create SDL_Texture for Font");
    SDL_UpdateTexture(*tex, NULL, imgd->pixels, imgd->width * imgd->channels);
    return 2;
}

static META_FUNCTION(Font, gc) {

    return 0;
}

BEGIN_META(Font) {
    BEGIN_REG()
        REG_FIELD(Font, create),
    END_REG()
    luaL_newmetatable(L, "sdlFont");
    luaL_setfuncs(L, _reg, 0);
    return 1;
}