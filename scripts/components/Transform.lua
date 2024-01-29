-- Primary Author : Philip Nygard
--
--© Copyright 1996-2016, DigiPen Institute of Technology (USA). All rights reserved.
---------------------------------------------------------------------------------
--[[ 
    Connecting events. Listeners are stored as smart pointers, 
    so these will be active so long as there is at least one reference to 
    them, or until you manually disconnect them.

    Listeners will be fired immediatly before the script update tick in the 
    game loop
]]--

--[[

Storing game objects:
gameObjects = {}

gameObjects["objectName"] = {instance, instance, instance}


]]
-- Position moved
local onMoved = parent.PositionMoved:Connect(
  function(movement)
    parent.Transform.position = parent.Transform.position + movement
  end)

-- Position set
local onSet = parent.PositionSet:Connect(
  function(movement)
    parent.Transform.position = movement
  end)

-- Rotated
local onRotated = parent.Rotated:Connect(
  function(rot)
    parent.Transform.rotation = parent.Transform.rotation + rot
  end)

-- Rotation set
local onRotSet = parent.RotationSet:Connect(
  function(rot)
    parent.Transform.rotation = rot
  end)

local moveSpeed = -100

-- Depth set
local onDepthSet = parent.TransformDepthSet:Connect(
  function(depth)
    parent.Transform.depth = depth
  end)

--[[
  This is necessary as the scope for the script will expire if execution ends for it, meaning
  that unless you have an empty infinite wait block like this, event listeners will be destroyed
  after execution finishes. This is a bug, and I'm looking into how to fix it. For now, just have
  an empty infinite look with a yield call in the condition check at the bottom of scripts that need
  to persist
--]]
--while(wait()) do end
done()
