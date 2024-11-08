local src_dir = _PLUGINS_DIR .. '/src/'
local plugins = os.matchdirs(src_dir .. '*')
local plugins_names = {}
for i,plugin in ipairs(plugins) do
    -- print(path.getbasename(plugin), path.getname(plugin))
    -- print(path.getdirectory(plugin), path.getdrive(plugin))
    -- plugins_names[i] = string.sub(plugin, #src_dir+1, #plugin)
    local name = path.getbasename(plugin)
    if _PLUGINS_BLOCKLIST and not table.contains(_PLUGINS_BLOCKLIST, name) then
        table.insert(plugins_names, name)
    end
end
if not _PLUGINS then _PLUGINS = {} end
local src_dir = _PLUGINS_DIR .. '/src/'
for i,plugin in ipairs(plugins_names) do
    local p = include(src_dir .. plugin .. '/init.lua')
    p.src_dir = src_dir .. plugin .. '/src'
    p.dir = src_dir .. plugin
    _PLUGINS[plugin] = p
end
