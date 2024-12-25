--- @class Music
--- @field playing boolean
--- @field looping boolean
--- @field chunk selene.Data
--- @field decoder selene.audio.Decoder
--- @field stream selene.sdl2.AudioStream
local Music = {}

local music_mt = {}
music_mt.__index = Music
music_mt.__gc = function(s)
    s.chunk:free()
    s.decoder:close()
    s.stream:free()
end

function Music.create(sys, decoder)
    local m = {}

    m.playing = false
    m.looping = false
    m.decoder = decoder
    local spec = {
        format = sdl.AUDIO_S16SYS,
        channels = m.decoder:get_channels(),
        sampleRate = m.decoder:get_sample_rate()
    }
    m.stream = sdl.new_audio_stream(spec, sys.spec)
    m.chunk = selene.create_data(sys.spec.size)
    return setmetatable(m, music_mt)
end

function Music.load(sys, path)
    local decoder = audio.load_decoder(path)
    return Music.create(sys, decoder)
end

return Music
