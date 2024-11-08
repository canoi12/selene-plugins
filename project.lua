require('setup')
project('selene-plugins')
    kind("SharedLib")
    filter {"options:plugins=static"}
        kind("StaticLib")
    filter {"options:plugins=shared"}
        defines {"BUILD_PLUGINS_AS_DLL"}
    filter {}
    language "C"
    files {"plugins.c", "plugins.h"}
    links {"lua"}
    targetdir(_BIN_DIR)
    targetname "plugins"
    filter {"system:emscripten"}
        linkoptions {"-sSIDE_MODULE"}
        pic "On"
    filter {}

if _OPTIONS['plugins'] then
    for name,data in pairs(_PLUGINS) do
        local lang = string.lower(data.language)
        if lang == 'c' then
            files {data.src_dir .. '/*.c', data.src_dir .. '/*.h'}
            if data.build_callback then
                data.build_callback()
            end
        elseif lang == 'lua' then
            files {data.dir .. '/' .. name .. '.c'}
        end
        filter {}
    end
end