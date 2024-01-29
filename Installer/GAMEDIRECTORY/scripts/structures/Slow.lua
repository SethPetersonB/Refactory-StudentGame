
function BehaviorState(structParent)
  
   -- variable that governs how much enemies are slowed
  local slowfactor = 0.5

   -- Misc variables
  local this = {}
  local children = {}
  local targets = {}
  local top = nil -- Need to store ID or will index out of range if block is destroyed
  local elapesdTime = 0
  local tilescale = Vector2(37.5, 37.5) -- Tile width & height / 2, should actually grab these variables 
  local topInst = nil
  local parent = structParent
  local colors = {}
  local tiles = {}
  --local audio = game:GetSystem("Audio_Engine")

  local function contains(table, element)
    for _, value in pairs(table) do
      if value == element then
        return true
      end
    end
    return false
  end

  local function InRange(topInst, enemy, moving)
    local instpos = topInst.Transform.position
    local enemypos = enemy.Transform.position
    local inrange = false

    -- this check is because the moment enemies spawn they are not moving, so 
    -- the tower would do nothing to them.
    if moving then
    if enemy.Physics.velocity == Vector2(0, 0) then return false end end
       
    if enemypos.x >= instpos.x - tilescale.x and enemypos.x <= instpos.x + tilescale.x then
      inrange = true
    end

    if enemypos.y >= instpos.y - tilescale.y and enemypos.y <= instpos.y + tilescale.y then
      inrange = true
    end

    return inrange 
  end

  local function SlowTargets()
    local enemies = stage:GetChildren("Enemy1")

    for i, enemy in ipairs(enemies) do
      for _, child in pairs(children) do 
        if InRange(child, enemy, true) then 
          if contains(targets, enemy) == false then
            -- slow enemies down 
            local vel = slowfactor * enemy.Physics.velocity
          --  print("velocity before = ", enemy.Physics.velocity.x, " ", enemy.Physics.velocity.y)
            enemy.Physics.velocity = vel
            enemy.Physics.acceleration = slowfactor * enemy.Physics.acceleration
          --  print("velcity after = ", enemy.Physics.velocity.x, " ", enemy.Physics.velocity.y)

            -- put enemy in list of targets
            targets[#targets + 1] = enemy
          end
        end
      end
    end
  
  end

  function SetColor()
   -- HARD CODED DEPRESSION
    local pos = {}
    pos[1] = children[1]
    if InRange(children[1], children[2], false) == false then pos[2] = children[2]
    elseif InRange(children[1], children[3], false) == false then pos[2] = children[3]
    elseif InRange(children[1], children[4], false) == false then pos[2] = children[4]
    end

   parent.StructureBase:SetColor(pos[1].Transform.position, pos[2].Transform.position)
      
  end
  
  function this:Load()
    
    local childIds = parent.StructureBase:GetInstanceList()
    --local topInst = nil
    
    for i in childIds do
      local inst = stage[i]
      children[#children + 1] = inst -- Put children from instance list into structure list
      
      if top == nil or inst.Transform.depth > topInst.Transform.depth then
        topInst = inst
        top = i
      end
    end
    SetColor()
  end
  
  function this:Update(dt)
    elapesdTime = elapesdTime + dt 

    SlowTargets()

    -- checks if target has been lost
    for i, enemy in pairs(targets) do
    local range = false
      for _, child in pairs(children) do
        if InRange(child, enemy)then
          range = true
        end
      end
      if range == false then 
        enemy.Physics.velocity = (1.0 / slowfactor) * enemy.Physics.velocity
        enemy.Physics.acceleration = (1.0 / slowfactor) * enemy.Physics.acceleration
        targets[i] = nil
      end
    end
    
  end
  
  function this:Unload()
   
    for _, enemy in pairs(targets) do
      -- set velocity to normal
      enemy.Physics.velocity = (1.0 / slowfactor) * enemy.Physics.velocity
      enemy.Physics.acceleration = (1.0 / slowfactor) * enemy.Physics.acceleration
    end

    targets = {} -- clear targets 
  end

  
  return this
end

return BehaviorState;