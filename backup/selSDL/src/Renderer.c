#include "selSDL.h"
#include "helper.h"

static MODULE_FUNCTION(Renderer, create) {
    INIT_ARG();
    CHECK_UDATA(sdlWindow, win);
    OPT_INTEGER(index, -1);
    OPT_INTEGER(flags, SDL_RENDERER_ACCELERATED);
    if (*win == NULL) return luaL_error(L, "[selene] invalid SDL window");
    SDL_Renderer* r = SDL_CreateRenderer(*win, index, flags);
    if (r == NULL) return luaL_error(L, "[selene] failed to create SDL renderer: %s", SDL_GetError());
    NEW_UDATA(sdlRenderer, render);
    *render = r;
    return 1;
}

static META_FUNCTION(Renderer, destroy) {
    CHECK_META(sdlRenderer);
    SDL_DestroyRenderer(*self);
    return 0;
}

static META_FUNCTION(Renderer, clear) {
    CHECK_META(sdlRenderer);
    SDL_RenderClear(*self);
    return 0;
}

static META_FUNCTION(Renderer, setColor) {
    CHECK_META(sdlRenderer);
    Uint8 d[4];
    for (int i = 0; i < 4; i++) d[i] = (Uint8)luaL_checkinteger(L, i+2);
    SDL_SetRenderDrawColor(*self, d[0], d[1], d[2], d[3]);
    return 0;
}

static META_FUNCTION(Renderer, setTarget) {
    CHECK_META(sdlRenderer);
    const TEST_UDATA(sdlTexture, tex);
    SDL_SetRenderTarget(*self, *tex);
    return 0;
}

static META_FUNCTION(Renderer, setClipRect) {
    CHECK_META(sdlRenderer);
    SDL_Rect* psrc = NULL;
    if (lua_type(L, arg) == LUA_TTABLE) {
        int src[4];
        for (int i = 0; i < 4; i++) {
            lua_rawgeti(L, arg, i+1);
            src[i] = (int)luaL_checkinteger(L, -1);
            lua_pop(L, 1);
        }
        psrc = (SDL_Rect*)src;
    }
    SDL_RenderSetClipRect(*self, psrc);
    return 0;
}

static META_FUNCTION(Renderer, setBlendMode) {
    CHECK_META(sdlRenderer);
    CHECK_INTEGER(blendMode);
    SDL_SetRenderDrawBlendMode(*self, blendMode);
    return 0;
}

static META_FUNCTION(Renderer, drawPoint) {
    CHECK_META(sdlRenderer);
    const CHECK_NUMBER(float, x);
    const CHECK_NUMBER(float, y);
    SDL_RenderDrawPointF(*self, x, y);
    return 0;
}

static META_FUNCTION(Renderer, drawLine) {
    CHECK_META(sdlRenderer);
    const CHECK_NUMBER(float, x0);
    const CHECK_NUMBER(float, y0);
    const CHECK_NUMBER(float, x1);
    const CHECK_NUMBER(float, y1);
    SDL_RenderDrawLineF(*self, x0, y0, x1, y1);
    return 0;
}

static META_FUNCTION(Renderer, drawTriangle) {
    CHECK_META(sdlRenderer);
    float pos[6];
    for (int i = 0; i < 6; i++) pos[i] = (float)luaL_checknumber(L, i+2);
    SDL_RenderDrawLineF(*self, pos[0], pos[1], pos[2], pos[3]);
    SDL_RenderDrawLineF(*self, pos[2], pos[3], pos[4], pos[5]);
    SDL_RenderDrawLineF(*self, pos[4], pos[5], pos[0], pos[1]);
    return 0;
}

static void s_vertices_swap(float v0[2], float v1[2]) {
    float aux[2] = {v0[0], v0[1]};
    // printf("V: {%d %d} {%d %d}\n", v0[0], v0[1], v1[0], v1[1]);
    v0[0] = v1[0];
    v0[1] = v1[1];
    v1[0] = aux[0];
    v1[1] = aux[1];
    // *v0 = *v1;
    // *v1 = aux;
}

static void s_vertices_y_sort(float v[3][2]) {
    if (v[1][1] < v[0][1]) s_vertices_swap(v[0], v[1]);
    if (v[2][1] < v[1][1]) s_vertices_swap(v[1], v[2]);
    if (v[2][1] < v[0][1]) s_vertices_swap(v[0], v[2]);
}

static void s_bottom_flat_triangle(SDL_Renderer* r, float v[3][2]) {
    float invslope1 = (float)(v[1][0] - v[0][0]) / (float)(v[1][1] - v[0][1]);
    float invslope2 = (float)(v[2][0] - v[0][0]) / (float)(v[2][1] - v[0][1]);
	
    float curx1, curx2;
    curx1 = curx2 = v[0][0];

    int scanY;
    for (scanY = (int)v[0][1]; scanY <= v[1][1]; scanY++) {
        float v0[2] = {curx1, (float)scanY};
        float v1[2] = {curx2, (float)scanY};
        SDL_RenderDrawLineF(r, v0[0], v0[1], v1[0], v1[1]);
        curx1 += invslope1;
        curx2 += invslope2;
    }
}

