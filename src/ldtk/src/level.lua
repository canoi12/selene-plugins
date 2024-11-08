local Level = {}
Level.__index = Level

function Level.create(prj, data)
    local level = {}

    level.identifier = data.identifier
    level.layers = {}

    return setmetatable(level, Level)
end

return Level