require('helper')
require('setup')

function generate_files()
    local src_dir = _PLUGINS_DIR .. '/src/'
    for plugin,data in pairs(_PLUGINS) do
        local plugin_src_dir = src_dir .. plugin .. '/src'
        local lang = string.lower(data.language)
        if lang == 'lua' then
            print('Generating ' .. data.dir .. '/' .. plugin .. '.c')
            convertLuaToC(plugin, plugin_src_dir .. '/**.lua', src_dir .. plugin .. '/' .. plugin .. '.c')
        end
    end
    print('Generating ' .. _PLUGINS_DIR .. '/plugins.h')
    generatePluginsH(_PLUGINS)
end

newaction {
    trigger = 'generate',
    description = 'Generate the C files',
    execute = generate_files
}

