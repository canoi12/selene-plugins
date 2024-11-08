#include "selene.h"
#include "lua_helper.h"
#include "selGL.h"

static MODULE_FUNCTION(Program, create) {
    NEW_UDATA(glProgram, p);
    *p = glCreateProgram();
    return 1;
}

static MODULE_FUNCTION(Program, use) {
    INIT_ARG();
    TEST_UDATA(glProgram, p);
    if (p) glUseProgram(*p);
    else glUseProgram(0);
    return 0;
}

// Meta

static MODULE_FUNCTION(Program, destroy) {
    CHECK_META(glProgram);
    glDeleteProgram(*self);
    return 0;
}

static MODULE_FUNCTION(Program, attachShader) {
    CHECK_META(glProgram);
    int args = lua_gettop(LUA_STATE_NAME);
    while (arg <= args) {
        CHECK_UDATA(glShader, s);
        glAttachShader(*self, *s);
    }
    return 0;
}

static MODULE_FUNCTION(Program, link) {
    CHECK_META(glProgram);
    glLinkProgram(*self);
    int success = 0;
    glGetProgramiv(*self, GL_LINK_STATUS, &success);
    if (!success) {
        int len;
        glGetProgramiv(*self, GL_INFO_LOG_LENGTH, &len);
    #if defined(OS_WIN)
        if (len >= 2048)
            len = 2048;
        char log[2049];
    #else
        char log[len+1];
    #endif
        glGetProgramInfoLog(*self, len, NULL, log);
        log[len] = '\0';
        return luaL_error(L, "Failed to link program: %s\n", log);
    }
    return 0;
}

static MODULE_FUNCTION(Program, getAttribLocation) {
    CHECK_META(glProgram);
    CHECK_STRING(name);
    PUSH_INTEGER(glGetAttribLocation(*self, name));
    return 1;
}

static MODULE_FUNCTION(Program, getUniformLocation) {
    CHECK_META(glProgram);
    CHECK_STRING(name);
    PUSH_INTEGER(glGetUniformLocation(*self, name));
    return 1;
}

BEGIN_META(Program) {
    BEGIN_REG()
        REG_FIELD(Program, create),
        REG_FIELD(Program, use),
        REG_FIELD(Program, destroy),
        REG_FIELD(Program, attachShader),
        REG_FIELD(Program, link),
        REG_FIELD(Program, getAttribLocation),
        REG_FIELD(Program, getUniformLocation),
    END_REG()
    // NEW_META(glProgram, _reg, _index_reg);
    luaL_newmetatable(L, "glProgram");
    luaL_setfuncs(L, _reg, 0);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    return 1;
}
