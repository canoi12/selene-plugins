local Buffer = require('gl_helper.Buffer')
--- @class gl.ArrayBuffer : gl.Buffer
local ArrayBuffer = {}
ArrayBuffer.__index = ArrayBuffer
ArrayBuffer.__target = gl.ARRAY_BUFFER

setmetatable(ArrayBuffer, Buffer)
function ArrayBuffer.create()
    local b = {}
    b.handle = gl.gen_buffers(1)
    return setmetatable(b, ArrayBuffer)
end
-- ArrayBuffer.bind = Buffer.bind
-- ArrayBuffer.unbind = Buffer.unbind
-- ArrayBuffer.data = Buffer.data
-- ArrayBuffer.subData = Buffer.subData

return ArrayBuffer