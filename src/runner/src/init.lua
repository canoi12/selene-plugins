local runner = {}
local AudioSystem = require('AudioSystem')

function runner.init(title, w, h)
    if not sdl.init(sdl.INIT_VIDEO, sdl.INIT_AUDIO) then
        error('failed to init SDL2')
    end
    runner.window = sdl.create_window(title, sdl.WINDOWPOS_CENTERED, sdl.WINDOWPOS_CENTERED, w, h, sdl.WINDOW_SHOWN)
    runner.renderer = sdl.create_renderer(runner.window)
    runner.event = sdl.create_event()
    runner.audio = AudioSystem.create()
    runner.current = 0
    runner.last = 0
end

--- @param dt number
function runner.update(dt)
end

function runner.render(r)
end

function runner.step()
    runner.audio:update()
    while runner.event:poll() do
        if runner.event:get_type() == sdl.QUIT then selene.set_running(false) end
        if runner.event:get_type() == sdl.WINDOWEVENT then
            if runner.event:window_event() == sdl.WINDOWEVENT_CLOSE then selene.set_running(false) end
        end
    end
    runner.last = runner.current
    runner.current = sdl.get_ticks()
    local delta = (runner.current - runner.last) / 1000
    runner.update(delta)
    runner.renderer:set_color(0, 0, 0, 255)
    runner.renderer:clear()
    runner.renderer:set_color(255, 255, 255, 255)
    runner.render(runner.renderer)
    runner.renderer:present()
end

function runner.quit()
    runner.renderer:destroy()
    runner.window:destroy()
    sdl.quit()
end

return runner