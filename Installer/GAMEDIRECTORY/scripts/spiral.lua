local function spiral()
  local this = {}

  function this:call(emitter, parent)
    local particles = {}
    local a = 5 -- spiral radius?
    local point = parent.Transform.position
    local x = point.x
    local y = point.y
    
    for i = 0, emitter.num_particles do
      local part = {}
      local speed = 4 --math.random(4 , 6) -- particle speed
     
      local vel = Vector2(speed * math.cos(i) , speed * math.sin(i))


      if i ~= 0 then
        --x(t) = at cos(t), y(t) = at sin(t)
        x = point.x + a * i * math.cos(i)
        y = point.y + a * i * math.sin(i)
      end

      -- Create particle
      part.entity = stage:NewInstance("TestParticle")
      part.elapsed = 0
      --part.lifespan = math.random_normal(emitter.life_mean, emitter.life_deviation)
      part.entity.Particle.lifespan = math.random(1, 7) / 10
      part.lifespan = part.entity.Particle.lifespan
      part.entity.Particle.speed = speed
      part.entity.Particle.angle = i
      --part.entity.Physics.velocity = vel
      part.entity.Transform.position = Vector2(x, y)
      part.entity.Particle.origin = parent.Transform.position
      part.entity.Particle.type = "spiral"
       -- Put particle into table
      particles[#particles + 1] = part

      
    end
    return particles
  end

  return this

end

return spiral
