#ifndef DEFAULT_SHADERS_H_
#define DEFAULT_SHADERS_H_

const char* vert2d_source =
"#version 100\n"
"attribute vec3 a_Position;\n"
"attribute vec4 a_Color;\n"
"attribute vec2 a_Texcoord;\n"
"uniform mat4 u_Perspective;\n"
"uniform mat4 u_View;\n"
"uniform mat4 u_Model;\n"
"varying vec4 v_Color;\n"
"varying vec2 v_Texcoord;\n"
"void main() {\n"
"   gl_Position = u_World * u_View * u_Model * vec4(a_Position, 1.0);\n"
"   v_Color = a_Color;\n"
"   v_Texcoord = a_Texcoord;\n"
"}";

const char* frag2d_source =
"#version 100\n"
"precision mediump float;\n"
"varying vec4 v_Color;\n"
"varying vec2 v_Texcoord;\n"
"uniform sampler2D u_Texture;\n"
"void main() {\n"
"   gl_FragColor = texture2D(u_Texture, v_Texcoord) * v_Color;\n"
"}";

const char* vert3d_source =
"#version 100\n"
"attribute vec3 a_Position;\n"
"attribute vec3 a_Normal;\n"
"attribute vec2 a_Texcoord;\n"
"uniform mat4 u_Perspective;\n"
"uniform mat4 u_View;\n"
"uniform mat4 u_Model;\n"
"varying vec2 v_Texcoord;\n"
"void main() {\n"
"   gl_Position = u_World * u_View * u_Model * vec4(a_Position, 1.0);\n"
"   v_Texcoord = a_Texcoord;\n"
"}";

const char* frag3d_source =
"#version 100\n"
"precision mediump float;\n"
"varying vec2 v_Texcoord;\n"
"uniform sampler2D u_Texture;\n"
"void main() {\n"
"   gl_FragColor = texture2D(u_Texture, v_Texcoord);\n"
"}";

#endif /* DEFAULT_SHADERS_H_ */