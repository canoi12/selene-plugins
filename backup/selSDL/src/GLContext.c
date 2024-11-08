#include "selSDL.h"

static MODULE_FUNCTION(GLContext, create) {
    INIT_ARG();
    CHECK_UDATA(sdlWindow, win);
    NEW_UDATA(sdlGLContext, ctx);
    *ctx = SDL_GL_CreateContext(*win);
    return 1;
}

static META_FUNCTION(GLContext, destroy) {
    CHECK_META(sdlGLContext);
    SDL_GL_DeleteContext(*self);
    return 0;
}

BEGIN_META(GLContext) {
    BEGIN_REG()
        REG_FIELD(GLContext, create),
    END_REG()
    BEGIN_REG(_index)
        REG_META_FIELD(GLContext, destroy),
    END_REG()
    NEW_META(sdlGLContext, _reg, _index_reg);
    return 1;
}
