--- @class graphics.Context
--- @field window userdata
--- @field gl_ctx userdata
--- @field color integer[]
--- @field vao integer
--- @field vbo integer
--- @field vert_data selene.Data
--- @field effect graphics.Effect
local Context = {}
Context.__index = Context

function Context:draw_point(x, y)
end

function Context:draw_line(x0, y0, x1, y1)
end

function Context:draw_circle(x, y, r)
end

function Context:draw_rect(x, y, w, h)
    local x0 = x
    local y0 = y
    local x1 = x + w
    local y1 = y + h
    local r, g, b, a = table.unpack(color)
    local vertices = {
        x0, y0, r, g, b, a, 0, 0,
        x1, y0, r, g, b, a, 1, 0,
        x1, y1, r, g, b, a, 1, 1,

        x0, y0, r, g, b, a, 0, 0,
        x1, y1, r, g, b, a, 1, 1,
        x0, y1, r, g, b, a, 0, 1,
    }
    self.vert_data:writeFloats(0,
        table.unpack(vertices)
    )
end

function Context:destroy()
    if self.vao then gl.delete_vertex_arrays(self.vao) end
    gl.delete_buffers(self.vbo)
    self.effect:destroy()
    self.window:destroy()
    self.gl_ctx:destroy()
end

function Context:swap()
    self.window:gl_swap()
end

return Context