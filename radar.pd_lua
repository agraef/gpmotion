local radar = pd.Class:new():register("radar")

-- creation arguments: name (receiver name, default "gravity")
-- axis (axis of rotation, default "z")
-- input range (default: )
function radar:initialize(sel, atoms)
   self.inlets = 0
   self.outlets = 0
   -- normalized coordinates (center = (0,0), nw = (-1,-1))
   self.x = 0
   self.y = 0
   -- button press (1 = on, 0 = off) for crosshairs
   self.z = 0
   -- foreground and background colors
   self.bg = Colors.background
   self.fg = Colors.foreground
   -- object size
   self:set_size(64, 64)
   -- receiver
   self.recv = type(atoms[1]) == "string" and atoms[1] or "touchpad"
   self.r_input = pd.receive(self, self.recv, "input")
   self.r_press = pd.receive(self, self.recv .. "z", "press")
   -- input range
   self.max = type(atoms[2]) == "number" and atoms[2] or 1
   -- axes
   local axis = type(atoms[2]) == "string" and atoms[2] or
      type(atoms[3]) == "string" and atoms[3] or "z"
   if axis == "x" then
      self.axes = {2,3}
   elseif axis == "y" then
      self.axes = {1,3}
   else
      self.axes = {1,2}
   end
   return true
end

-- translate the coordinates for the main (x, y) input
function radar:translate(x)
   if type(x) == "number" then
      return x/self.max
   else
      return 0
   end
end

function radar:input(sel, atoms)
   --pd.post(string.format("%s: %s %s", self.recv, sel, table.concat(atoms, " ")))
   if sel ~= "list" then
      return
   elseif self.recv == "touchpad"  and #atoms == 2 then
      -- touchpad has unipolar coordinates
      atoms[1] = 2*atoms[1]-1
      atoms[2] = 2*atoms[2]-1
   end
   if #atoms == 2 then
      self.x = self:translate(atoms[1])
      self.y = self:translate(atoms[2])
   elseif #atoms == 3 then
      self.x = self:translate(atoms[self.axes[1]])
      self.y = self:translate(atoms[self.axes[2]])
   else
      return
   end
   self:repaint()
end

function radar:press(sel, atoms)
   --pd.post(string.format("%s: %s %s", self.recv, sel, table.concat(atoms, " ")))
   if sel == "float" then
      self.z = atoms[1] ~= 0 and 1 or 0
      self:repaint()
   end
end

-- draw the radar
function radar:paint(g)
   local width, height = self:get_size()
   local x, y = (self.x+1)*width/2, (self.y+1)*width/2

   -- standard object border, fill with bg color
   g:set_color(0)
   g:fill_all()

   -- helper function to set a color value
   function set_color(x)
      if type(x) == "table" then
	 g:set_color(table.unpack(x))
      elseif type(x) == "number" then
	 g:set_color(x)
      end
   end

   -- draw circle and hand of the radar in the set colors
   set_color(self.bg)
   g:fill_ellipse(1, 1, width - 2, height - 2)
   set_color(self.fg)
   g:stroke_ellipse(0, 0, width, height, 1)
   g:fill_ellipse(width/2 - 2, height/2 - 2, 4, 4)
   g:fill_ellipse(x - 3.5, y - 3.5, 7, 7)
   g:draw_line(x, y, width/2, height/2, 2)
   -- draw crosshairs if z is nonzero
   if self.z ~= 0 then
      g:draw_line(width/2, 0, width/2, height, 1)
      g:draw_line(0, height/2, width, height/2, 1)
   end
end
