--- @class gl.Texture
--- @field handle integer
--- @field __target integer
--- @field __binding integer
local Texture = {}

--- Destroy the internal GL Texture
function Texture:destroy()
    gl.gen_textures(self.handle)
end

function Texture:bind()
    gl.bind_texture(self.__target, self.handle)
    self.__binding = self.handle
end

function Texture:unbind()
    gl.bind_texture(self.__target)
    self.__binding = 0
end