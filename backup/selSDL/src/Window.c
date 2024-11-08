#include "selSDL.h"

static MODULE_FUNCTION(Window, create) {
    INIT_ARG();
    CHECK_STRING(title);
    CHECK_INTEGER(x);
    CHECK_INTEGER(y);
    CHECK_INTEGER(width);
    CHECK_INTEGER(height);
    int flags = 0;
    int top = lua_gettop(L);
    if (top >= arg) {
        if (lua_type(L, arg) != LUA_TTABLE)
            return luaL_argerror(L, arg, "Must be a table");
        PUSH_NIL();
        while (lua_next(L, arg) != 0) {
            flags |= (int)luaL_checkinteger(L, -1);
            lua_pop(L, 1);
        }
    } else
    {
        flags = SDL_WINDOW_SHOWN;
    }
    SDL_Window* win = SDL_CreateWindow(
        title,
        x, y,
        width, height,
        flags
    );
    if (!win) return luaL_error(L, "[selene] failed to create SDL window: %s", SDL_GetError());
    NEW_UDATA(sdlWindow, window);
    *window = win;
    return 1;
}

static META_FUNCTION(Window, destroy) {
    CHECK_META(sdlWindow);
    SDL_DestroyWindow(*self);
    return 0;
}

static META_FUNCTION(Window, getSize) {
    INIT_ARG();
    CHECK_UDATA(sdlWindow, window);
    int width, height;
    SDL_GetWindowSize(*window, &width, &height);
    PUSH_INTEGER(width);
    PUSH_INTEGER(height);
    return 2;
}

static META_FUNCTION(Window, setSize) {
    return 0;
}

static META_FUNCTION(Window, getPosition) {
    CHECK_META(sdlWindow);
    int x, y;
    SDL_GetWindowPosition(*self, &x, &y);
    lua_pushinteger(L, x);
    lua_pushinteger(L, y);
    return 2;
}

static META_FUNCTION(Window, setPosition) {
    return 0;
}

static META_FUNCTION(Window, glSwap) {
    CHECK_META(sdlWindow);
    SDL_GL_SwapWindow(*self);
    return 0;
}

static META_FUNCTION(Window, setBordered) {
    return 0;
}

static META_FUNCTION(Window, maximize) {
    CHECK_META(sdlWindow);
    SDL_MaximizeWindow(*self);
    return 0;
}

static META_FUNCTION(Window, minimize) {
    CHECK_META(sdlWindow);
    SDL_MinimizeWindow(*self);
    return 0;
}

static META_FUNCTION(Window, restore) {
    CHECK_META(sdlWindow);
    SDL_RestoreWindow(*self);
    return 0;
}

static META_FUNCTION(Window, showSimpleMessageBox) {
    return 0;
}

int l_Window_meta(lua_State* L) {
    BEGIN_REG()
        REG_FIELD(Window, create),
    END_REG()
    BEGIN_REG(_index)
        REG_META_FIELD(Window, destroy),
        REG_META_FIELD(Window, getSize),
        REG_META_FIELD(Window, setSize),
        REG_META_FIELD(Window, getPosition),
        REG_META_FIELD(Window, setPosition),
        REG_META_FIELD(Window, glSwap),
        REG_META_FIELD(Window, setBordered),
        REG_META_FIELD(Window, maximize),
        REG_META_FIELD(Window, minimize),
        REG_META_FIELD(Window, restore),
        REG_META_FIELD(Window, showSimpleMessageBox),
    END_REG()
    NEW_META(sdlWindow, _reg, _index_reg);
    return 1;
}