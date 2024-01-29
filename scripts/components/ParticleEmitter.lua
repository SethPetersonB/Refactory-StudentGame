-- Primary Author : Dylan Weber
--
--© Copyright 1996-2016, DigiPen Institute of Technology (USA). All rights reserved.
---------------------------------------------------------------------------------
local emitter = parent.ParticleEmitter

local particles = {}

local onActive = parent.SetParticlesActive:Connect(
  function(active)
    parent.ParticleEmitter.active = active
  end)

local onSetParticles = parent.SetNumParticles:Connect(
  function(particles)
    parent.ParticleEmitter.num_particles = particles
  end)


function CreateParticles(type_)
  if not emitter.active then
    local particle = require(type_)()
    particles = particle:call(emitter, parent)
      
    emitter.active = true
  end
end


 local onUpdate = parent.ParticleEmitterUpdate:Connect(
   function(dt)
    local i = 1

    while i <= #particles do
      particles[i].elapsed = particles[i].elapsed + dt

      if particles[i].elapsed >= particles[i].lifespan then
        --stage:RemoveInstance(particles[i].entity)
        table.remove(particles, i)
      else
        i = i + 1
      end
    end
 end)

 local onParticle = parent.CreateParticles:Connect(
 function(type_)
 CreateParticles(type_)
 end)

--while wait() do end
done()