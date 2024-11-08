local Texture = require('gl_helper.Texture')
--- @class gl.Texture2D : gl.Texture
--- @field handle integer
--- @field width integer
--- @field height integer
local Texture2D = {}
Texture2D.__target = gl.TEXTURE_2D
Texture2D.__index = Texture2D
Texture2D.__binding = 0

--- @param image_data selene.ImageData|nil
--- @return gl.Texture2D
function Texture2D.create(image_data)
    local t = {}
    t.handle = gl.gen_textures(1)
    if image_data then
        t.comp = image_data.comp
        t.width = image_data.width
        t.height = image_data.height
        local target = gl.TEXTURE_2D
        local comp = gl.RGBA
        if t.comp == 4 then comp = gl.RGB end
        gl.bind_texture(target, t.handle)
        gl.tex_image2d(target, 0, comp, t.width, t.height, 0, comp, gl.UNSIGNED_BYTE, image_data.pixels)
        gl.tex_parameteri(target, gl.TEXTURE_MIN_FILTER, gl.NEAREST)
        gl.tex_parameteri(target, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE)
        gl.tex_parameteri(target, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE)
        gl.bind_texture(target)
    end
    return setmetatable(t, Texture2D)
end

Texture2D.bind = Texture.bind
Texture2D.unbind = Texture.unbind


--- @enum gl.PixelFormat
local PixelFormat = {
    'RED',
    'RG',
    'RGB',
    'RGBA',
    'BGR',
    'BGRA',
}

--- @param comp gl.PixelFormat
--- @param width integer
--- @param height integer
--- @param data selene.Data|nil
function Texture2D:texImage2D(comp, width, height, data)
    gl.tex_image2d(self.__target, 0, gl[comp], width, height, 0, gl[comp], gl.UNSIGNED_BYTE, data)
end


return Texture2D