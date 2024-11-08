--- @type selene.gl
local gl = selene.gl
local sdl = selene.sdl2
local Mat4 = require 'core.math.Mat4'
local Color = require 'core.graphics.Color'
local Window = require 'core.Window'

local Rect = require('core.Rect')

local Drawable = require('core.graphics.Drawable')

local Batch = require 'core.graphics.Batch'
local Canvas = require 'graphics.Canvas'
local Effect = require 'core.graphics.Effect'
local Font = require 'core.graphics.Font'
local Image = require 'core.graphics.Image'

--- @class RenderState
--- @field vao gl.VertexArray
--- @field drawMode string
--- @field drawColor Color
--- @field clearColor Color
--- @field canvas Canvas | nil
--- @field font Font
--- @field texture selene.gl.Texture
--- @field framebuffer selene.gl.Framebuffer
--- @field program selene.gl.Program
--- @field clipRect Rect | nil
--- @field projection selene.linmath.Mat4
--- @field modelview selene.linmath.Mat4
local RenderState = {}

--- @class RendererBackup
--- @field state RenderState
--- @field glContext sdl.GLContext
--- @field vao gl.VertexArray
--- @field batch Batch
--- @field whiteImage Image
--- @field defaultCanvas Canvas
--- @field defaultEffect Effect
--- @field defaultFont Font
--- @field window Window
local Renderer = {}

local size = {0, 0}

---Creates a new Renderer
---@param app App
---@return Renderer
function Renderer.create(app)
    local win = app.window
    local render = {}
    render.glContext = sdl.GLContext.create(win.handle)
    sdl.glMakeCurrent(win.handle, render.glContext)
    gl.loadGlad(sdl.glGetProcAddress())

    render.window = win
    --- @type RenderState
    render.state = {}

    render.state.drawMode = "triangles"
    render.state.drawColor = Color.white
    local vao = gl.VertexArray.create()
    local batch = Batch.create(1000)

    render.vao = vao
    render.batch = batch

    vao:bind()
    gl.Buffer.bind(gl.ARRAY_BUFFER, batch.buffer)

    gl.VertexArray.enable(0)
    gl.VertexArray.enable(1)
    gl.VertexArray.enable(2)

    local effect = Effect.create()
    local p = effect.program
    gl.VertexArray.attribPointer(p:getAttribLocation("a_Position"), 2, gl.FLOAT, false, 32, 0)
    gl.VertexArray.attribPointer(p:getAttribLocation("a_Color"), 4, gl.FLOAT, false, 32, 8)
    gl.VertexArray.attribPointer(p:getAttribLocation("a_Texcoord"), 2, gl.FLOAT, false, 32, 24)
    
    gl.VertexArray.unbind()
    gl.Buffer.bind(gl.ARRAY_BUFFER)

    local imageData = selene.Data.create(4)
    imageData:writeBytes(0, 255, 255, 255, 255)
    local white = Image.create(1, 1, 4, imageData)
    imageData:free()

    local width, height = win:getSize()
    local canvas = setmetatable({}, { __index = Canvas })
    canvas.width = width
    canvas.height = height

    local font = Font.default()

    render.whiteImage = white
    render.defaultCanvas = canvas
    render.defaultEffect = effect
    render.defaultFont = font

    render.state.projection = Mat4.create()
    render.state.projection:ortho(0, width, height, 0, -1, 1)

    local matrixStack = {}

    for _=1,255 do
        table.insert(matrixStack, Mat4.create())
    end
    render.state.matrixStack = matrixStack

    render.state.modelview = Mat4.create()
    render.state.modelview:identity()

    size[1] = width
    size[2] = height

    render.drawCalls = {}

    sdl.glSetSwapInterval(true)
    return setmetatable(render, {
        __index = Renderer
    })
end

function Renderer:destroy()
    self.glContext:destroy()
end

function Renderer:newImage(width, height, channels, data)
    return Image.create(width, height, channels, data)
end

function Renderer:loadTexture(path)
    Image.load()
end

function Renderer:newCanvas(width, height)
end

function Renderer:loadEffect(path)
end

function Renderer:newEffect(position, pixel)
end

function Renderer:newCamera(width, height)
end

function Renderer:clearColor(color)
    local cc = {0, 0, 0, 1}
    for i,c in ipairs(color) do
        cc[i] = c / 255
    end
    gl.clearColor(table.unpack(cc))
