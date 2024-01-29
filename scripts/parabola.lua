-- Primary Author : Dylan Weber
--
--© Copyright 1996-2016, DigiPen Institute of Technology (USA). All rights reserved.
---------------------------------------------------------------------------------
local function parabola()
  local this = {}

  function this:call(emitter, parent)
    local particles = {}
    local focus = parent.Transform.position
    local p = math.random(-50, 50)
    local vertex = Vector2(focus.x + p, focus.y + p)
    local left = 0
    local right = 0
    local horizontal = math.random(0, 1)

    for i = 0, emitter.num_particles do
      local part = {}
      -- (x - vertex.x)^2 = 4p(y - vertex.y)
      -- y = (x - vertex.x)^2 / 4p + vertex.y
      local x = 0
      local y = 0
       
      if i < emitter.num_particles / 2 then
        if horizontal == 1 then
        x = vertex.x - left
        y = (((x - vertex.x) * (x - vertex.x)) / (4 * p)) + vertex.y
        else
        y = vertex.y - left
        x = (((y - vertex.y) * (y - vertex.y)) / (4 * p)) + vertex.x
        end
        left = left - 10
         
      elseif i >= emitter.num_particles / 2 then
        if horizontal == 1 then
        x = vertex.x + right
        y = (((x - vertex.x) * (x - vertex.x)) / (4 * p)) + vertex.y
        else
        y = vertex.y + right
        x = (((y - vertex.y) * (y - vertex.y)) / (4 * p)) + vertex.x
        end
        right = right - 10
      end

        -- Create particle
      part.entity = stage:NewInstance("TestParticle")
      part.elapsed = 0
      --part.lifespan = math.random_normal(emitter.life_mean, emitter.life_deviation)
      part.entity.Particle.lifespan = math.random_normal(0, 0.7)
      part.lifespan = part.entity.Particle.lifespan
      --part.entity.Physics.velocity = vel

      part.entity.Transform.position = Vector2(x, y)
      part.entity.Particle.origin = Vector2(x, y)
      part.entity.Particle.type = "parabola"

       -- Put particle into table
      particles[#particles + 1] = part

    end
    
    return particles
  end

  return this

end

return parabola
