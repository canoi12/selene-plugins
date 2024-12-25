--- @class gl.Buffer
--- @field handle integer
--- @field __target integer
local Buffer = {}
Buffer.__index = Buffer

function Buffer.create()
    local b = {}
    b.handle = gl.gen_buffers(1)
    return setmetatable(b, Buffer)
end

--- Bind the buffer
function Buffer:bind()
    gl.bind_buffer(self.__target, self.handle)
end

--- Unbind the buffer
function Buffer:unbind()
    gl.bind_buffer(self.__target)
end

--- @enum gl.DrawType
local DrawType = {
    'STATIC_DRAW',
    'DYNAMIC_DRAW'
}

--- @param data selene.Data
--- @param draw gl.DrawType
function Buffer:data(data, draw)
    gl.buffer_data(self.__target, data:size(), data:root(), gl[draw])
end

function Buffer:subData(start, data)
    gl.buffer_subdata(self.__target, start, data:size(), data:root())
end

return Buffer