-- Primary Author : Philip Nygard
--
--© Copyright 1996-2016, DigiPen Institute of Technology (USA). All rights reserved.
---------------------------------------------------------------------------------
local onHPSet = parent.HpSet:Connect( 
  function(val)
    parent.ObjectStats.hp = val
  end)
  
local onDmgSet = parent.DamageSet:Connect(
  function(val)
    parent.ObjectStats.damage = val
  end)

done()