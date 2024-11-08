#include "selene.h"
#include "lua_helper.h"
#include "selGL.h"

extern MODULE_FUNCTION(Texture, meta);
extern MODULE_FUNCTION(Framebuffer, meta);
extern MODULE_FUNCTION(Buffer, meta);
extern MODULE_FUNCTION(VertexArray, meta);
extern MODULE_FUNCTION(Shader, meta);
extern MODULE_FUNCTION(Program, meta);

struct GLInfo {
    uint8_t major, minor;
    uint16_t glsl;
    uint16_t es;
};

static struct GLInfo _gl_info;

static MODULE_FUNCTION(gl, setup) {
#if !defined(__EMSCRIPTEN__)
    INIT_ARG();
    CHECK_LUDATA(void, proc_fn);
    if (gladLoadGLLoader((GLADloadproc)proc_fn)) return 0;
#else
    if (1) return 0;
#endif
    else return luaL_error(L, "[selene] failed to load Modern OpenGL functions");
}

static MODULE_FUNCTION(gl, loadGlad) {
    INIT_ARG();
    CHECK_LUDATA(void, proc_fn);
#if !defined(__EMSCRIPTEN__)
    if (!gladLoadGLLoader((GLADloadproc)proc_fn)) {
        return luaL_error(L, "Failed to init GLAD");
    }
#endif
    return 0;
}

static MODULE_FUNCTION(gl, getString) {
    INIT_ARG();
    CHECK_INTEGER(flag);
    PUSH_STRING(glGetString(flag));
    return 1;
}

static MODULE_FUNCTION(gl, viewport) {
    int view[4];
    glGetIntegerv(GL_VIEWPORT, view);
    int args = lua_gettop(L);
    for (int i = 0; i < args; i++) {
        view[i] = (int)luaL_checkinteger(L, 1+i);
    }
    glViewport(view[0], view[1], view[2], view[3]);
    return 0;
}

static MODULE_FUNCTION(gl, clearDepth) {
    INIT_ARG();
    OPT_NUMBER(float, depth, 1.f);
    #if !defined(__EMSCRIPTEN__)
        glClearDepth(depth);
    #else
        glClearDepthf(depth);
    #endif
    return 0;
}

static MODULE_FUNCTION(gl, clearColor) {
    float color[4] = { 0.f, 0.f, 0.f, 1.f };
    int args = lua_gettop(L);
    for (int i = 0; i < args; i++)
        color[i] = (float)lua_tonumber(L, i+1);
    glClearColor(color[0], color[1], color[2], color[3]);
    return 0;
}

static MODULE_FUNCTION(gl, clear) {
    GLenum flags = 0;
    int args = lua_gettop(L);
    for (int i = 0; i < args; i++) {
        flags |= luaL_checkinteger(L, 1+i);
    }
    glClear(flags);
    return 0;
}

static MODULE_FUNCTION(gl, enable) {
    int args = lua_gettop(L);
    for (int i = 0; i < args; i++)
        glEnable((GLenum)luaL_checkinteger(L, 1+i));
    return 0;
}

static MODULE_FUNCTION(gl, disable) {
    int args = lua_gettop(L);
    for (int i = 0; i < args; i++)
        glDisable((GLenum)luaL_checkinteger(L, 1+i));
    return 0;
}

static MODULE_FUNCTION(gl, scissor) {
    INIT_ARG();
    CHECK_INTEGER(x);
    CHECK_INTEGER(y);
    CHECK_INTEGER(w);
    CHECK_INTEGER(h);
    glScissor(x, y, w, h);
    return 0;
}

static MODULE_FUNCTION(gl, blendFunc) {
    INIT_ARG();
    OPT_INTEGER(sfn, GL_SRC_ALPHA);
    OPT_INTEGER(dfn, GL_ONE_MINUS_SRC_ALPHA);
    glBlendFunc(sfn, dfn);
    return 0;
}

static MODULE_FUNCTION(gl, blendEquation) {
    INIT_ARG();
    CHECK_INTEGER(value);
    glBlendEquation(value);
    return 0;
}

static MODULE_FUNCTION(gl, drawArrays) {
    uint16_t mode = (uint16_t)luaL_checkinteger(L, 1);
    uint32_t start = (uint32_t)luaL_checkinteger(L, 2);
    uint32_t count = (uint32_t)luaL_checkinteger(L, 3);
    glDrawArrays(mode, start, count);
    return 0;
}

static MODULE_FUNCTION(gl, drawElements) {
    uint16_t mode = (uint16_t)luaL_checkinteger(L, 1);
    uint32_t count = (uint32_t)luaL_checkinteger(L, 2);
    uint16_t _type = (uint16_t)luaL_checkinteger(L, 3);
    uint32_t start = (uint32_t)luaL_checkinteger(L, 4);
    glDrawElements(mode, count, _type, (void*)start);
    return 0;
}

/* Uniforms */

