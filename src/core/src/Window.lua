--- @class Window
--- @field handle selene.sdl2.Window
--- @field width integer
--- @field height integer
--- @field fullscreen boolean
--- @field borderless boolean
local Window = {}

--- Create a new window with settings
--- @param settings Settings
--- @return Window | nil
function Window.create(settings)
    --- @type Window
    local window = {}
    sdl.gl_set_attribute(sdl.GL_DOUBLEBUFFER, 1)
    sdl.gl_set_attribute(sdl.GL_DEPTH_SIZE, 24)
    sdl.gl_set_attribute(sdl.GL_STENCIL_SIZE, 8)
    if os.host() == "emscripten" or os.host() == 'android' then
        sdl.gl_set_attribute(sdl.GL_CONTEXT_PROFILE_MASK, sdl.GL_CONTEXT_PROFILE_ES)
        sdl.gl_set_attribute(sdl.GL_CONTEXT_MAJOR_VERSION, 2);
        sdl.gl_set_attribute(sdl.GL_CONTEXT_MINOR_VERSION, 0);
    else
        sdl.gl_set_attribute(sdl.GL_CONTEXT_PROFILE_MASK, sdl.GL_CONTEXT_PROFILE_CORE)
        sdl.gl_set_attribute(sdl.GL_CONTEXT_MAJOR_VERSION, 3);
        sdl.gl_set_attribute(sdl.GL_CONTEXT_MINOR_VERSION, 3);
    end

    local flags = sdl.WINDOW_SHOWN | sdl.WINDOW_OPENGL

    if settings.window.resizable then
        flags = flags | sdl.WINDOW_RESIZABLE
    end
    if settings.window.borderless then
        flags = flags | sdl.WINDOW_BORDERLESS
    end
    if settings.window.fullscreen then
        flags = flags | sdl.WINDOW_FULLSCREEN_DESKTOP
    end
    if settings.window.alwaysOnTop then
        flags = flags | sdl.WINDOW_ALWAYS_ON_TOP
    end

    window.fullscreen = settings.window.fullscreen
    window.borderless = settings.window.borderless
    window.width = settings.window.width
    window.height = settings.window.height

    window.handle = sdl.create_window (
        settings.window.title,
        sdl.WINDOWPOS_CENTERED, sdl.WINDOWPOS_CENTERED,
        settings.window.width, settings.window.height,
        flags
    )

    if window.handle == nil then return nil end

    return setmetatable(window, { __index = Window })
end

function Window:destroy()
    self.handle:destroy()
end

--- @return integer, integer
function Window:getSize()
    return self.handle:get_size()
end

function Window:swap()
    self.handle:gl_swap()
end

return Window