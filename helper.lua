function convertLuaToC(plugin, pattern, out_file)
    local files = os.matchfiles(pattern)

    local builder = {
        files = {}
    }

    for i,file in ipairs(files) do
        local basename = path.getbasename(file)
        local directory = path.getdirectory(file)

        local aux = string.sub(pattern, 1, #pattern - 6)
        -- print(aux, string.sub(directory, #aux, #directory))
        local subdirs = string.sub(directory, #aux, #directory)
        local name = basename
        local f = {}
        -- print('a.', subdirs, subdirs[1])
        if string.sub(subdirs, 1, 1) == '/' then
            subdirs = string.sub(subdirs, 2, #subdirs)
        end
        -- print('b.', subdirs)
        f.name = name
        f.modname = plugin .. '.' .. name
        if subdirs ~= '' then
            f.modname = plugin .. '.' .. subdirs:gsub('/', '.')
            if name ~= 'init' then
                f.modname = f.modname .. '.' .. name
            end
        else
            if name == "init" then
                f.modname = plugin
                f.name = plugin
            end
        end
        
        -- print(f.modname)
        f.cname = '_' .. plugin .. '_' .. name
        if subdirs ~= '' then
            f.cname = '_' .. plugin .. '_' .. subdirs:gsub('/', '_') .. '_' .. name
        end
        local content = io.readfile(file)
        f.content = {}
        f.data_size = #content

        local cont = string.explode(content, '\n')
        for _,line in ipairs(cont) do
            local t = {}
            for c=1,#line do
                local b = line:byte(c)
                table.insert(t, b)
                if c == #line and b ~= 13 then
                    table.insert(t, 13)
                end
            end
            table.insert(f.content, t)
        end
        table.insert(builder, f)
    end

    local cfile = [[
#include "selene.h"
    ]]
    -- local cfile = ""

    for i,file in ipairs(builder) do
        cfile = cfile .. "const char " .. file.cname .. "[] = {\n"
        for j,cont in ipairs(file.content) do
            for _,byte in ipairs(cont) do
                cfile = cfile .. string.format('0x%02X', byte) .. ', '
            end
            cfile = cfile .. '\n'
        end

        cfile = cfile .. "0x00\n};\n"
        cfile = cfile .. 'const size_t ' .. file.cname .. '_size = ' .. file.data_size .. ';\n'
        cfile = cfile .. "int _preload" .. file.cname .. "(lua_State* L) {\n"
        cfile = cfile .. "  luaL_dostring(L, " .. file.cname .. ");\n"
        cfile = cfile .. "  return 1;\n}\n"
    end

    cfile = cfile .. "int seleneopen_" .. plugin .. "(lua_State* L) {\n"
    cfile = cfile ..
[[
  lua_getglobal(L, "package");
  lua_getfield(L, -1, "preload");
]]
    for i,file in ipairs(builder) do
        cfile = cfile .. "\tlua_pushcfunction(L, _preload" .. file.cname .. ");\n"
        cfile = cfile .. "\tlua_setfield(L, -2, \"" .. file.modname .. "\");\n"
    end
    cfile = cfile .. "\tlua_pop(L, 2);\n"
    cfile = cfile .. "\treturn 0;\n}\n"
    io.writefile(out_file, cfile)
end

function generatePluginsH(plugins)
    local header = [[
#ifndef SELENE_PLUGINS_H_
#define SELENE_PLUGINS_H_

#include "platforms.h"
#include "selene.h"

#if defined(BUILD_PLUGINS_AS_DLL)
    #if defined(OS_WIN)
        #define SELENE_PLUGINS_API __declspec(dllexport)
    #else
        #define SELENE_PLUGINS_API extern
    #endif
#else
    #define SELENE_PLUGINS_API
#endif


]]
    for plugin,data in pairs(plugins) do
        header = header .. 'SELENE_PLUGINS_API int seleneopen_' .. plugin .. '(lua_State* L);\n'
    end
    
    header = header .. [[

const lua_CFunction plugins_list[] = {
    ]]
    local a = [[
    seleneopen_%s,
    ]]

    
    for plugin,data in pairs(plugins) do
        header = header .. string.format(a, plugin, plugin)
    end
    header = header .. [[
    NULL
};

SELENE_PLUGINS_API int luaopen_plugins(lua_State* L);

#endif /* SELENE_PLUGINS_H_ */
]]
    io.writefile(_PLUGINS_DIR .. "/plugins.h", header)
end