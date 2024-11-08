print(_WORKING_DIR, _MAIN_SCRIPT_DIR)

if not _PLUGINS_DIR then _PLUGINS_DIR = _MAIN_SCRIPT_DIR end
if _WORKING_DIR ~= _MAIN_SCRIPT_DIR then
    package.path = package.path .. ';' .. _WORKING_DIR .. '/?.lua;' .. _WORKING_DIR .. '/?/init.lua'
end

if _PLUGINS_DIR ~= _MAIN_SCRIPT_DIR then
    package.path = package.path .. ';' .. _PLUGINS_DIR .. '/?.lua;' .. _PLUGINS_DIR .. '/?/init.lua'
end

require('generate_files')
generate_files()
workspace "selene-plugins"
    language "C"
    configurations {"Debug", "Release"}

    filter {"action:vs20*"}
        platforms {"win32", "win64"}
    filter {"action:gmake*", "options:not emscripten"}
        platforms {"linux", "win32", "win64"}
    filter {"action:androidmk"}
        platforms {"android"}
        system "android"
        defines {"__ANDROID__"}
    filter {}

    location(_OUT_DIR)

    includedirs {_WORKING_DIR, _LUA_DIR .. '/src'}
    includedirs {_WORKING_DIR .. '/include'}
    includedirs {_PLUGINS_DIR .. '/third'}

    if COMPILE_WEB then
        platforms {"emscripten"}
        defines {"__EMSCRIPTEN__"}
    end

    build_sdl()

    filter {'options:plugins=shared'}
        defines {"BUILD_PLUGINS_AS_DLL"}

    if _OPTIONS['plugins'] == 'shared' then
        print("Building plugins as DLL")
    end

    filter {"action:gmake*", "platforms:win32 or win64"}
        system "windows"

    filter {'configurations:Debug'}
        symbols "On"
        defines {'DEBUG'}
        
    filter {'configurations:Release'}
        defines {'NDEBUG'}
        optimize "On"

    filter {"system:android"}
        if _ACTION == 'androidmk' then
            ndkabi "x86 x86_64 arm64-v8a"
            ndkplatform "android-26"
        end

    filter {"platforms:linux"}
        system "linux"

    if (COMPILE_WEB) then
        filter {"platforms:emscripten"}
            system "emscripten"
            linkoptions {"-s WASM=1"}
    end
    filter {}

require('lua_proj')
if _OPTIONS['plugins'] then
    require('project')
else
    include(_PLUGINS_DIR .. '/src/plugins.lua')
end

-- if not _OPTIONS['static-lib'] then
-- end

-- require('project')

-- require('plugins_loader')(proj)
