--- @class gl.VertexArray
--- @field handle integer
local VertexArray = {}
VertexArray.__index = VertexArray

function VertexArray.create()
    local vao = {}
    vao.handle = gl.gen_vertex_arrays(1)
    return setmetatable(vao, VertexArray)
end

function VertexArray:bind()
    gl.bind_vertex_array(self.handle)
end

function VertexArray:unbind()
    gl.bind_vertex_array()
end

return VertexArray