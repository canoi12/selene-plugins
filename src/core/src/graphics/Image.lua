--- @type core.Rect
local Rect = require('core.Rect')
local Drawable = require('core.graphics.Drawable')

--- @class core.graphics.Image : core.graphics.Drawable
--- @field channels integer
local Image = {}
local image_mt = {}
image_mt.__index = Image
image_mt.__gc = function(s)
    gl.gen_textures(s.texture)
end

function Image.create(width, height, channels, data)
    local img = {}
    local texture = gl.gen_textures(1)
    channels = channels or 4
    assert(channels > 0 and channels < 5)
    img.texture = texture
    img.width = width
    img.height = height
    img.channels = channels

    gl.bind_texture(gl.TEXTURE_2D, texture)
    gl.tex_image2d(gl.TEXTURE_2D, 0, gl.RGBA, width, height, 0, gl.RGBA, gl.UNSIGNED_BYTE, data)
    gl.tex_parameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST)
    gl.tex_parameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST)
    gl.tex_parameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE)
    gl.tex_parameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE)
    gl.bind_texture(gl.TEXTURE_2D)

    return setmetatable(img, image_mt)
end

function Image.load(path)
    local img = {}
    local data, w, h, c = data_types.ImageData.load(path)
    local img = Image.create(w, h, c, data)
    data:free()
    return img
end

Image.getTexture = Drawable.getTexture
Image.getWidth = Drawable.getWidth
Image.getHeight = Drawable.getHeight

function Image:getUV(rect)
    local width = self.width
    local height = self.height

    self.

    rect = (rect or { x = 0, y = 0, w = width, h = height })

    local uv = {}
    local uv_y = rect.y / height
    uv[1] = rect.x / width
    uv[2] = uv_y
    uv[3] = uv[1] + rect.w / width
    uv[4] = uv_y + rect.h / height
    return uv
end

return Image
