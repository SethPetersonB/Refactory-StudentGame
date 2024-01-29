local audio = game:GetSystem("Audio_Engine")

wait()
local range = parent.ObjectStats.hp
local speed = 700

local hit = {}

local dist = 0

function hitEnemies()
  local enemies = stage:GetChildren("Enemy1")

  for _, enemy in ipairs(enemies) do
    -- If we haven't hit the enemy yet
    if hit[enemy.id] == nil then
      -- Determine if enemy is in range
      local eDist = (parent.Transform.position - enemy.Transform.position).magnitude

      if eDist <= dist / 2 then
        -- damage enemy and create feedback
        enemy.ParticleEmitter:Emit("explosion")
        enemy.ObjectStats.hp = enemy.ObjectStats.hp - parent.ObjectStats.damage
        audio:PlaySounds("shock.wav", -30)

        -- Add enemy to the hit list
        hit[enemy.id] = enemy
      end
    end
  end
end

while dist <= range  do
  local dt = wait()
  dist = dist + speed * dt

  hitEnemies()

  parent.Transform.scale = Vector2(dist, dist)  
end

local color = parent.Sprite.color
local time = 0.25
local elapsed = time
while elapsed > 0 do
  elapsed = elapsed - wait()
  parent.Sprite.color = Color4(color.r, color.g, color.b, color.a * (elapsed / time))
  hitEnemies()
end

stage:RemoveInstance(parent)
