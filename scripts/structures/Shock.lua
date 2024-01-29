-- Primary Author : Philip Nygard
--
--© Copyright 1996-2016, DigiPen Institute of Technology (USA). All rights reserved.
---------------------------------------------------------------------------------
function BehaviorState(structParent)
  
  -- Tower properties
  local cooldownTime = 1        -- Seconds between firing
  local range = 350             -- Range of the tower
  local damage = 2             -- Damage dealt
  local indicatorColor = Color4(1, 1, 1, 0.25)  -- RGBA color of the range indicator

  -- Fireball properties
  local fireColor = Color4(0, 0.75, 4, 0.9)    -- RGBA color of the fireball
  local chargeWait = 0.25   -- Time before full charge to have full opacity
  local startDepth = 4.5    -- Starting depth
  local bobAmplitude = 0.25  -- Max epth offset
  local bobFrequency = 1    -- Bobbing speed

   -- Misc variables
  local this = {}
  local children = {}
  local top = nil -- Need to store ID or will index out of range if block is destroyed
  local fire = nil
  local circle = nil
  local elapesdTime = 0
  local cooldown = cooldownTime
  local wave = 0
  local parent = structParent
  local audio = game:GetSystem("Audio_Engine")

  -- Check if there is an enemy in the towers range
  local function EnemyInRange()
    local enemies = stage:GetChildren("Enemy1")
    local topInst = stage[top]

    if topInst == nil then return false end

    for _, enemy in ipairs(enemies) do
      local dist = (enemy.Transform.position - topInst.Transform.position).magnitude

      if dist <= range then
        return true -- At least one enemy is in range
      end
    end

    return false  -- No enemies are in range
  end

  local function Fire()
    local sw = stage:NewInstance("ShockWave")
    sw.Transform.position = fire.Transform.position
    sw.Transform.depth = 0
    sw.ObjectStats.damage = damage
    sw.ObjectStats.hp = range * 2 -- Dumb, but we're using hp to calculate shockwave range
    sw.Transform.scale = Vector2(0, 0)
    sw.Sprite.color = 0.25* fireColor
    audio:PlaySounds("tower_splash_attack_slow.wav", -18)

    return sw.id
  end

  local function FireIfReady(dt)

    if cooldown <= 0 and stage[wave] == nil then
      
      if EnemyInRange() then

      wave = Fire()
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
