newoption {
    trigger = 'selene-dir',
    value = 'PATH',
    description = 'Set the selene folder path'
}

newoption {
    trigger = 'lua-version',
    value = 'VERSION',
    description = 'Set used Lua version',
    default = '5.4',
    allowed = {
        {'5.2', 'Lua 5.2'},
        {'5.3', 'Lua 5.3'},
        {'5.4', 'Lua 5.4'},
        {'jit', 'LuaJIT'},
    }
}

newoption {
    trigger = 'no-sdl',
    description = 'Disable the SDL2 compilation from the code'
}

if not _OPTIONS['selene-dir'] then
    error("A selene directory path must be given")
end

require('options')

local lua_dir = _OPTIONS['selene-dir'] .. '/lua/lua' .. _OPTIONS['lua-version']
selene_dir = _OPTIONS['selene-dir']
out_dir = _OPTIONS['selene-dir'] .. '/build/plugins'
bin_dir = selene_dir .. '/build/bin/%{cfg.platform}/%{cfg.buildcfg}'
base_dir = _MAIN_SCRIPT_DIR

_BASE_DIR = _MAIN_SCRIPT_DIR
_PLUGINS_DIR = _BASE_DIR
_SELENE_DIR = _BASE_DIR .. '/' .. selene_dir

newaction {
    trigger = 'clean',
    description = "clean the build folder",
    execute     = function ()
        print("clean the build...")
        os.rmdir(out_dir)
        print("done.")
    end
}

plugins_blocklist = {'selGL', 'selSDL'}

workspace "selene-plugins"
    configurations {"Debug", "Release"}
    platforms {"linux", "win32", "win64"}
    location(out_dir)
    includedirs {selene_dir, selene_dir .. '/include', lua_dir .. "/src"}
    language "C"

    if COMPILE_WEB then
        platforms {"web"}
    end

    filter {"options:no-sdl"}
        defines {"SELENE_NO_SDL"}

    filter {'configurations:Debug'}
        symbols "On"
        defines {'DEBUG'}
        
    filter {'configurations:Release'}
        defines {'NDEBUG'}
        optimize "On"

    filter {"platforms:Win32 or Win64"}
        system "windows"

    filter {"platforms:linux"}
        system "linux"

    if (COMPILE_WEB) then
        filter {"platforms:web"}
            system "emscripten"
            linkoptions {"-s WASM=1"}
    end

    filter {}

project ("lua")
    language "C"
    files {lua_dir .. "/src/*.c", lua_dir .. "/src/*.h"}
    removefiles {lua_dir .. "/src/lua.*", lua_dir .. "/src/luac.*"}
    kind "SharedLib"

    filter "system:linux"
        defines {"LUA_USE_LINUX"}
    filter {"system:windows"}
        defines {"LUA_BUILD_AS_DLL"}
    filter {"system:emscripten"}
        linkoptions {"-sSIDE_MODULE"}
        pic "On"
    filter {}


local proj = project('selene-plugins')
    kind("SharedLib")
    language "C"
    files {"plugins.c", "plugins.h"}
    links {"lua"}
    targetname "plugins"
    targetdir(bin_dir)
    includedirs {"third/", './'}
    filter {"system:not windows"}
        targetprefix ""
    filter {"system:emscripten"}
        linkoptions {"-sSIDE_MODULE"}
        pic "On"
    filter {}

require('plugins_loader')(proj)