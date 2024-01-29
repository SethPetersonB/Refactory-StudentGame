--local this = {}


local function mod()
  local totalCalls = 0

  local this = {}

  function this:call()
    totalCalls = totalCalls + 1
    return math.sin(totalCalls)
  end

  this.stuff = 34

  print 'Sandbox environment module test script loaded'

  return this
end

return mod
