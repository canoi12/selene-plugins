#include "selSDL.h"

extern MODULE_FUNCTION(AudioStream, meta);
extern MODULE_FUNCTION(AudioDeviceID, meta);

extern MODULE_FUNCTION(Window, meta);
extern MODULE_FUNCTION(Renderer, meta);
extern MODULE_FUNCTION(GLContext, meta);

extern MODULE_FUNCTION(sdlTexture, meta);
extern MODULE_FUNCTION(Font, meta);

extern MODULE_FUNCTION(Event, meta);

extern MODULE_FUNCTION(Joystick, meta);
extern MODULE_FUNCTION(Gamepad, meta);

static MODULE_FUNCTION(selSDL, init) {
    int flags = 0;
    int args = lua_gettop(L);
    for (int i = 0; i < args; i++) {
        flags |= luaL_checkinteger(L, i + 1);
    }
    int error = SDL_Init(flags);
    PUSH_BOOLEAN(!error);
    return 1;
}

static MODULE_FUNCTION(selSDL, quit) {
    SDL_Quit();
    return 0;
}

static MODULE_FUNCTION(selSDL, create_window) {
  INIT_ARG();
  CHECK_STRING(title);
  CHECK_INTEGER(x);
  CHECK_INTEGER(y);
  CHECK_INTEGER(width);
  CHECK_INTEGER(height);
  CHECK_INTEGER(flags);
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

/************************
 #                      #
 #          GL          #
 #                      #
 ************************/

static MODULE_FUNCTION(selSDL, glMakeCurrent) {
  INIT_ARG();
  CHECK_UDATA(sdlWindow, win);
  CHECK_UDATA(sdlGLContext, ctx);
  SDL_GL_MakeCurrent(*win, *ctx);
  return 0;
}

static MODULE_FUNCTION(selSDL, glGetProcAddress) {
  PUSH_LUDATA(SDL_GL_GetProcAddress);
  return 1;
}

static MODULE_FUNCTION(selSDL, glSetAttribute) {
  INIT_ARG();
  CHECK_INTEGER(attr);
  CHECK_INTEGER(value);
  SDL_GL_SetAttribute(attr, value);
  return 0;
}

static MODULE_FUNCTION(selSDL, glSetSwapInterval) {
  INIT_ARG();
  GET_BOOLEAN(value);
  int res = SDL_GL_SetSwapInterval(value);
  PUSH_BOOLEAN(res == 0);
  return 1;
}

static MODULE_FUNCTION(selSDL, glGetSwapInterval) {
  int res = SDL_GL_GetSwapInterval();
  PUSH_INTEGER(res);
  return 1;
}

/************************
 #                      #
 #      Clipboard       #
 #                      #
 ************************/

static MODULE_FUNCTION(selSDL, getClipboardText) {
  char *text = SDL_GetClipboardText();
  PUSH_STRING(text);
  SDL_free(text);
  return 1;
}

static MODULE_FUNCTION(selSDL, hasClipboardText) {
  PUSH_BOOLEAN(SDL_HasClipboardText());
  return 1;
}

static MODULE_FUNCTION(selSDL, setClipboardText) {
  INIT_ARG();
  CHECK_STRING(text);
  SDL_SetClipboardText(text);
  return 0;
}

/************************
 #                      #
 #      Filesystem      #
 #                      #
 ************************/

static MODULE_FUNCTION(selSDL, getBasePath) {
  char *path = SDL_GetBasePath();
  PUSH_STRING(path);
  SDL_free(path);
  return 1;
}

static MODULE_FUNCTION(selSDL, getPrefPath) {
  INIT_ARG();
  CHECK_STRING(org);
  CHECK_STRING(app);
  char *path = SDL_GetPrefPath(org, app);
  PUSH_STRING(path);
  SDL_free(path);
  return 1;
}

/************************
 #                      #
 #        Shared        #
 #                      #
 ************************/

static MODULE_FUNCTION(selSDL, loadObject) {
  INIT_ARG();
  CHECK_STRING(sofile);
  void *obj = SDL_LoadObject(sofile);
  PUSH_LUDATA(obj);
  return 1;
}

static MODULE_FUNCTION(selSDL, unloadObject) {
  INIT_ARG();
  CHECK_LUDATA(void, handle);
  SDL_UnloadObject(handle);
  return 0;
}

static MODULE_FUNCTION(selSDL, loadFunction) {
  INIT_ARG();
  GET_LUDATA(void, handle);
  CHECK_STRING(name);
  void *func = SDL_LoadFunction(handle, name);
  PUSH_LUDATA(func);
  return 1;
}

/************************
 #                      #
 #       Keyboard       #
 #                      #
 ************************/

const Uint8 *keys;
static MODULE_FUNCTION(selSDL, checkKeyState) {
  INIT_ARG();
  CHECK_INTEGER(key);
  PUSH_BOOLEAN(keys[key]);
  return 1;
}

static MODULE_FUNCTION(selSDL, getKeyboardState) {
  void* data = (void*)SDL_GetKeyboardState(NULL);
  PUSH_LUDATA(data);
  lua_pushvalue(L, -1);
  lua_rawsetp(L, LUA_REGISTRYINDEX, data);
  return 1;
}

static MODULE_FUNCTION(selSDL, checkKey) {
  INIT_ARG();
  CHECK_LUDATA(Uint8, k);
  CHECK_INTEGER(i);
  lua_pushboolean(L, k[i]);
  return 1;
}

static MODULE_FUNCTION(selSDL, hasScreenKeyboardSupport) {
  PUSH_BOOLEAN(SDL_HasScreenKeyboardSupport());
  return 1;
}

static MODULE_FUNCTION(selSDL, isScreenKeyboardShown) {
  INIT_ARG();
  CHECK_UDATA(sdlWindow, win);
  PUSH_BOOLEAN(SDL_IsScreenKeyboardShown(*win));
  return 1;
}

static MODULE_FUNCTION(selSDL, getScancodeFromName) {
  INIT_ARG();
  CHECK_STRING(name);
  PUSH_INTEGER(SDL_GetScancodeFromName(name));
  return 1;
}

static MODULE_FUNCTION(selSDL, getScancodeName) {
  INIT_ARG();
  CHECK_INTEGER(scancode);
  PUSH_STRING(SDL_GetScancodeName(scancode));
  return 1;
}

static MODULE_FUNCTION(selSDL, getKeyFromName) {
  INIT_ARG();
  CHECK_STRING(name);
  PUSH_INTEGER(SDL_GetKeyFromName(name));
  return 1;
}

static MODULE_FUNCTION(selSDL, getKeyName) {
  INIT_ARG();
  CHECK_INTEGER(keycode);
  PUSH_STRING(SDL_GetKeyName(keycode));
  return 1;
}

static MODULE_FUNCTION(selSDL, startTextInput) {
  SDL_StartTextInput();
  return 0;
}

static MODULE_FUNCTION(selSDL, stopTextInput) {
  SDL_StopTextInput();
  return 0;
}

/************************
 #                      #
 #         Mouse        #
 #                      #
 ************************/

static MODULE_FUNCTION(selSDL, getMousePosition) {
  int x, y;
  SDL_GetMouseState(&x, &y);
  PUSH_NUMBER(x);
  PUSH_NUMBER(y);
  return 2;
}

static MODULE_FUNCTION(selSDL, getRelativeMousePosition) {
  int x, y;
  SDL_GetRelativeMouseState(&x, &y);
  PUSH_NUMBER(x);
  PUSH_NUMBER(y);
  return 2;
}

static MODULE_FUNCTION(selSDL, isMouseDown) {
  INIT_ARG();
  CHECK_INTEGER(button);
  PUSH_BOOLEAN(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(button));
  return 1;
}

/************************
 #                      #
 #        Timer         #
 #                      #
 ************************/

static MODULE_FUNCTION(selSDL, getTicks) {
  PUSH_INTEGER(SDL_GetTicks());
  return 1;
}

static MODULE_FUNCTION(selSDL, delay) {
  Uint32 ms = (Uint32)luaL_checknumber(L, 1);
  SDL_Delay(ms);
  return 0;
}

static MODULE_FUNCTION(selSDL, getPerformanceCounter) {
  PUSH_INTEGER(SDL_GetPerformanceCounter());
  return 1;
}

static MODULE_FUNCTION(selSDL, getPerformanceFrequency) {
  PUSH_INTEGER(SDL_GetPerformanceFrequency());
  return 1;
}

static MODULE_FUNCTION(selSDL, setTime) { return 1; }

/************************
 #                      #
 #        Video         #
 #                      #
 ************************/

static MODULE_FUNCTION(selSDL, enableScreenSaver) { SDL_EnableScreenSaver(); return 0; }

/************************
 #                      #
 #        Error         #
 #                      #
 ************************/

static MODULE_FUNCTION(selSDL, getError) {
  PUSH_STRING(SDL_GetError());
  return 1;
}

static MODULE_FUNCTION(selSDL, setError) {
  INIT_ARG();
  CHECK_STRING(msg);
#if !defined(OS_ANDROID)
  SDL_SetError(msg);
#endif
  return 0;
}

/************************
 #                      #
 #        System        #
 #                      #
 ************************/

static MODULE_FUNCTION(selSDL, getPlatform) {
  PUSH_STRING(SDL_GetPlatform());
  return 1;
}

static MODULE_FUNCTION(selSDL, getCPUCacheLineSize) {
  PUSH_INTEGER(SDL_GetCPUCacheLineSize());
  return 1;
}

static MODULE_FUNCTION(selSDL, getCPUCount) {
  PUSH_INTEGER(SDL_GetCPUCount());
  return 1;
}

static MODULE_FUNCTION(selSDL, getSystemRAM) {
  PUSH_INTEGER(SDL_GetSystemRAM());
  return 1;
}

#include "sdlEnums.h"

THIRD_LIB_API int luaopen_selSDL(lua_State* L) {
    BEGIN_REG()
        REG_FIELD(selSDL, init),
        REG_FIELD(selSDL, quit),
        REG_FIELD(selSDL, glMakeCurrent),
        REG_FIELD(selSDL, glGetProcAddress),
        REG_FIELD(selSDL, glSetAttribute),
        REG_FIELD(selSDL, glSetSwapInterval),
        REG_FIELD(selSDL, glGetSwapInterval),
        /* Clipboard */
        REG_FIELD(selSDL, getClipboardText),
        REG_FIELD(selSDL, setClipboardText),
        REG_FIELD(selSDL, hasClipboardText),
        /* Filesystem  */
        REG_FIELD(selSDL, getBasePath),
        REG_FIELD(selSDL, getPrefPath),
        /* Shared  */
        REG_FIELD(selSDL, loadObject),
        REG_FIELD(selSDL, unloadObject),
        REG_FIELD(selSDL, loadFunction),
        /* Keyboard */
        REG_FIELD(selSDL, getKeyboardState),
        REG_FIELD(selSDL, getScancodeFromName),
        REG_FIELD(selSDL, getScancodeName),
        REG_FIELD(selSDL, getKeyFromName),
        REG_FIELD(selSDL, getKeyName),
        REG_FIELD(selSDL, hasScreenKeyboardSupport),
        REG_FIELD(selSDL, isScreenKeyboardShown),
        REG_FIELD(selSDL, checkKeyState),
        REG_FIELD(selSDL, checkKey),
        REG_FIELD(selSDL, startTextInput),
        REG_FIELD(selSDL, stopTextInput),
        /* Mouse */
        REG_FIELD(selSDL, getMousePosition),
        REG_FIELD(selSDL, getRelativeMousePosition),
        REG_FIELD(selSDL, isMouseDown),
        /* Timer */
        REG_FIELD(selSDL, getTicks),
        REG_FIELD(selSDL, delay),
        REG_FIELD(selSDL, getPerformanceCounter),
        REG_FIELD(selSDL, getPerformanceFrequency),
        /* System */
        REG_FIELD(selSDL, getPlatform),
        REG_FIELD(selSDL, getCPUCount),
        REG_FIELD(selSDL, getCPUCacheLineSize),
        REG_FIELD(selSDL, getSystemRAM),
        /* Video */
        REG_FIELD(selSDL, enableScreenSaver),
        /* Error */
        REG_FIELD(selSDL, getError),
        REG_FIELD(selSDL, setError),
    END_REG()
    luaL_newlib(L, _reg);
    LOAD_META(AudioDeviceID);
    LOAD_META(AudioStream);
    LOAD_META(Font);
    LOAD_META(Window);
    LOAD_META(Renderer);
    LOAD_META(GLContext);
    LOAD_META(Event);
    l_sdlTexture_meta(LUA_STATE_NAME);
    lua_setfield(L, -2, "Texture");
    LOAD_META(Joystick);
    LOAD_META(Gamepad);
    LOAD_ENUM(selSDL)
    return 1;
}

int seleneopen_selSDL(lua_State* L) {
  lua_getglobal(L, "package");
  lua_getfield(L, -1, "preload");
  lua_pushcfunction(L, luaopen_selSDL);
  lua_setfield(L, -2, "selSDL");
  lua_pop(L, 2);
  return 0;
}