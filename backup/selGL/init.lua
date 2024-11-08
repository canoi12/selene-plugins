-- createCPlugin('selGL')
--     includedirs {"../third/glad/include", "../third/glad/"}
--     files {"../third/glad/src/glad.c"}
--     filter {"system:windows"}
--         links {"opengl32"}
--     filter {"system:not windows"}
--         links {"GL"}
--     filter {}

-- project "selene"
--     links {"selGL"}
--     filter {"system:windows"}
--         links {"opengl32"}
--     filter {"system:not windows"}
--         links {"GL"}
--     filter {}
local function link_opengl()
    filter {'system:windows'}
        links {'opengl32'}
    filter {'system:not windows'}
        links {"GL"}
    filter {}
end
return {
    name = 'selGL',
    language = 'C',
    includedirs = {base_dir .. '/third/glad/include', base_dir .. '/third/glad/'},
    files = {base_dir .. '/third/glad/src/glad.c'},
    plugin_callback = link_opengl,
    main_callback = link_opengl
}