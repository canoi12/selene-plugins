#include "selSDL.h"

static SDL_Texture* texture_from_image_data(SDL_Renderer* renderer, int access, ImageData* img_data) {
    int w = img_data->width;
    int h = img_data->height;
    int format = 0;
    switch (img_data->pixel_format) {
        case SELENE_PIXEL_BGR: format = SDL_PIXELFORMAT_BGR888; break;
        case SELENE_PIXEL_RGB: format = SDL_PIXELFORMAT_RGB888; break;
        case SELENE_PIXEL_RGBA: format = SDL_PIXELFORMAT_RGBA32; break;
        case SELENE_PIXEL_BGRA: format = SDL_PIXELFORMAT_BGRA32; break;
    }
    return SDL_CreateTexture(renderer, format, access, w, h);
}

static MODULE_FUNCTION(Texture, create) {
    INIT_ARG();
    CHECK_UDATA(sdlRenderer, render);
    TEST_UDATA(ImageData, img);
    SDL_Texture* tex = NULL;
    if (img) {
        OPT_INTEGER(access, SDL_TEXTUREACCESS_STATIC);
        tex = texture_from_image_data(*render, access, img);
        if (!tex) return luaL_error(L, "[selene] failed to create SDL texture: %s", SDL_GetError());
        int comp = 3;
        if (img->pixel_format == SDL_PIXELFORMAT_RGBA32) comp = 4;
        int pitch = img->width * comp;
        SDL_UpdateTexture(tex, NULL, img->pixels, pitch);
    } else {
        arg--;
        CHECK_INTEGER(format);
        CHECK_INTEGER(access);
        CHECK_INTEGER(width);
        CHECK_INTEGER(height);
        SDL_Texture* tex = SDL_CreateTexture(*render, format, access, width, height);
        if (!tex) return luaL_error(L, "[selene] failed to create SDL texture: %s", SDL_GetError());
        if (lua_type(L, arg) == LUA_TLIGHTUSERDATA) {
            void* data = lua_touserdata(L, arg);
            int comp = 3;
            if (format == SDL_PIXELFORMAT_RGBA32) comp = 4;
            int pitch = width * comp;
            SDL_UpdateTexture(tex, NULL, data, pitch);
        }
    }
    NEW_UDATA(sdlTexture, t);
    *t = tex;
    return 1;
}

static META_FUNCTION(Texture, destroy) {
    CHECK_META(sdlTexture);
    SDL_DestroyTexture(*self);
    return 0;
}

static META_FUNCTION(Texture, query) {
    CHECK_META(sdlTexture);
    Uint32 format;
    int access;
    int w, h;
    SDL_QueryTexture(*self, &format, &access, &w, &h);
    PUSH_INTEGER(format);
    PUSH_INTEGER(access);
    PUSH_INTEGER(w);
    PUSH_INTEGER(h);
    return 4;
}

static META_FUNCTION(Texture, setAlphaMod) {
    CHECK_META(sdlTexture);
    CHECK_INTEGER(alpha);
    SDL_SetTextureAlphaMod(*self, (Uint8)alpha);
    return 0;
}

static META_FUNCTION(Texture, setColorMod) {
    CHECK_META(sdlTexture);
    CHECK_INTEGER(r);
    CHECK_INTEGER(g);
    CHECK_INTEGER(b);
    SDL_SetTextureColorMod(*self, r, g, b);
    return 0;
}

static META_FUNCTION(Texture, setBlendMode) {
    CHECK_META(sdlTexture);
    CHECK_INTEGER(blendMode);
    SDL_SetTextureBlendMode(*self, blendMode);
    return 0;
}

static META_FUNCTION(Texture, setScaleMode) {
    CHECK_META(sdlTexture);
    CHECK_INTEGER(scaleMode);
    SDL_SetTextureScaleMode(*self, scaleMode);
    return 0;
}

int l_sdlTexture_meta(lua_State* L) {
    BEGIN_REG()
        REG_FIELD(Texture, create),
    END_REG()
    BEGIN_REG(_index)
        REG_META_FIELD(Texture, destroy),
        REG_META_FIELD(Texture, query),
        REG_META_FIELD(Texture, setAlphaMod),
        REG_META_FIELD(Texture, setColorMod),
        REG_META_FIELD(Texture, setBlendMode),
        REG_META_FIELD(Texture, setScaleMode),
    END_REG()
    NEW_META(sdlTexture, _reg, _index_reg);
    return 1;
}