static void s_top_flat_triangle(SDL_Renderer* r, float v[3][2]) {
    float invslope1 = (v[2][0] - v[0][0]) / (v[2][1] - v[0][1]);
    float invslope2 = (v[2][0] - v[1][0]) / (v[2][1] - v[1][1]);
	
    float curx1, curx2;
    curx1 = curx2 = v[2][0];

    int scanY;
    for (scanY = (int)v[2][1]; scanY >= v[0][1]; scanY--) {
        float v0[2] = {curx1, (float)scanY};
        float v1[2] = {curx2, (float)scanY};
        SDL_RenderDrawLineF(r, v0[0], v0[1], v1[0], v1[1]);
        curx1 -= invslope1;
        curx2 -= invslope2;
    }
}

static META_FUNCTION(Renderer, fillTriangle) {
    CHECK_META(sdlRenderer);
    float v[3][2] = {
        {0.f, 0.f},
        {0.f, 0.f},
        {0.f, 0.f}
    };
    float* p = v[0];
    for (int i = 0; i < 6; i++) p[i] = (float)luaL_checknumber(L, i+2);
    s_vertices_y_sort(v);
    if (v[1][1] == v[2][1]) s_bottom_flat_triangle(*self, v);
    else if (v[0][1] == v[1][1]) s_top_flat_triangle(*self, v);
    else {
        float v3[2];
        v3[0] = v[0][0] + ((v[1][1] - v[0][1]) / (v[2][1] - v[0][1]))  * (v[2][0] - v[0][0]);
        v3[1] = v[1][1];
        float va[3][2] = {
            {v[0][0], v[0][1]},
            {v[1][0], v[1][1]},
            {v3[0], v3[1]}
        };
        float vb[3][2] = {
           {v[1][0], v[1][1]},
            {v3[0], v3[1]},
            {v[2][0], v[2][1]}
        };
        s_bottom_flat_triangle(*self, va);
        s_top_flat_triangle(*self, vb);
    }
    return 0;
}

static META_FUNCTION(Renderer, drawRect) {
    CHECK_META(sdlRenderer);
    float d[4];
    for (int i = 0; i < 4; i++) {
        d[i] = (float)luaL_checknumber(L, i+2);
    }
    SDL_FRect rect = {d[0], d[1], d[2], d[3]};
    SDL_RenderDrawRectF(*self, &rect);
    return 0;
}

static META_FUNCTION(Renderer, fillRect) {
    CHECK_META(sdlRenderer);
    float d[4];
    for (int i = 0; i < 4; i++) {
        d[i] = (float)luaL_checknumber(L, i+2);
    }
    SDL_FRect rect = {d[0], d[1], d[2], d[3]};
    SDL_RenderFillRectF(*self, &rect);
    return 0;
}

static META_FUNCTION(Renderer, drawCircle) {
    CHECK_META(sdlRenderer);
    const CHECK_NUMBER(float, x);
    const CHECK_NUMBER(float, y);
    const CHECK_INTEGER(radius);
    const OPT_INTEGER(sides, 32);

    for (int i = 0; i < sides; i++) {
        float tetha = ((float)i * (float)M_PI2) / (float)sides;
        float p0[2];
        p0[0] = x + (cosf(tetha) * (float)radius);
        p0[1] = y + (sinf(tetha) * (float)radius);

        tetha = ((float)(i+1) * (float)M_PI2) / (float)sides;
        float p1[2];
        p1[0] = x + (cosf(tetha) * (float)radius);
        p1[1] = y + (sinf(tetha) * (float)radius);
        SDL_RenderDrawLineF(*self, p0[0], p0[1], p1[0], p1[1]);
    }
    return 0;
}

static META_FUNCTION(Renderer, fillCircle) {
    CHECK_META(sdlRenderer);
    const CHECK_NUMBER(float, cx);
    const CHECK_NUMBER(float, cy);
    const CHECK_INTEGER(radius);
    int x = radius;
    int y = 0;
    int err = 1 - x;

    while (x >= y) {
        SDL_RenderDrawLineF(*self, cx - x, cy + y, cx + x, cy + y);
        SDL_RenderDrawLineF(*self, cx - y, cy + x, cx + y, cy + x);
        SDL_RenderDrawLineF(*self, cx - x, cy - y, cx + x, cy - y);
        SDL_RenderDrawLineF(*self, cx - y, cy - x, cx + y, cy - x);

        y++;
        if (err < 0) {
            err += 2 * y + 1;
        } else {
            x--;
            err += 2 * (y - x) + 1;
        }
    }

    return 0;
}

