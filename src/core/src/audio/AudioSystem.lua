local SoundInstance = require('core.audio.SoundInstance')
--- @class core.audio.AudioSystem
--- @field music core.audio.Music | nil
--- @field sounds table
--- @field device sdlAudioDeviceID
--- @field spec AudioSpec
--- @field pool table
--- @field auxData selene.Data
local AudioSystem = {}

local system_mt = {}
system_mt.__index = AudioSystem

--- Creates a new audio system
--- @return core.audio.AudioSystem
function AudioSystem.create(settings)
    local num = sdl.get_num_audio_devices(false)
    if num <= 0 then
        error('No audio device found')
    end
    local audio = {}
    audio.device, audio.spec = sdl.open_audio_device(nil, false, settings.audio)
    if not audio.device then
        local name = sdl.get_audio_device_name(0, false)
        audio.device, audio.spec = sdl.open_audio_device(name, false, settings.audio)
        if not audio.device then
            error('Failed to open audio device: ' .. sdl.get_error())
        end
    end
    audio.music = nil
    audio.sounds = {}

    audio.auxData = data_types.Data.create(audio.spec.size)
    audio.pool = {}
    for _=1,64 do
        table.insert(
            audio.pool,
            sdl.new_audio_stream(audio.spec, audio.spec)
        )
    end
    audio.device:pause(false)
    return setmetatable(audio, system_mt)
end

--- Destroy audio system
function AudioSystem:destroy()
    if self.device then
        self.device:pause(false)
        self.device:close()
    end
end

function AudioSystem:update()
    if self.music and self.music.playing then
        --- @type Music
        local music = self.music
        local read = music.decoder:get_chunk(self.auxData, self.spec.samples)
        if read < 0 then
            error('Error decoding music')
        elseif read == 0 then
            local wait = music.stream:available()
            if wait == 0 then
                if music.looping then
                    music.decoder:seek(0)
                else
                    music.stream:unbind(self.device)
                    self.music = nil
                end
            end
        else
            local res = music.stream:put(self.auxData:get_pointer(), read * 4)
            if res < 0 then
                error('Stream put error: ' .. sdl.get_error())
            end
        end
    end

    local sounds_to_remove = {}
    for i,sound in ipairs(self.sounds) do
        if sound.playing then
            local stream = sound.stream
            local size = sound.size
            local len = self.spec.size
            if sound.offset + len > size then
                len = size - sound.offset
            end
            local res = 0
            if len ~= 0 then
                res = stream:put(sound.data:get_pointer(sound.offset), len)
                sound.offset = sound.offset + len
            end
            if res < 0 then
                error('Stream put error: ' .. sdl.get_error())
            elseif stream:available() == 0 then
                if sound.loop then sound.offset = 0
                else table.insert(sounds_to_remove, i) end
            end
        end
    end

    for _,id in ipairs(sounds_to_remove) do
        local sound = table.remove(self.sounds, id)
        sound.stream:unbind(self.device)
        table.insert(self.pool, sound.stream)
    end
end

--- Play music
--- @param music core.audio.Music
function AudioSystem:playMusic(music)
    if self.music then
        self.music.stream:unbind(self.device)
    end
    if music then music.playing = true end
    self.music = music
    self.music.stream:bind(self.device)
end

function AudioSystem:stopMusic()
    if self.music then
        self.music.stream:unbind(self.device)
        self.music = nil
    end
end

function AudioSystem:playSound(sound)
    local instance = SoundInstance.create(table.remove(self.pool), sound.data)
    instance.stream:bind(self.device)
    table.insert(self.sounds, instance)
    return instance
end

--- Stop a sound instance
--- @param instance core.audio.SoundInstance
function AudioSystem:stopSound(instance)
    for i=#self.sounds,1,-1 do
        if self.sounds[i] == instance then
            table.remove(self.sounds, i)
            break
        end
    end
    instance.stream:unbind(self.device)
    table.insert(self.pool, instance.stream)
end

return AudioSystem
