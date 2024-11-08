--- @class gl.Program
--- @field handle integer
local Program = {}
Program.__index = Program

--- @return gl.Program
function Program.create()
    local p = {}
    p.handle = gl.create_program()
    return setmetatable(p, Program)
end

function Program:attach(shader)
    gl.attach_shader(self.handle, shader.handle)
end

function Program:link()
    gl.link_program(self.handle)
    if gl.get_programiv(self.handle, gl.LINK_STATUS) then
        print('Failed to link program')
        print(gl.get_program_info_log(self.handle))
    end
end

return Program