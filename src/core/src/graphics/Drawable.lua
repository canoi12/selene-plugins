--- @class core.graphics.Drawable
--- @field texture integer | nil
--- @field width integer
--- @field height integer
local Drawable = {}

--- @return integer
function Drawable:getTexture() return self.texture end
--- @param rect core.Rect
--- @return table
function Drawable:getUV(rect) end
--- @return integer
function Drawable:getWidth() return self.width end
--- @return integer
function Drawable:getHeight() return self.height end

return Drawable