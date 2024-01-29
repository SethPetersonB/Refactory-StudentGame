-- Primary Author : Philip Nygard
--
--© Copyright 1996-2016, DigiPen Institute of Technology (USA). All rights reserved.
---------------------------------------------------------------------------------
local particle = parent.Particle
local t = 0
local function mag(vec)
  local m = math.sqrt(vec.x * vec.x + vec.y * vec.y)
  return m
end

local function dot(a, b)
  local m = a.x * b.x + a.y * b.y
  return m
end

local onUpdate = parent.ParticleUpdate:Connect(
function(dt)
  
  if particle.type == "spiral" then
    local speed = particle.speed
    local dir = parent.Transform.position - particle.origin -- direction vector 
    local r = mag(dir)
    local EX = Vector2(1 , 0) -- normal right vector 
    local theta = math.acos(dot(dir, EX) / mag(dir) * mag(EX)) -- find angle from horizontal 
   -- local pos = parent.Transform.position + Vector2(particle.angle * t * math.cos(2 * math.pi * t), particle.angle * t* math.sin(2 * math.pi * t))
    local vel = speed * Vector2(r * -math.sin(theta), r * math.cos(theta)) -- derivative of position on circle
    --parent.Transform.position = pos
    parent.Physics.velocity = vel
 
  end
   t = t + dt
end)
