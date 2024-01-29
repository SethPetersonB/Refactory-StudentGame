-- Primary Author : Philip Nygard
--
--© Copyright 1996-2016, DigiPen Institute of Technology (USA). All rights reserved.
---------------------------------------------------------------------------------
while wait() do
  if parent.Transform.depth <= -1 then
    stage:RemoveInstance(parent)
    break;
  end
end
