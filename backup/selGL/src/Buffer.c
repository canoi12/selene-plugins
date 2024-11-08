#include "selene.h"
#include "lua_helper.h"
#include "selGL.h"

static MODULE_FUNCTION(Buffer, create) {
    NEW_UDATA(glBuffer, buffer);
    glGenBuffers(1, buffer);
    return 1;
}

static MODULE_FUNCTION(Buffer, bind) {
    INIT_ARG();
    CHECK_INTEGER(target);
    TEST_UDATA(glBuffer, buf);
    if (buf) glBindBuffer(target, *buf);
    else glBindBuffer(target, 0);
    return 0;
}

static MODULE_FUNCTION(Buffer, data) {
    INIT_ARG();
    CHECK_INTEGER(target);
    CHECK_INTEGER(usage);
    CHECK_INTEGER(size);
    if (lua_type(L, arg) == LUA_TLIGHTUSERDATA) {
        glBufferData(target, size, lua_touserdata(L, arg), usage);
    } else {
        glBufferData(target, size, NULL, usage);
    }
    return 0;
}

static MODULE_FUNCTION(Buffer, subData) {
    INIT_ARG();
    CHECK_INTEGER(target);
    CHECK_INTEGER(start);
    CHECK_INTEGER(size);
    CHECK_LUDATA(void, data);
    glBufferSubData(target, start, size, data);
    return 0;
}

// Meta

static META_FUNCTION(Buffer, destroy) {
    CHECK_META(glBuffer);
    glDeleteBuffers(1, self);
    return 0;
}

BEGIN_META(Buffer) {
    BEGIN_REG()
        REG_FIELD(Buffer, create),
        REG_FIELD(Buffer, bind),
        REG_FIELD(Buffer, data),
        REG_FIELD(Buffer, subData),
    END_REG()
    BEGIN_REG(_index)
        REG_META_FIELD(Buffer, destroy),
    END_REG()
    NEW_META(glBuffer, _reg, _index_reg);
    return 1;
}
