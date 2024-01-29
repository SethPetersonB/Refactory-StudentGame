-- Primary Author : Dylan Weber
--
--© Copyright 1996-2016, DigiPen Institute of Technology (USA). All rights reserved.
---------------------------------------------------------------------------------
local input = game:GetSystem("InputSystem")
local audio = game:GetSystem("Audio_Engine")
local camera = game:GetSystem("Camera")
local mouse = game:GetSystem("Mouse")

local controller = parent.Controller

onHPSet = parent.SetPlayerHP:Connect( function(hp)
  parent.Controller.hp = hp
end)

onWallsSet = parent.SetAvailableWalls:Connect( function(walls)
  parent.Controller.walls = walls
end)

onTakeDamage = parent.PlayerTakeDamage:Connect( function(damage)
  if not controller.god then
    parent.Controller.hp =  parent.Controller.hp - damage
  end
end)

onToggleGod = parent.ToggleGod:Connect( function()
  parent.Controller.god =  not parent.Controller.god
end)

onToggleAutoplay = parent.ToggleAuto:Connect( function()
  parent.Controller.autoplay =  not parent.Controller.autoplay
end)

onControllerUpdate = parent.ControllerUpdate:Connect( function(dt)
  
  if stage:IsRunning() then
    -- Toggle god mode
    if input:KeyPressed(Enum.KeyCode.G) and input:KeyDown(Enum.KeyCode.LShift) then
      parent.Controller.god = not parent.Controller.god
    end

    -- Toggle autoplay
    if input:KeyPressed(Enum.KeyCode.F6) then
      parent.Controller.autoplay = not parent.Controller.autoplay
    end

    if input:KeyPressed(Enum.KeyCode.K) then 
    audio:PlaySounds("building_destroyed_fast.wav", 0)
    end 

    if input:KeyDown(Enum.KeyCode.A) or input:KeyDown(Enum.KeyCode.Left) then
    camera:Translate(Vector2(-150 * dt, 0))
    end 

    if input:KeyDown(Enum.KeyCode.D) or input:KeyDown(Enum.KeyCode.Right) then
    camera:Translate(Vector2(150 * dt, 0))
    end

    if input:KeyDown(Enum.KeyCode.W) or input:KeyDown(Enum.KeyCode.Up) then
    camera:Translate(Vector2(0, 150 * dt))
    end
  
    if input:KeyDown(Enum.KeyCode.S) or input:KeyDown(Enum.KeyCode.Down) then
  camera:Translate(Vector2(0, -150 * dt))
    end

    if input:KeyPressed(Enum.KeyCode.Q) then
    camera:Rotate(math.pi / 2, math.pi * 2)
    end

    if input:KeyPressed(Enum.KeyCode.E) then
    camera:Rotate(-math.pi / 2, math.pi * 2)
    end

    if mouse.scroll ~= 0 then
    camera:Zoom(-1.5 * mouse.scroll)
    end

  end
end)


--[[
onControllerPreUpdate = parent.ControllerPreUpdate:Connect( function(dt)
end)
]]--


--while wait() do end
done()
