-- Primary Author : Philip Nygard
--
--© Copyright 1996-2016, DigiPen Institute of Technology (USA). All rights reserved.
---------------------------------------------------------------------------------
package.path = "./scripts/?.lua;" .. package.path

-- These are all the functions that can be called from within the sandbox environment (determined "safe")
local env = require 'environment'
local functional = require 'functional'

local modules = {}
local moduleRoot = "scripts/"

local scripts = {}
local scriptsCount = 0

function loadScript(file, instance)
  local hierarchy
  if instance ~= nil then
  local success
    success, hierarchy = assert(pcall(functional.build_hierarchy, functional, instance))
  end

  local success, id = pcall(functional.create_and_run, functional, file, hierarchy)
  if not success then 
    env.log.error(id)

    return Script(-1, file)
  else 
    return id
  end
end

function unloadScript(script)
  local success, mess = pcall(functional.kill_routine, functional, script)

  if not success then
    env.log.error(mess)
  end
end

function update(dt)
  pcall(functional.routines_burst, functional, dt)
end
  
function pushEvent(func, msg)
  local success, err = pcall(func, msg)

  if not success then env.log.error(err) end
end

function env_loadstring(data)
  local compile = loadstring(data)
  setfenv(compile, env)

  return compile
end

function init(stage)
  env.stage = stage.hierarchy

  -- Finds the first instance with a given type
  env.stage.FindFirstChild = function(stage, type)
    if stage[type] == nil then return nil end

    -- Easiest way to get first instance, as theres no way to know the lowest value
    for i, v in pairs(stage[type]) do
      return v
    end

    -- If instance list is empty
    return nil
  end

  -- Get all instances of a given type
  env.stage.GetChildren = function(stage, type)

    local children = {}
    
    if stage[type] ~= nil then
      for i, v in pairs(stage[type]) do
        children[#children + 1] = v
      end
    end

    return children
  end

  -- Wait fo an instance to exist 
  env.stage.WaitForChild = function(stage, type)

    local instance = nil

    repeat
      instance = stage:FindFirstChild(type)

      -- Wait until an instance with the given name is added
    until instance ~= nil or not env.wait() 

    return instance
  end

    -- Create a new instance on the stage
  env.stage.NewInstance = function(stage, type)
    return stage._RAW:NewInstance(type):GetHierarchy()
  end

  env.stage.IsRunning = function(stage)
   return stage._RAW:IsRunning()
  end

  --Remove an instance from the stage
  env.stage.RemoveInstance = function(stage, obj)

    if type(obj) == type(1) then
      obj = stage[obj]
    end

    return stage._RAW:RemoveInstance(obj._RAW)
  end
end

-- Rerout loadstring to custom string loader
env.loadstring = env_loadstring
--fetchGameState()