end

local drawModes = {
    points = gl.POINTS,
    lines = gl.LINES,
    triangles = gl.TRIANGLES
}

local function setFramebuffer(r, t)
    if t ~= r.state.texture then
        r:finish()
        r.batch:clear()
        r.state.texture = t
        gl.Texture.bind(gl.TEXTURE_2D, t)
    end
end

--- @param r Renderer
--- @param t selene.gl.Texture | nil
local function setImage(r, t)
    t = t or r.whiteImage.handle
    if t ~= r.state.texture then
        r:finish()
        r.batch:clear()
        r.state.texture = t
        gl.Texture.bind(gl.TEXTURE_2D, t)
    end
end

local function setDrawMode(r, mode)
    if mode ~= r.state.drawMode then
        r:finish()
        r.batch:clear()
        r.state.drawMode = mode
    end
end

local function sendMatrix(program, name, matrix)
    local loc = program:getUniformLocation(name)
    gl.uniformMatrix4fv(loc, 1, false, matrix)
end

function Renderer:setEffect(effect)
    effect = effect or self.defaultEffect
    if effect.program ~= self.state.program then
        self:finish()
        self.batch:clear()
        self.state.program = effect.program
        gl.Program.use(effect.program)
        effect:send("u_MVP", self.state.projection)
        effect:send("u_View", self.state.modelview)
    end
end

function Renderer:setCanvas(canvas)
    canvas = canvas or self.defaultCanvas
    if canvas.handle ~= self.state.framebuffer then
        self:finish()
        self.batch:clear()
        gl.viewport(0, 0, canvas.width, canvas.height)
        gl.Framebuffer.bind(gl.FRAMEBUFFER, canvas.handle)
        self.state.framebuffer = canvas.handle
        
        self.state.projection:ortho(0, canvas.width, canvas.height, 0, -1, 1)
        -- local loc = self.state.program:getUniformLocation("u_MVP")
        -- gl.uniformMatrix4fv(loc, 1, false, self.state.projection)
        sendMatrix(self.state.program, "u_MVP", self.state.projection)
        sendMatrix(self.state.program, "u_View", self.state.modelview)
    end
end

function Renderer:setCamera(camera)
    camera = camera or self.defaultCamera
end

--- @field rect Rect | nil
function Renderer:setClipRect(rect)
    self:finish()
    self.batch:clear()
    if not rect then
        gl.scissor(0, 0, size[1], size[2])
    else
        gl.scissor(rect.x, size[2] - rect.h - rect.y, rect.w, rect.h)
        -- print(rect.x, rect.y, rect.w, rect.h)
    end
end

function Renderer:setFont(font)
    font = font or self.defaultFont
    self.state.font = font
  end

function Renderer:setDrawColor(c)
    self.state.drawColor = c
end

--- @param c Color
function Renderer:setClearColor(c)
    self.state.clearColor = c
    gl.clearColor(c:toFloat())
end

function Renderer:clear()
    gl.clear(gl.COLOR_BUFFER_BIT)
end

function Renderer:begin()
    self.batch:clear()
    self.drawCalls = {}

    self:setEffect()
    self:setFont()
    self:setCanvas()

    gl.enable(gl.BLEND, gl.SCISSOR_TEST)
    gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA)
    self.defaultCanvas.width, self.defaultCanvas.height = self.window:getSize()

    self.state.modelview:identity()

    self:setDrawColor(Color.white)
end

function Renderer:finish()
    local mode = drawModes[self.state.drawMode]
    assert(mode, "Invalid draw mode")
    if not self.batch:flush() then return end
    local count = self.batch:getCount()
    self.vao:bind()
    gl.drawArrays(mode, 0, count)
    gl.VertexArray.unbind()
end

function Renderer:onResize(w, h)
    gl.viewport(0, 0, w, h)
    self.state.projection:ortho(0, w, h, 0, -1, 1)
    size[1] = w
    size[2] = h
    self.defaultCanvas.width = w
    self.defaultCanvas.height = h
    print('Resizing: ', w, h)
    if self.state.program then
        -- local loc = self.state.program:getUniformLocation("u_MVP")
        -- gl.uniformMatrix4fv(loc, 1, false, self.state.projection)
        sendMatrix(self.state.program, "u_MVP", self.state.projection)
        sendMatrix(self.state.program, "u_View", self.state.modelview)
    end
