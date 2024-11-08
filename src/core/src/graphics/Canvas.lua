--- @type core.Rect
local Rect = require 'core.Rect'
local Drawable = require('core.graphics.Drawable')

--- @class core.graphics.Canvas : core.graphics.Drawable
--- @field handle integer
--- @field channels integer
local Canvas = {}
local canvas_mt = {
    __index = Canvas,
    __gc = function(c)
        gl.delete_framebuffers(c.handle)
        gl.delete_textures(c.texture)
    end
}

function Canvas.create(width, height)
    local canvas = {}

    canvas.width = width
    canvas.height = height
    canvas.channels = 4
    canvas.texture = gl.Texture.create()
    gl.bind_texture(gl.TEXTURE_2D, canvas.texture)
    gl.tex_image2d(gl.TEXTURE_2D, 0, gl.RGBA, width, height, 0, gl.RGBA, gl.UNSIGNED_BYTE)
    gl.tex_parameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST)
    gl.tex_parameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST)
    gl.tex_parameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE)
    gl.tex_parameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE)
    gl.bind_texture(gl.TEXTURE_2D)

    canvas.handle = gl.gen_framebuffers(1)
    gl.bind_framebuffer(gl.FRAMEBUFFER, canvas.handle)
    gl.framebuffer_texture2d(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, canvas.texture)
    gl.bind_framebuffer(gl.FRAMEBUFFER)

    return setmetatable(canvas, canvas_mt)
end

function Canvas.default()
end

Canvas.getTexture = Drawable.getTexture
Canvas.getWidth = Drawable.getWidth
Canvas.getHeight = Drawable.getHeight

function Canvas:getUV(rect)
    local width = self.width
    local height = self.height

    rect = (rect or { x = 0, y = 0, w = width, h = height })

    local uv = {}
    local uv_y = rect.y / height
    uv[1] = rect.x / width
    uv[2] = 1 - uv_y
    uv[3] = uv[1] + rect.w / width
    uv[4] = 1 - (uv_y + rect.h / height)
    return uv
end

return Canvas