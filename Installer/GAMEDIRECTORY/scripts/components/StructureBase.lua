
local structDef = nil

local onStructureLoaded = parent.StructureLoaded:Connect( 
  function(type)
    print(parent.id .. ": Loaded type " .. type)
    structDef = require("structures/" .. type)(parent)
    structDef:Load()
  end)

local onStructureUnloaded = parent.StructureUnloaded:Connect(
  function(type)
    print(parent.id .. ": Unloaded type " .. type)

    if structDef then
      structDef:Unload()
    end

    structDef = nil
  end)


  local onStructureUpdate = parent.StructureBaseUpdate:Connect(
    function(dt)
      if structDef then
        structDef:Update(dt)
      end
    end)

  done()
