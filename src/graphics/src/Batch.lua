--- @class graphics.Batch
--- @field offset integer
--- @field size integer
--- @field vertexSize integer
--- @field buffer integer
--- @field data selene.Data
local Batch = {}
local batch_mt = {
    __index = Batch
}

---Creates a new Batch file
---@param size integer
---@return graphics.Batch
function Batch.create(size, vertexSize)
    local b = {}
    b.offset = 0
    b.vertexSize = vertexSize or 32 -- 8 floats (8 * 4 = 32 bytes)
    b.size = size * b.vertexSize
    b.buffer = gl.gen_buffers(1)
    b.data = selene.create_data(b.size)

    gl.bind_buffer(gl.ARRAY_BUFFER, b.buffer)
    gl.buffer_data(gl.ARRAY_BUFFER, b.size, nil, gl.DYNAMIC_DRAW)
    gl.bind_buffer(gl.ARRAY_BUFFER)

    return setmetatable(b, batch_mt)
end

function Batch:push(x, y, r, g, b, a, u, v)
    if self.offset + self.vertexSize >= self.size then
        self.size = self.size * 2
        self.data = selene.create_data(self.size, self.data)
        gl.bind_buffer(gl.ARRAY_BUFFER, self.buffer)
        gl.buffer_data(gl.ARRAY_BUFFER, self.size, nil, gl.DYNAMIC_DRAW)
        gl.bind_buffer(gl.ARRAY_BUFFER)
    end
    self.data:write_floats(self.offset, x, y, r, g, b, a, u, v)
    self.offset = self.offset + self.vertexSize
end

function Batch:clear()
    self.offset = 0
end

function Batch:flush()
    local offset = self.offset
    if offset <= 0 then return false end
    gl.bind_buffer(gl.ARRAY_BUFFER, self.buffer)
    gl.buffer_subdata(gl.ARRAY_BUFFER, 0, self.offset, self.data.root)
    gl.bind_buffer(gl.ARRAY_BUFFER)
    return true
end

function Batch:getCount()
    return self.offset / self.vertexSize
end

function Batch:getSize()
    return self.size
end

return Batch