static META_FUNCTION(Renderer, copy) {
    CHECK_META(sdlRenderer);
    CHECK_UDATA(sdlTexture, tex);
    SDL_Rect* psrc = NULL;
    SDL_FRect* pdst = NULL;
    if (lua_type(L, arg) == LUA_TTABLE) {
        int src[4];
        for (int i = 0; i < 4; i++) {
            lua_rawgeti(L, arg, i+1);
            src[i] = (int)luaL_checkinteger(L, -1);
            lua_pop(L, 1);
        }
        psrc = (SDL_Rect*)src;
    }
    if (lua_type(L, arg+1) == LUA_TTABLE) {
        float dst[4];
        for (int i = 0; i < 4; i++) {
            lua_rawgeti(L, arg+1, i+1);
            dst[i] = (float)luaL_checknumber(L, -1);
            lua_pop(L, 1);
        }
        pdst = (SDL_FRect*)dst;
    }
    
    SDL_RenderCopyF(*self, *tex, psrc, pdst);
    return 0;
}

static META_FUNCTION(Renderer, copyEx) {
    CHECK_META(sdlRenderer);
    CHECK_UDATA(sdlTexture, tex);
    SDL_Rect* psrc = NULL;
    SDL_FRect* pdst = NULL;
    if (lua_type(L, arg) == LUA_TTABLE) {
        int src[4];
        for (int i = 0; i < 4; i++) {
            lua_rawgeti(L, arg, i+1);
            src[i] = (int)luaL_checkinteger(L, -1);
            lua_pop(L, 1);
        }
        psrc = (SDL_Rect*)src;
    }
    if (lua_type(L, arg+1) == LUA_TTABLE) {
        float dst[4];
        for (int i = 0; i < 4; i++) {
            lua_rawgeti(L, arg+1, i+1);
            dst[i] = (float)luaL_checknumber(L, -1);
            lua_pop(L, 1);
        }
        pdst = (SDL_FRect*)dst;
    }
    arg += 2;
    OPT_NUMBER(double, angle, 0.f);
    SDL_FPoint* center = NULL;
    if (lua_type(L, arg) == LUA_TTABLE) {
        float ctr[2];
        for (int i = 0; i < 2; i++) {
            lua_rawgeti(L, arg+1, i+1);
            ctr[i] = (float)luaL_checknumber(L, -1);
            lua_pop(L, 1);
        }
        center = (SDL_FPoint*)ctr;
    }
    arg += 1;
    OPT_INTEGER(flip, SDL_FLIP_NONE);
    SDL_RenderCopyExF(*self, *tex, psrc, pdst, angle, center, flip);
    return 0;
}

static META_FUNCTION(Renderer, print) {
    CHECK_META(sdlRenderer);
    CHECK_UDATA(sdlFont, font);
    CHECK_STRING(text);
    OPT_NUMBER(float, x, 0);
    OPT_NUMBER(float, y, 0);
    float ox = x;
    float oy = y;

    float x0, y0;
    x0 = 0.f;
    y0 = 0.f;
    uint8_t* p = (uint8_t*)text;
    int w = font->font_data->image_width;
    int h = font->font_data->image_height;
    while (*p != 0) {
        int codepoint;
        p = utf8_codepoint(p, &codepoint);
        SDL_FRect dest;
        int pos[2];
        int src[4];
        char_rect(font->font_data, codepoint, &x0, &y0, pos, src, 0);
        dest.x = x + pos[0];
        dest.y = y + pos[1];
        dest.w = src[2];
        dest.h = src[3];
        // fprintf(stderr, "%c:%d", *p, *p);
        // fprintf(stdout, "src (%c): %d %d %d %d\n", *p, src[0], src[1], src[2], src[3]);

        SDL_Rect t;
        t.x = src[0];
        t.y = src[1];
        t.w = src[2];
        t.h = src[3];
        SDL_RenderCopyF(*self, *font->texture, &t, &dest);
    }
    // fprintf(stderr, "\n");
    
    return 0;
}

static META_FUNCTION(Renderer, flush) {
    CHECK_META(sdlRenderer);
    SDL_RenderFlush(*self);
    return 0;
}

static META_FUNCTION(Renderer, present) {
    CHECK_META(sdlRenderer);
    SDL_RenderPresent(*self);
    return 0;
}

int l_Renderer_meta(lua_State* L) {
    BEGIN_REG()
        REG_FIELD(Renderer, create),
    END_REG()
    BEGIN_REG(_index)
        REG_META_FIELD(Renderer, destroy),
        REG_META_FIELD(Renderer, clear),
        REG_META_FIELD(Renderer, setColor),
        REG_META_FIELD(Renderer, setTarget),
        REG_META_FIELD(Renderer, setClipRect),
        REG_META_FIELD(Renderer, setBlendMode),
        REG_META_FIELD(Renderer, drawPoint),
        REG_META_FIELD(Renderer, drawLine),
        REG_META_FIELD(Renderer, drawTriangle),
        REG_META_FIELD(Renderer, fillTriangle),
        REG_META_FIELD(Renderer, drawRect),
        REG_META_FIELD(Renderer, fillRect),
        REG_META_FIELD(Renderer, drawCircle),
        REG_META_FIELD(Renderer, fillCircle),
        REG_META_FIELD(Renderer, copy),
        REG_META_FIELD(Renderer, copyEx),
        REG_META_FIELD(Renderer, print),
        REG_META_FIELD(Renderer, flush),
        REG_META_FIELD(Renderer, present),
    END_REG()
    NEW_META(sdlRenderer, _reg, _index_reg);
    return 1;
}