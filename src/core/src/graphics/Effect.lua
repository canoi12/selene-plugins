--- @class core.graphics.Effect
--- @field program selene.gl.Program
--- @field vertShader selene.gl.Shader
--- @field fragShader selene.gl.Shader
--- @field worldLocation integer
--- @field modelViewLocation integer
local Effect = {}

local defaultPosition = [[
vec4 position(vec2 pos, mat4 mvp, mat4 view) {
    return mvp * view * vec4(pos, 0.0, 1.0);
}
]]

local defaultPixel = [[
vec4 pixel(vec4 color, vec2 texcoord, sampler2D tex) {
    return color * texture(tex, texcoord);
}
]]

--- @class core.graphics.ShaderDef
--- @field version string
--- @field attribute string
--- @field varying table
--- @field precision string
local ShaderDef = {}

local OS = os.host()
--- @type core.graphics.ShaderDef
local shaderBase = {}
if OS == "emscripten"  or OS == "android" then
  shaderBase.version = "#version 100"
  shaderBase.attribute = "attribute"
  shaderBase.varying = { "varying", "varying" }
  shaderBase.precision = "#define texture texture2D\nprecision mediump float;"
else
  shaderBase.version = "#version 140"
  shaderBase.attribute = "in"
  shaderBase.varying = { "out", "in" }
  shaderBase.precision = ""
end

local effect_mt = {
  __index = Effect,
  __gc = function(effect)
    gl.delete_program(effect.program)
    gl.delete_shader(effect.vertShader)
    gl.delete_shader(effect.fragShader)
  end
}

function Effect.create(position, pixel)
  position = position or defaultPosition
  pixel = pixel or defaultPixel
  local effect = {}
  local attribs = {'vec2 a_Position', 'vec4 a_Color', 'vec2 a_Texcoord'}
  local varyings = {'vec4 v_Color', 'vec2 v_Texcoord'}

  local vertSource = shaderBase.version
  local fragSource = shaderBase.version .. '\n' .. shaderBase.precision
  for _,a in ipairs(attribs) do
    vertSource = vertSource .. '\n' .. shaderBase.attribute .. " " .. a .. ';'
  end

  for _,v in ipairs(varyings) do
    vertSource = vertSource .. "\n" ..
      shaderBase.varying[1] .. " " .. v .. ";"

    fragSource = fragSource .. "\n" ..
      shaderBase.varying[2] .. " " .. v .. ";"
  end

  vertSource = vertSource .. [[
uniform mat4 u_MVP;
uniform mat4 u_View;
]] .. position .. [[
void main() {
    gl_Position = position(a_Position, u_MVP, u_View);
    v_Color = a_Color;
    v_Texcoord = a_Texcoord;
}
]]
    fragSource = fragSource .. [[
uniform sampler2D u_Texture;
]] .. pixel .. [[
void main() {
    gl_FragColor = pixel(v_Color, v_Texcoord, u_Texture);
}
]]
    local vert = gl.create_shader(gl.VERTEX_SHADER)
    gl.shader_source(vert, vertSource)
    gl.compile_shader(vert)

    local frag = gl.create_shader(gl.FRAGMENT_SHADER)
    gl.shader_source(frag, fragSource)
    gl.compile_shader(frag)

    local prog = gl.create_program()
    gl.attach_shader(prog, vert)
    gl.attach_shader(prog, frag)
    gl.link_program(prog)

    effect.program = prog
    effect.vertShader = vert
    effect.fragShader = frag
    return setmetatable(effect, effect_mt)
end


--- @type {string:function}
local funcs = {
  ["number"] = function(prog, name, ...)
    local location = gl.get_uniform_location(prog.program, name)
    gl.uniform1fv(location, ...)
  end,
  ["table"] = function(prog, name, ...)
    local tables = {...}
    local len = #tables[1]
    local location = gl.get_uniform_location(prog.program, name)
    if len == 2 then
      gl.uniform2fv(location, ...)
    elseif len == 3 then
      gl.uniform3fv(location, ...)
    elseif len == 4 then
      gl.uniform4fv(location, ...)
    else
      error("Invalid vector size")
    end
  end,
  ["userdata"] = function(prog, name, ...)
    local mat = {...}
    local location = gl.get_uniform_location(prog.program, name)
    gl.uniform_matrix4fv(location, 1, false, mat[1])
  end
}

function Effect:send(name, ...)
  local values = {...}
  local t = type(values[1])
  if funcs[t] then
    funcs[t](self, name, ...)
  end
end

return Effect
