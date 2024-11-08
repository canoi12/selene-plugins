#include "selene.h"
#include "lua_helper.h"
#include "selGL.h"

static MODULE_FUNCTION(Texture, create) {
    NEW_UDATA(glTexture, tex);
    glGenTextures(1, tex);
    return 1;
}

static MODULE_FUNCTION(Texture, bind) {
    INIT_ARG();
    CHECK_INTEGER(target);
    TEST_UDATA(glTexture, tex);
    if (tex) glBindTexture(target, *tex);
    else glBindTexture(target, 0);
    return 0;
}

static MODULE_FUNCTION(Texture, image2D) {
    INIT_ARG();
    CHECK_INTEGER(target);
    CHECK_INTEGER(internal);
    CHECK_INTEGER(width);
    CHECK_INTEGER(height);
    CHECK_INTEGER(format);
    CHECK_INTEGER(type);
    void* data = NULL;
    if (lua_type(L, arg) == LUA_TLIGHTUSERDATA) {
        // ImageData* dt = (ImageData*)lua_touserdata(L, arg);
        data = (void*)lua_touserdata(L, arg);
    }
    glTexImage2D(target, 0, internal, width, height, 0, format, type, data);
    return 0;
}

static MODULE_FUNCTION(Texture, subImage2D) {
    INIT_ARG();
    CHECK_INTEGER(target);
    CHECK_INTEGER(xoffset);
    CHECK_INTEGER(yoffset);
    CHECK_INTEGER(width);
    CHECK_INTEGER(height);
    CHECK_INTEGER(format);
    CHECK_INTEGER(type);
    void* data = NULL;
    if (lua_type(L, arg) == LUA_TLIGHTUSERDATA) {
        // CHECK_UDATA(Data, dt);
        data = (void*)lua_touserdata(L, arg);
    }
    glTexSubImage2D(target, 0, xoffset, yoffset, width, height, format, type, data);
    return 0;
}

static MODULE_FUNCTION(Texture, parameteri) {
    INIT_ARG();
    CHECK_INTEGER(target);
    CHECK_INTEGER(pname);
    CHECK_INTEGER(param);
    glTexParameteri(target, pname, param);
    return 0;
}

// Meta

static META_FUNCTION(Texture, destroy) {
    CHECK_META(glTexture);
    glDeleteTextures(1, self);
    return 0;
}

BEGIN_META(Texture) {
    BEGIN_REG()
        REG_FIELD(Texture, create),
        REG_FIELD(Texture, bind),
        REG_FIELD(Texture, image2D),
        REG_FIELD(Texture, subImage2D),
        REG_FIELD(Texture, parameteri),
    END_REG()
    BEGIN_REG(_index)
        REG_META_FIELD(Texture, destroy),
    END_REG()
    NEW_META(glTexture, _reg, _index_reg);
    return 1;
}
