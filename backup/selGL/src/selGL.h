#ifndef SELENE_GL_H_
#define SELENE_GL_H_

#if defined(__EMSCRIPTEN__)
    #include <GLES2/gl2.h>
    #include <emscripten.h>
#else
    #include "glad/glad.h"
#endif

#include "modules/data_types/data_types.h"

enum {
    SELENE_VERTEX_BUFFER = 0,
    SELENE_INDEX_BUFFER
};

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct glRenderer glRenderer;

typedef struct glAttrib glAttrib;
typedef struct glVertexFormat glVertexFormat;

typedef unsigned int glTexture;
typedef unsigned int glFramebuffer;
typedef unsigned int glRenderbuffer;

typedef unsigned int glProgram;
typedef unsigned int glShader;

typedef unsigned int glBuffer;
typedef unsigned int glVertexArray;

struct glAttrib {
    int location;
    int size;
    int type;
    int stride;
    int offset;
};

struct glVertexFormat {
    int count;
    glAttrib* attribs;
};

struct glRenderer {
    glTexture* white_texture;

    glProgram* current_program;
    glTexture* current_texture;
    glFramebuffer* current_framebuffer;

    glVertexArray* current_vao;
    glBuffer* current_buffers[4];
};

glShader create_and_compile_shader(int type, const char* source);
glProgram create_and_link_program(int count, glShader* shaders);

#if defined(__cplusplus)
}
#endif

#endif /* SELENE_GL_H_ */