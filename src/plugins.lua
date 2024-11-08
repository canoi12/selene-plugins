local src_dir = _PLUGINS_DIR .. '/src/'
for name,data in pairs(_PLUGINS) do
    local plugin_src_dir = data.src_dir
    local lang = string.lower(data.language)
    group "plugins"
    project (data.name)
        filter {}
        -- print(data.name)
        if lang == 'c' then
            language "C"
            kind "SharedLib"
            files {plugin_src_dir .. '/*.c', plugin_src_dir .. '/*.h'}
            pic "On"
            if data.build_callback then
                data.build_callback()
            end
            links {"lua"}
            if data.link_callback then
                data.link_callback()
            end
        elseif lang == 'lua' then
            language "C"
            kind "SharedLib"
            pic "On"
            links {"lua"}
            files {src_dir .. name .. '/%{prj.name}.c'}
        end
        filter {}
    group ""
end
