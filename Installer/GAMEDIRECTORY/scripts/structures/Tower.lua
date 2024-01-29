
function BehaviorState(structParent)
  
  -- Tower properties
  local cooldownTime = 0.5      -- Seconds between firing
  local range = 200             -- Range of the tower
  local shotSpeed = 1000        -- Speed of projectiles
  local damage = 5              -- Damage dealt
  local indicatorColor = Color4(1, 1, 1, 0.25)  -- RGBA color of the range indicator

  -- Fireball properties
  local fireColor = Color4(1, 1, 1, 0.9)    -- RGBA color of the fireball
  local chargeWait = 0.25   -- Time before full charge to have full opacity
  local startDepth = 4.5    -- Starting depth
  local bobAmplitude = 0.5  -- Max epth offset
  local bobFrequency = 3    -- Bobbing speed

   -- Misc variables
  local this = {}
  local children = {}
  local top = nil -- Need to store ID or will index out of range if block is destroyed
  local fire = nil
  local circle = nil
  local elapesdTime = 0
  local cooldown = cooldownTime

  local parent = structParent
  local audio = game:GetSystem("Audio_Engine")

  local function GetClosestEnemy()
    local enemies = stage:GetChildren("Enemy1")
    local closest = nil
    local dist = math.huge
    
    for i, enemy in ipairs(enemies) do
      local eDist = (fire.Transform.position - enemy.Transform.position).magnitude
      if eDist <= range and eDist < dist then
        dist = eDist
        closest = enemy
      end
    end
    
     return closest
  end
  
  local function FireIfReady(dt)
    if cooldown <= 0 then
        local closest = GetClosestEnemy()
        if closest ~= nil then
          local proj = stage:NewInstance("TowerProjectile")
          local dist = (closest.Transform.position - fire.Transform.position).magnitude
          local time = dist / shotSpeed
          local dir = (closest.Transform.position - fire.Transform.position).unit
          proj.Transform.depth = fire.Transform.depth
          proj.Transform.scale = fire.Transform.scale
          proj.Transform.position = fire.Transform.position
          proj.Physics.velocity = shotSpeed * dir
          proj.Physics.depthVelocity = -(proj.Transform.depth * 1.25) / time
          proj.ObjectStats.damage = damage
          audio:PlaySounds("tower_attack.wav", -18)
          fire.Sprite.color = Color4(fireColor.r, fireColor.g, fireColor.b, 0)
	  cooldown = cooldownTime
        end
      else
      cooldown = cooldown - dt
      
      if cooldown - chargeWait <= 0 then
        fire.Sprite.color = fireColor
      else
        fire.Sprite.color = Color4(fireColor.r, fireColor.g, fireColor.b, (1 - ((cooldown - chargeWait) / cooldownTime)) * fireColor.a)
      end
      end
  end
  
  function this:Load()
    
    local childIds = parent.StructureBase:GetInstanceList()
    local topInst = nil
    
    for i in childIds do
      local inst = stage[i]
      children[#children + 1] = inst -- Put children from instance list into structure list
      
      if top == nil or inst.Transform.depth > topInst.Transform.depth then
        topInst = inst
        top = i
      end
    end
    
    -- Set top sprite
    if top ~= nil then
      topInst.Sprite.frame = 1
    end
    
    -- Create fireball
    fire = stage:NewInstance("Fire")
    
    fire.Transform.position = topInst.Transform.position
    --fire.Transform.scale = Vector2(15, 15)
    fire.Transform.depth = topInst.Transform.depth + startDepth
    fire.Sprite.color= fireColor

    if indicatorColor.a > 0 then
      circle = stage:NewInstance("TowerCircle")
      circle.Transform.scale = Vector2(range * 2, range * 2)
      circle.Transform.position = topInst.Transform.position
      circle.Transform.depth = 0
      circle.Sprite.color = indicatorColor
      end
  end
  
  function this:Update(dt)
    elapesdTime = elapesdTime + dt 
    
    if fire then
      fire.Transform.depth = startDepth + bobAmplitude * (math.sin(elapesdTime * bobFrequency) + 1)
      
      FireIfReady(dt)
    end
    
  end
  
  function this:Unload()
    local topInst = stage[top]
    
    if topInst ~= nil then
      topInst.Sprite.frame = 0
    end
  
    if fire ~= nil then
      stage:RemoveInstance(fire)
      fire = nil
    end
    
    if circle ~= nil then
      stage:RemoveInstance(circle)
      circle = nil
    end
  end
  
  return this
end

return BehaviorState;
