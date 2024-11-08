--- @class gl.Shader
--- @field handle integer
local Shader = {}
Shader.__index = Shader

--- @return gl.Shader
function Shader.create(shader_type)
    local p = {}
    local tp = 0
    if shader_type == 'fragment' then
        tp = gl.FRAGMENT_SHADER
    elseif shader_type == 'vertex' then
        tp = gl.VERTEX_SHADER
    else
        error("Invalid shader type")
    end
    p._type = shader_type
    p.handle = gl.create_shader(tp)
    return setmetatable(p, Shader)
end

function Shader:source(src)
    gl.shader_source(self.handle, src)
end

function Shader:link()
    gl.compile_shader(self.handle)
    if gl.get_shaderiv(self.handle, gl.COMPILE_STATUS) then
        print('Failed to compile ' .. self._type .. ' shader')
        print(gl.get_shader_info_log(self.handle))
    end
end

return Shader