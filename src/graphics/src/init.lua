local graphics = {}

local Context = require('graphics.Context')
local Image = require('graphics.Image')
local Effect = require('graphics.Effect')

local default_options = {
    title = 'selene',
    width = 640,
    height = 380,
    gl_major = 3,
    gl_minor = 2,
    gl_profile = sdl.GL_CONTEXT_PROFILE_CORE,
    fullscreen = false,
    resizable = false
}

if os.host() == 'android' then
    default_options.gl_major = 2
    default_options.gl_minor = 0
    default_options.gl_profile = sdl.GL_CONTEXT_PROFILE_ES
end

--- @param options table
--- @return graphics.Context
function graphics.create(options)
    local opt = setmetatable(options or {}, { __index = default_options })
    local ctx = {}

    sdl.gl_set_attribute(sdl.GL_CONTEXT_MAJOR_VERSION, opt.gl_major)
    sdl.gl_set_attribute(sdl.GL_CONTEXT_MINOR_VERSION, opt.gl_minor)
    sdl.gl_set_attribute(sdl.GL_CONTEXT_PROFILE_MASK, opt.gl_profile)
    local flags = sdl.WINDOW_SHOWN | sdl.WINDOW_OPENGL
    if opt.fullscreen then
        flags = flags | sdl.WINDOW_FULLSCREEN
    end
    if opt.resizable then
        flags = flags | sdl.WINDOW_RESIZABLE
    end
    ctx.window = sdl.create_window(
        opt.title,
        sdl.WINDOWPOS_CENTERED, sdl.WINDOWPOS_CENTERED,
        opt.width, opt.height,
        flags
    )
    if not ctx.window then
        error("failed to create SDL window: " .. sdl.get_error())
    end
    ctx.gl_ctx = sdl.create_gl_context(ctx.window)
    sdl.gl_make_current(ctx.window, ctx.gl_ctx)
    gl.setup()

    ctx.vao = gl.gen_vertex_arrays(1)
    ctx.vbo = gl.gen_buffers(1)

    ctx.effect = Effect.create()

    gl.bind_vertex_array(ctx.vao)
    gl.bind_buffer(gl.ARRAY_BUFFER, ctx.vbo)
    local attribs = {
        "a_Position",
        "a_Color",
        "a_Texcoord"
    }
    for _,attr in ipairs(attribs) do
        local loc = gl.get_attrib_location(ctx.effect.program, attr)
        gl.enable_vertex_attrib_array(loc)
    end
    gl.vertex_attrib_pointer(0, 2, gl.FLOAT, false, 8 * 4, 0)
    gl.vertex_attrib_pointer(1, 4, gl.FLOAT, false, 8 * 4, 2*4)
    gl.vertex_attrib_pointer(2, 2, gl.FLOAT, false, 8 * 4, 6*4)
    gl.bind_vertex_array()
    gl.bind_buffer(gl.ARRAY_BUFFER)

    ctx.vert_data = selene.create_data(1000 * 4 * 8 * 6)

    return setmetatable(ctx, Context)
end

graphics.load_image = Image.load
graphics.create_effect = Effect.create

return graphics