end

function Renderer:origin()
    self:finish()
    self.batch:clear()
    self.state.modelview:identity()
    sendMatrix(self.state.program, "u_View", self.state.modelview)
end

function Renderer:translate(x, y)
    self:finish()
    self.batch:clear()
    self.state.modelview:translate(x, y)
    sendMatrix(self.state.program, "u_View", self.state.modelview)
end

function Renderer:scale(x, y)
    self:finish()
    self.batch:clear()
    self.state.modelview:scale(x, y)
    sendMatrix(self.state.program, "u_View", self.state.modelview)
end

function Renderer:rotate(angle)
    self:finish()
    self.batch:clear()
    self.state.modelview:rotate(angle)
    sendMatrix(self.state.program, "u_View", self.state.modelview)
end

function Renderer:drawPoint(x, y)
    setImage(self)
    setDrawMode(self, 'points')
    local r,g,b,a = self.state.drawColor:toFloat()
    self.batch:push(x, y, r, g, b, a, 0, 0)
end

function Renderer:drawLine(x0, y0, x1, y1)
    setImage(self)
    setDrawMode(self, 'lines')
    local r,g,b,a = self.state.drawColor:toFloat()
    self.batch:push(x0, y0, r, g, b, a, 0.0, 0.0)
    self.batch:push(x1, y1, r, g, b, a, 0.0, 0.0)
end

function Renderer:drawRectangle(x, y, width, height)
    setImage(self)
    setDrawMode(self, 'lines')
    local r,g,b,a = self.state.drawColor:toFloat()
    self.batch:push(x, y, r, g, b, a, 0.0, 0.0)
    self.batch:push(x+width, y, r, g, b, a, 0.0, 0.0)

    self.batch:push(x+width, y, r, g, b, a, 0.0, 0.0)
    self.batch:push(x+width, y+height, r, g, b, a, 0.0, 0.0)

    self.batch:push(x+width, y+height, r, g, b, a, 0.0, 0.0)
    self.batch:push(x, y+height, r, g, b, a, 0.0, 0.0)

    self.batch:push(x, y+height, r, g, b, a, 0.0, 0.0)
    self.batch:push(x, y, r, g, b, a, 0.0, 0.0)
end

function Renderer:fillRectangle(x, y, width, height)
    setImage(self)
    setDrawMode(self, 'triangles')
    local r,g,b,a = self.state.drawColor:toFloat()
    self.batch:push(x, y, r, g, b, a, 0.0, 0.0)
    self.batch:push(x+width, y, r, g, b, a, 0.0, 0.0)
    self.batch:push(x+width, y+height, r, g, b, a, 0.0, 0.0)

    self.batch:push(x, y, r, g, b, a, 0, 0)
    self.batch:push(x+width, y+height, r, g, b, a, 0, 0)
    self.batch:push(x, y+height, r, g, b, a, 0, 0)
end

function Renderer:drawCircle(x, y, radius, side)
    setImage(self)
    setDrawMode(self, 'lines')
    sides = sides or 32.0
    local pi2 = math.pi * 2
    local r,g,b,a = self.state.drawColor:toFloat()
    local u,v = 0, 0
    for i=1,sides do
    local tetha = ((i-1) * pi2) / sides

    local xx = x + (math.cos(tetha) * radius)
    local yy = y + (math.sin(tetha) * radius)
    self.batch:push(xx, yy, r, g, b, a, u, v)

    tetha = (i * pi2) / sides
    xx = x + (math.cos(tetha) * radius)
    yy = y + (math.sin(tetha) * radius)
    self.batch:push(xx, yy, r, g, b, a, u, v)
    end
end

function Renderer:fillCircle(x, y, radius, sides)
    setImage(self)
    setDrawMode(self, 'triangles')
    sides = sides or 32.0
    local pi2 = math.pi * 2
    local r,g,b,a = self.state.drawColor:toFloat()
    local u, v = 0, 0
    for i=1,sides do
        self.batch:push(x, y, r, g, b, a, u, v)

        local tetha = ((i-1) * pi2) / sides
        local xx = x + (math.cos(tetha) * radius)
        local yy = y + (math.sin(tetha) * radius)
        self.batch:push(xx, yy, r, g, b, a, u, v)

        tetha = (i * pi2) / sides
        xx = x + (math.cos(tetha) * radius)
        yy = y + (math.sin(tetha) * radius)
        self.batch:push(xx, yy, r, g, b, a, u, v)
    end
