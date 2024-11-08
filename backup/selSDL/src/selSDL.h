#ifndef SEL_SDL2_PLUGIN_H_
#define SEL_SDL2_PLUGIN_H_

#include "selene.h"
#include "lua_helper.h"
#include "modules/data_types/data_types.h"

#if defined(__EMSCRIPTEN__)
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_opengles2.h>
    #include <emscripten.h>
#else
    #if defined(OS_WIN)
    #define SDL_MAIN_HANDLED
    #include <windows.h>
    #endif
    #include <SDL.h>
    #include <SDL_opengl.h>
#endif

typedef SDL_Window* sdlWindow;
typedef SDL_GLContext* sdlGLContext;
typedef SDL_Renderer* sdlRenderer;
typedef SDL_Event sdlEvent;
typedef SDL_Texture* sdlTexture;

typedef SDL_Joystick* sdlJoystick;
typedef SDL_GameController* sdlGamepad;

typedef SDL_AudioDeviceID sdlAudioDeviceID;
typedef SDL_AudioStream* sdlAudioStream;

typedef struct {
    int top, count;
    sdlAudioStream* data;
    int* availables;
} AudioStreamPool;

typedef struct {
    FontData* font_data;
    sdlTexture* texture;
} sdlFont;

#endif /* SEL_SDL2_PLUGIN_H_ */