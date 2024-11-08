--- @class core.graphics.Font : core.graphics.Drawable
--- @field size integer
--- @field rects font.Glyph
local Font = {}

local font_mt = {
    __index = Font,
    __gc = function(o)
        gl.delete_textures(o.texture)
    end
}

--- Load font from a TTF file
--- @param path string
--- @param size integer
---@return Font
function Font.load(path, size)
    local f = {}

    local data, width, height, rects = data_types.FontData.load_from_ttf(path, size)

    f.texture = gl.gen_textures(1)
    gl.bind_texture(gl.TEXTURE_2D, f.texture)
    gl.tex_image2d(gl.TEXTURE_2D, 0, gl.RGBA, width, height, 0, gl.RGBA, gl.UNSIGNED_BYTE, data)
    gl.tex_parameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST)
    gl.tex_parameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST)
    gl.tex_parameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE)
    gl.tex_parameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE)
    gl.bind_texture(gl.TEXTURE_2D)
    data:free()

    f.size = size
    f.rects = rects
    f.width = width
    f.height = height

    return setmetatable(f, font_mt)
end

--- Load default bitmap font
---@return Font
function Font.default()
    local f = {}
    local data, width, height, rects = data_type.FontData.create8x8()
    f.texture = gl.Texture.create()
    gl.bind_texture(gl.TEXTURE_2D, f.texture)
    gl.tex_image2d(gl.TEXTURE_2D, 0, gl.RGBA, width, height, 0, gl.RGBA, gl.UNSIGNED_BYTE, data)
    gl.tex_parameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST)
    gl.tex_parameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST)
    gl.tex_parameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE)
    gl.tex_parameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE)
    gl.bind_texture(gl.TEXTURE_2D)
    data:free()

    f.size = 8
    f.rects = rects
    f.width = width
    f.height = height

    return setmetatable(f, font_mt)
end

function Font:getTexture() return self.texture end
function Font:getHeight() return self.height end

return Font