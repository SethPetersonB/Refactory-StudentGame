local onHPSet = parent.HpSet:Connect( 
  function(val)
    parent.ObjectStats.hp = val
  end)
  
local onDmgSet = parent.DamageSet:Connect(
  function(val)
    parent.ObjectStats.damage = val
  end)

done()