--- @class core.audio.Music
--- @field playing boolean
--- @field looping boolean
--- @field chunk selene.Data
--- @field decoder selene.AudioDecoder
--- @field stream sdl.AudioStream
local Music = {}

local music_mt = {}
music_mt.__index = Music
music_mt.__gc = function(s)
    s.chunk:free()
    s.decoder:close()
    s.stream:free()
end

--- Create a music from a decoder
--- @param sys core.audio.AudioSystem
--- @param decoder selene.AudioDecoder
--- @return core.audio.Music
function Music.create(sys, decoder)
    local m = {}

    m.playing = false
    m.looping = false
    m.decoder = decoder
    local spec = {
        format = sdl.AUDIO_S16SYS,
        channels = m.decoder:getChannels(),
        sampleRate = m.decoder:getSampleRate()
    }
    m.stream = sdl.AudioStream.create(spec, sys.spec)
    m.chunk = selene.Data.create(sys.spec.size)
    return setmetatable(m, music_mt)
end

--- Load a music from a file
--- @param sys core.audio.AudioSystem
--- @param path string
--- @return core.audio.Music
function Music.load(sys, path)
    local decoder = selene.audio.Decoder.load(path)
    return Music.create(sys, decoder)
end

return Music
