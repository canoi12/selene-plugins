local Image = {}
Image.__index = Image

function Image.load(path)
    local t = {}
    t.data = image.from_file(path)
    t.handle = gl.gen_textures(1)
    t.width = t.data.width
    t.height = t.data.height

    gl.bind_texture(gl.TEXTURE_2D, t.handle)
    gl.tex_image2d(gl.TEXTURE_2D, 0, gl.RGBA, t.width, t.height, 0, gl.RGBA, gl.UNSIGNED_BYTE, t.data.pixels)
    gl.tex_parameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST)
    gl.tex_parameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE)
    gl.tex_parameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE)
    gl.bind_texture(gl.TEXTURE_2D)

    return setmetatable(t, Image)
end

function Image:destroy()
    gl.delete_textures(self.handle)
end

return Image
