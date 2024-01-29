-- Primary Author : Dylan Weber
--
--© Copyright 1996-2016, DigiPen Institute of Technology (USA). All rights reserved.
---------------------------------------------------------------------------------
function BehaviorState(structParent)
  
   -- shield properties
   local hpboost = 2 
   local shieldpower = 1
   -- using immuneTurn is a bit weird because each structure is composed of 3 blocks, 
   -- meaning blocks could be different colors and confusing when multiple enemies hit it at once so.. maybe not use it?
   -- unless you want each independent block to have immune turns...?
   local immuneTurn = 0 -- every # of hits that shield will take no damage, if 0 it will have no immune turns

   -- Misc variables
  local this = {}
  local children = {}
  local top = nil -- Need to store ID or will index out of range if block is destroyed
  local elapesdTime = 0
  local topInst = nil
  local parent = structParent
  local objects = {}

  --local audio = game:GetSystem("Audio_Engine")


  function this:Load()
    
    local childIds = parent.StructureBase:GetInstanceList()
    --local topInst = nil
    
    -- child blocks will be fulled healed + buffed on shield creation
    for i in childIds do
      local inst = stage[i]
      inst.ObjectStats.maxHp = inst.ObjectStats.maxHp + hpboost -- increase health
      inst.ObjectStats.hp = inst.ObjectStats.maxHp
      inst.StructureLogic.shield = inst.StructureLogic.shield + shieldpower -- set shield
      inst.Sprite.frame = 2 -- change sprite 
      inst.Sprite.color = Color4(1, 1, 1, 1)
      inst.StructureLogic.immuneTurn = immuneTurn
      objects[#objects + 1] = inst.id
      children[#children + 1] = inst -- Put children from instance list into structure list
      
      if top == nil or inst.Transform.depth > topInst.Transform.depth then
        topInst = inst
        top = i
      end
    end

  end
  
  function this:Update(dt)
    elapesdTime = elapesdTime + dt 

    -- sychronize data between blocks in shield structure 
    local largestHit = 0

    for _, child in pairs(children) do 
      local timesHit = child.StructureLogic.timesHit
      if timesHit > largestHit then largestHit = timesHit end
    end

    if largest ~= 0 then
      for _, child in pairs(children) do
        child.StructureLogic.timesHit = largestHit
      end
    end
    
  end
  
  function this:Unload()
   
    for _, i in pairs(objects) do 
      if stage[i] ~= nil then
        stage[i].ObjectStats.maxHp = stage[i].ObjectStats.maxHp - hpboost
        stage[i].StructureLogic.shield = stage[i].StructureLogic.shield - shieldpower
        stage[i].Sprite.frame = 0
        stage[i].StructureLogic.immuneTurn = 0
      end
    end

  end

  
  return this
end

return BehaviorState;