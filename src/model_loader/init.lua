-- createCPlugin('model_loader')
--     includedirs {"../third/tinyobjloader-c/", "../third/cgltf"}
--     filter {}

-- project "selene"
--     links {"model_loader"}
return {
    name = 'model_loader',
    language = 'C',
    build_callback = function()
        includedirs {_PLUGINS_DIR .. '/third/tinyobjloader-c/', _PLUGINS_DIR .. '/third/cgltf'}
    end,
}