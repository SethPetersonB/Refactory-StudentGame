-- Module providing basic functionality to scripts

local env = require 'environment'

local this = {}


--[[ Routine creation ]]--
local routines = {}
local total_routines = 0

local ended_scripts = {}

local modules = {}
local modPath = 'scripts/'

function this.readonlytable(table)
  return setmetatable({}, {
    __index = table,
    __newindex = function(table, key, value)
      error("Attempt to modify read-only table. Table index: ".. key .. "(" .. type(key))
    end,
    __metatable = false
   });
end

-- Create environment table for routines
local function createEnv()
  local newEnv = {}

  for i, v in pairs(env) do
    newEnv[i] = v
  end

  newEnv.stage = this.readonlytable(newEnv.stage)
  return newEnv
end

-- Use to add onto a routines environment
local function joinEnv(lhs, rhs)
  for i, v in pairs(rhs) do
    lhs[i] = v
  end
  return lhs
end

-- Build object hierarchy
function this:build_hierarchy(instance)
  return this.readonlytable(env.stage[instance:GetID()])
end

-- Creates a new routine from the given bytecode and and optional environment table
function this:new_routine(file, newEnv)
  local job, jobnum

  local compiled = assert(loadfile(file))
  -- Determine job number
  for i = 1, total_routines do
    if routines[i] == nil then
      jobnum = i
      break
    end
  end

  if jobnum == nil then
    total_routines = total_routines + 1
    jobnum = total_routines
  end

  routines[jobnum] = Script(jobnum, file)

  routines[jobnum].source = compiled
  routines[jobnum].env = createEnv()

  -- Set parent to the given instance hierarchy
  if newEnv ~= nil then
    routines[jobnum].env.parent = newEnv
  end

 setfenv( routines[jobnum].source, routines[jobnum].env)

  routines[jobnum].job = coroutine.create(routines[jobnum].source)

  return routines[jobnum]
end

function this:create_and_run(compiled, env)
  local v =  this:new_routine(compiled, env)
  
  local success, state = coroutine.resume(v.job, v:ElapsedTime())

        -- Deal with errors in scripts
        if not success then 
          this.lua_log_error(state) 
        end
  return v
end

function this:unload_script(script)
  for i = 1, total_routines do
    if script == routines[i] then
      kill_routine()
    end
  end
end

-- Bursts all currently suspended routines
function this:routines_burst(dt)
  for i= 1, total_routines do
    local v = routines[i]

    if v ~= nil and v:IsWaiting() and coroutine.status(v.job) == "suspended" then
      v:TimeTick(dt)

      if not v:IsWaiting() then
        local success, state = coroutine.resume(v.job, v:ElapsedTime())

        -- Deal with errors in scripts
        if not success then 
          this.lua_log_error(state) 
        end
      end
    end
  end

end

function this:kill_routine(id)
-- may need to do more (such as disconnecting events if necessary), but this should work for now
  local active_table
  
  if routines[id.index] ~= nil then
    routines[id.index] = nil
  end
  
  if ended_scripts[id.index] ~= nil then
    ended_scripts[id.index] = nil
  end

 -- collectgarbage('collect')
end

--[[ library extensions ]]--
function lua_wait(time)
  if not time then time = 0 end -- set default wait time

  local thread = assert(coroutine.running(), "Attempting to call wait on the main thread")
  
  local routine

  for i, v in pairs(routines) do
    if v.job == thread then
      routine = v
      break
    end
  end

  assert(routine, "Cannot find thread in routines list. Are you Attempting to call wait in a coroutine?")

  routine:Wait(time)

  return coroutine.yield()

end

function lua_exit()
 
  local thread = assert(coroutine.running(), "Attempting to call wait on the main thread")
  
  local routine
  local index
  
  for i, v in pairs(routines) do
    if v.job == thread then
      routine = v
      index = i
      break
    end
  end
  
  assert(routine, "Cannot find thread in routines list. Are you Attempting to call wait in a coroutine?")
  
  ended_scripts[index] = routine
  routines[index] = nil
  
  return coroutine.yield()
  
end

local function lua_log_conversion(args)
  local printString = ""

  for i, v in ipairs(args) do
    printString = printString .. tostring(v)
  end

  return printString
end

function this.lua_log_info(...)
  return log.info(lua_log_conversion(arg))
end

function this.lua_log_warn(...)
  return log.warn(lua_log_conversion(arg))
end

function this.lua_log_error(...)
  return log.error(lua_log_conversion(arg))
  --return error("wat")
end

function this:lua_getSystem(sys)
  return systems[sys].getSystem()
end

function lua_require_module(modName)
  if modules[modName] == nil then
    local mod = assert(loadfile(modPath .. modName .. '.lua'))

    setfenv(mod, env)

    modules[modName] = mod()
  end

  return modules[modName]
end

do
  env.print = this.lua_log_info
  env.warn = this.lua_log_warn

  env.log = {
    info = this.lua_log_info,
    warn = this.lua_log_warn,
    error = this.lua_log_error,
  }
 
  print = this.lua_log_info

  --print'fetching'
  --env.game = fetchGameState()
  env.game = {}
  env.game.GetSystem = this.lua_getSystem

  env.wait = lua_wait
  env.done = lua_exit
  env.require = lua_require_module

  return this
end
