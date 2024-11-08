function link_sdl2()
    if _OPTIONS['sdl2-dir'] then
        local sdl_dir = _OPTIONS['sdl2-dir']
        -- print(sdl_dir)
        includedirs {sdl_dir .. '/include'}
        filter {"platforms:Win32"}
            libdirs {sdl_dir .. "/lib/x86"}
        filter {"platforms:Win64"}
            libdirs {sdl_dir .. "/lib/x64"}

        filter {"system:android"}
if COMPILE_ANDROID then
            amk_includes {sdl_dir .. '/Android.mk'}
            amk_sharedlinks {"SDL2"}
end
    else
        filter {"system:not emscripten", "action:gmake2"}
            buildoptions {"`sdl2-config --cflags`"}
            linkoptions {"`sdl2-config --libs`"}
    end
    filter {"system:emscripten"}
        linkoptions {"-sUSE_SDL=2"}
    filter {"system:not emscripten", "system:not android"}
        links {"SDL2"}
    filter {}
end
return {
    name = 'selSDL',
    language = 'C',
    plugin_callback = link_sdl2,
    main_callback = link_sdl2
}