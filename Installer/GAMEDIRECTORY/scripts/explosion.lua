local function explosion()
  local this = {}

  function this:call(emitter, parent)
    local particles = {}

    for i = 0, emitter.num_particles do
      local part = {}
      local angle = math.random(0, 360)
      local speed = math.random_normal(emitter.speed_mean, emitter.speed_deviation)
      
      local vel = Vector2(speed * math.cos(math.rad(angle)), speed * math.sin(math.rad(angle)))

      -- Create particle
      part.entity = stage:NewInstance("TestParticle")
      part.elapsed = 0
      --part.lifespan = math.random_normal(emitter.life_mean, emitter.life_deviation)
      part.entity.Particle.lifespan = math.random(1, 10) / 10
      part.lifespan = part.entity.Particle.lifespan
      part.entity.Physics.velocity = vel
      part.entity.Transform.position = parent.Transform.position
      part.entity.Particle.origin = parent.Transform.position
      part.entity.Particle.type = "explosion"
      part.entity.Particle.speed = speed
      part.entity.Particle.angle = angle

       -- Put particle into table
      particles[#particles + 1] = part

      
    end
    return particles
  end

  return this

end

return explosion
