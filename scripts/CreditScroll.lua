-- Primary Author : Philip Nygard
--
--© Copyright 1996-2016, DigiPen Institute of Technology (USA). All rights reserved.
---------------------------------------------------------------------------------
local speed = 0.25
print("Start")
wait(3)

while true do
  local dt = wait()

  parent.UIFrame.position = parent.UIFrame.position + speed * Vector2(0, dt)
end