#include "selGL.h"
#include "default_shaders.h"

MODULE_FUNCTION(Renderer, create) {
    INIT_ARG();
    NEW_UDATA(glRenderer, render);
    memset(render, 0, sizeof(*render));

    NEW_UDATA(glTexture, tex);
    glGenTextures(1, tex);
    render->white_texture = tex;
    lua_rawsetp(L, LUA_REGISTRYINDEX, tex);
    glBindTexture(GL_TEXTURE_2D, *tex);
    unsigned char pixels[] = {255, 255, 255, 255};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    lua_pushnil(L);
    lua_rawsetp(L, LUA_REGISTRYINDEX, &(render->current_program));
    lua_pushnil(L);
    lua_rawsetp(L, LUA_REGISTRYINDEX, &(render->current_texture));
    lua_pushnil(L);
    lua_rawsetp(L, LUA_REGISTRYINDEX, &(render->current_framebuffer));
    
    return 1;
}

MODULE_FUNCTION(Renderer, createProgram2D) {
    NEW_UDATA(glProgram, prog);
    unsigned int vert, frag;
    vert = create_and_compile_shader(GL_VERTEX_SHADER, vert2d_source);
    frag = create_and_compile_shader(GL_FRAGMENT_SHADER, frag2d_source);
    glShader* shaders[] = {vert, frag};
    *prog = create_and_link_program(2, shaders);
    glDeleteShader(vert);
    glDeleteShader(frag);
    return 1;
}

MODULE_FUNCTION(Renderer, createProgram3D) {
    NEW_UDATA(glProgram, prog);
    unsigned int vert, frag;
    vert = create_and_compile_shader(GL_VERTEX_SHADER, vert3d_source);
    frag = create_and_compile_shader(GL_FRAGMENT_SHADER, frag3d_source);
    glShader* shaders[] = {vert, frag};
    *prog = create_and_link_program(2, shaders);
    glDeleteShader(vert);
    glDeleteShader(frag);
    return 1;
}

MODULE_FUNCTION(Renderer, whiteTexture) {
    CHECK_META(glRenderer);
    lua_rawgetp(L, LUA_REGISTRYINDEX, self->white_texture);
    return 1;
}

MODULE_FUNCTION(Renderer, currentProgram) {
    CHECK_META(glRenderer);
    lua_rawgetp(L, LUA_REGISTRYINDEX, &(self->current_program));
    return 1;
}

MODULE_FUNCTION(Renderer, currentTexture) {
    CHECK_META(glRenderer);
    lua_rawgetp(L, LUA_REGISTRYINDEX, &(self->current_texture));
    return 1;
}

MODULE_FUNCTION(Renderer, currentFramebuffer) {
    CHECK_META(glRenderer);
    lua_rawgetp(L, LUA_REGISTRYINDEX, &(self->current_framebuffer));
    return 1;
}

MODULE_FUNCTION(Renderer, setTexture) {
    CHECK_META(glRenderer);
    if (lua_type(L, arg) == LUA_TNIL) {
        glBindTexture(GL_TEXTURE_2D, 0);
        self->current_texture = NULL;
    } else if(lua_type(L, arg) == LUA_TUSERDATA) {
        CHECK_UDATA(glTexture, tex);
        glBindTexture(GL_TEXTURE_2D, *tex);
        self->current_texture = tex;
    }
    return 0;
}

MODULE_FUNCTION(Renderer, setProgram) {
    CHECK_META(glRenderer);
    glProgram* p = NULL;
    if (lua_type(L, arg) == LUA_TNIL) {
        glUseProgram(0);
        lua_pushnil(L);
    } else if(lua_type(L, arg) == LUA_TUSERDATA) {
        CHECK_UDATA(glProgram, prog);
        glUseProgram(*prog);
        p = prog;
        lua_pushvalue(L, arg-1);
    }
    self->current_program = p;
    lua_rawsetp(L, LUA_REGISTRYINDEX, &(self->current_program));
    return 0;
}

MODULE_FUNCTION(Renderer, setFramebuffer) {
    CHECK_META(glRenderer);
    if (lua_type(L, arg) == LUA_TNIL) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        self->current_framebuffer = NULL;
    } else if(lua_type(L, arg) == LUA_TUSERDATA) {
        CHECK_UDATA(glFramebuffer, fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, *fbo);
        self->current_framebuffer = fbo;
    }
    return 0;
}

BEGIN_META(Renderer) {
    BEGIN_REG()
        REG_FIELD(Renderer, create),
        REG_FIELD(Renderer, setTexture),
        REG_FIELD(Renderer, setProgram),
        REG_FIELD(Renderer, setFramebuffer),
    END_REG()
    return 1;
}