end

function Renderer:drawTriangle(p0, p1, p2)
    setImage(self)
    setDrawMode(self, 'lines')
    local r,g,b,a = self.state.drawColor:toFloat()
    self.batch:push(p0[1], p0[2], r, g, b, a, 0.0, 0.0)
    self.batch:push(p1[1], p1[2], r, g, b, a, 0.0, 0.0)

    self.batch:push(p1[2], p1[2], r, g, b, a, 0.0, 0.0)
    self.batch:push(p2[1], p2[2], r, g, b, a, 0.0, 0.0)

    self.batch:push(p2[1], p2[2], r, g, b, a, 0.0, 0.0)
    self.batch:push(p0[1], p0[2], r, g, b, a, 0.0, 0.0)
end

function Renderer:fillTriangle(p0, p1, p2)
    setImage(self)
    setDrawMode(self, 'triangles')
    local r,g,b,a = self.state.drawColor:toFloat()
    self.batch:push(p0[1], p0[2], r, g, b, a, 0.0, 0.0)
    self.batch:push(p1[1], p1[2], r, g, b, a, 0.0, 0.0)
    self.batch:push(p2[1], p2[2], r, g, b, a, 0.0, 0.0)
end

--- @param drawable Drawable
---@param src Rect | nil
---@param dest Rect | nil
function Renderer:copy(drawable, src, dest)
    setImage(self, drawable:getTexture())
    setDrawMode(self, "triangles")
    local o = drawable

    local r,g,b,a = self.state.drawColor:toFloat()
    local s = src or Rect.create(0, 0, o:getWidth(), o:getHeight())
    local d = dest or {x = 0, y = 0, w = s.w, h = s.h}
    local uv = o:getUV(s)
    -- local uv = {0, 0, 1, 1}

    self.batch:push(d.x, d.y, r, g, b, a, uv[1], uv[2])
    self.batch:push(d.x+d.w, d.y, r, g, b, a, uv[3], uv[2])
    self.batch:push(d.x+d.w, d.y+d.h, r, g, b, a, uv[3], uv[4])

    self.batch:push(d.x, d.y, r, g, b, a, uv[1], uv[2])
    self.batch:push(d.x+d.w, d.y+d.h, r, g, b, a, uv[3], uv[4])
    self.batch:push(d.x, d.y+d.h, r, g, b, a, uv[1], uv[4])
end

function Renderer:print(text, x, y)
    setDrawMode(self, "triangles")

    local ox = x or 0
    local oy = y or 0

    local font = self.state.font
    local image = font.texture
    setImage(self, image)
    local r,g,b,a = self.state.drawColor:toFloat()
    local i = 1
    while i <= #text do
        local c = text:byte(i)
        local codepoint, bytes = selene.UTF8Codepoint(text, i)
        i = i + bytes
        local rect = font.rects[codepoint]
        if c == string.byte('\n') then
            ox = x
            oy = oy + font.height
        elseif c == string.byte('\t') then
            ox = ox + (rect.bw * 2)
        else
            local xx = ox + rect.bl
            local yy = oy + rect.bt

            local uv = {}
            uv[1] = rect.tx / font.width
            uv[2] = 0
            uv[3] = uv[1] + (rect.bw / font.width)
            uv[4] = uv[2] + (rect.bh / font.height)
            self.batch:push(xx, yy, r, g, b, a, uv[1], uv[2])
            self.batch:push(xx+rect.bw, yy, r, g, b, a, uv[3], uv[2])
            self.batch:push(xx+rect.bw, yy+rect.bh, r, g, b, a, uv[3], uv[4])

            self.batch:push(xx, yy, r, g, b, a, uv[1], uv[2])
            self.batch:push(xx+rect.bw, yy+rect.bh, r, g, b, a, uv[3], uv[4])
            self.batch:push(xx, yy+rect.bh, r, g, b, a, uv[1], uv[4])

            ox = ox + rect.ax
            oy = oy + rect.ay
        end
    end
end

return Renderer
