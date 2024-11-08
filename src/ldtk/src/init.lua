local ldtk = {}
ldtk.__version = '0.1.0'
ldtk.__index = ldtk

local graphics = require('graphics')
local Image = require('graphics.Image')
local json = require('json')

ldtk.graphics = graphics
ldtk.json = json

local Level = require('ldtk.level')

function ldtk:loadTexture(path)
    return Image.load(self.basedir .. '/' .. path)
end

function ldtk:getTileset(uid)
    for i,tset in ipairs(self.tilesets) do
        if tset.uid == uid then return tset end
    end
    return nil
end

function ldtk.load(path)
    local p = {}
    p.tilesets = {}
    p.levels = {}
    p.basedir = selene.__dir
    local src = sdl.read_file(p.basedir .. '/' .. path)
    p.data = json.decode(src)

    for i,tset in ipairs(p.data.defs.tilesets) do
        local t = {}
        t.identifier = tset.identifier
        t.uid = tset.uid
        t.texture = ldtk.loadTexture(p, tset.relPath)
        t.width = tset.pxWid
        t.height = tset.pxHei
        t.gridSize = tset.tilesetGridSize

        table.insert(p.tilesets, t)
    end

    for i,level in ipairs(p.data.levels) do
        local l = {}
        l.identifier = level.identifier
        l.iid = level.iid
        l.uid = level.uid
        l.width = level.pxWid
        l.height = level.pxHei
        l.layers = {}
        for j,layer in ipairs(level.layerInstances) do
            local la = {}
            la.tileset = ldtk.getTileset(p, layer.__tilesetDefUid)
            la.iid = layer.iid
            la.tiles = layer.autoLayerTiles
            la.type = layer.type

            table.insert(l.layers, la)
        end
        table.insert(p.levels, l)
    end

    return setmetatable(p, ldtk)
end

function ldtk:loadLevel(level)
    self.currentLevel = {}
end

return ldtk