static MODULE_FUNCTION(gl, uniform1fv) {
    int location = (int)luaL_checkinteger(L, 1);
    int args = (int)lua_gettop(L) - 1;
#if defined(OS_WIN)
    float values[1024];
#else
    float values[args];
#endif
    for (int i = 0; i < args; i++) {
        values[i] = (float)luaL_checknumber(L, 2+i);
    }
    glUniform1fv(location, args, values);
    return 0;
}

static MODULE_FUNCTION(gl, uniform2fv) {
    int location = (int)luaL_checkinteger(L, 1);
    int args = lua_gettop(L) - 2;
    int size = args * 4 * 2;
#if defined(OS_WIN)
    float values[1024];
#else
    float values[args*2];
#endif
    float* v = values;
    for (int i = 0; i < args; i++) {
        int index = i + 2;
        if (lua_type(L, index) != LUA_TTABLE)
            return luaL_argerror(L, index, "Must be a table");
        lua_pushnil(L);
        int j = 0;
        while (lua_next(L, index) != 0) {
            v[j++] = (float)luaL_checknumber(L, -1);
            lua_pop(L, 1);
        }
        v += 2;
    }
    glUniform2fv(location, args, values);
    return 0;
}

static MODULE_FUNCTION(gl, uniform3fv) {
    int location = (int)luaL_checkinteger(L, 1);
    int args = lua_gettop(L) - 2;
    int size = args * 4 * 3;
#if defined(OS_WIN)
    float values[1024];
#else
    float values[args*3];
#endif
    float *v = values;
    for (int i = 0; i < args; i++) {
        int index = i + 2;
        if (lua_type(L, index) != LUA_TTABLE)
            return luaL_argerror(L, index, "Must be a table");
        lua_pushnil(L);
        int j = 0;
        while (lua_next(L, index) != 0) {
            v[j++] = (float)luaL_checknumber(L, -1);
            lua_pop(L, 1);
        }
        v += 3;
    }
    glUniform3fv(location, args, values);
    return 0;
}

static MODULE_FUNCTION(gl, uniform4fv) {
    int location = (int)luaL_checkinteger(L, 1);
    int args = lua_gettop(L) - 2;
    int size = args * 4 * 4;
#if defined(OS_WIN)
    float values[1024];
#else
    float values[args*4];
#endif
    float *v = values;
    for (int i = 0; i < args; i++) {
        int index = i + 2;
        if (lua_type(L, index) != LUA_TTABLE)
            return luaL_argerror(L, index, "Must be a table");
        lua_pushnil(L);
        int j = 0;
        while (lua_next(L, index) != 0) {
            v[j++] = (float)luaL_checknumber(L, -1);
            lua_pop(L, 1);
        }
        v += 4;
    }
    glUniform4fv(location, args, values);
    return 0;
}

static MODULE_FUNCTION(gl, uniformMatrix4fv) {
    INIT_ARG();
    CHECK_INTEGER(location);
    CHECK_INTEGER(count);
    GET_BOOLEAN(normalize);
    float m[16];
    if (lua_type(L, arg) == LUA_TUSERDATA) {
        float* udata = luaL_checkudata(L, arg, "mat4x4");
        memcpy(m, udata, sizeof(m));
    }
    else if (lua_type(L, arg) == LUA_TTABLE) {
        for (int i = 0; i < 16; i++) {
        lua_rawgeti(L, arg, i+1);
        m[i] = lua_tonumber(L, -1);
        lua_pop(L, 1);
    }
    }
    else return luaL_argerror(L, arg, "[selene] invalid matrix value, must be a table or Mat4");
    
    glUniformMatrix4fv(location, count, normalize, (const float*)m);
    return 0;
}

#include "glEnums.h"

THIRD_LIB_API int luaopen_selGL(lua_State* L) {
    BEGIN_REG()
        REG_FIELD(gl, setup),
        REG_FIELD(gl, getString),
        REG_FIELD(gl, clearDepth),
        REG_FIELD(gl, clearColor),
        REG_FIELD(gl, clear),
        REG_FIELD(gl, viewport),
        REG_FIELD(gl, scissor),
        REG_FIELD(gl, enable),
        REG_FIELD(gl, disable),
        REG_FIELD(gl, blendFunc),
        REG_FIELD(gl, blendEquation),
        REG_FIELD(gl, drawArrays),
        REG_FIELD(gl, drawElements),
        /* Uniform */
        REG_FIELD(gl, uniform1fv),
        REG_FIELD(gl, uniform2fv),
        REG_FIELD(gl, uniform3fv),
        REG_FIELD(gl, uniform4fv),
        REG_FIELD(gl, uniformMatrix4fv),
    END_REG()
    luaL_newlib(LUA_STATE_NAME, _reg);
    LOAD_META(Texture);
    LOAD_META(Framebuffer);
    LOAD_META(Buffer);
    LOAD_META(VertexArray);
    LOAD_META(Shader);
    LOAD_META(Program);
    LOAD_ENUM(gl);
    return 1;
}

int seleneopen_selGL(lua_State* L) {
  lua_getglobal(L, "package");
  lua_getfield(L, -1, "preload");
  lua_pushcfunction(L, luaopen_selGL);
  lua_setfield(L, -2, "selGL");
  lua_pop(L, 2);
  return 0;
}