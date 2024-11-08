-- createLuaPlugin('AudioSystem')
-- createCPlugin('audioSystem')
-- filter "system:windows"
--         includedirs {"../third/SDL2/MSVC/include"}

-- filter {"platforms:Win32"}
--         libdirs {"../third/SDL2/MSVC/lib/x86"}
-- filter {"platforms:Win64"}
--         libdirs {"../third/SDL2/MSVC/lib/x64"}

-- filter {"system:not emscripten"}
--         links {"SDL2"}
-- filter {"system:emscripten"}
--         linkoptions {"-sUSE_SDL=2"}
-- filter {}
return {
    name = 'AudioSystem',
    language = 'Lua'
}