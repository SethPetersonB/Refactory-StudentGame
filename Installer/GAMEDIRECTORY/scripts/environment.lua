-- This file contains a table of all functions that are allowed to be ran in the sandbox environment
local env = {
  -- Allowed coroutine library functions
  coroutine = {
    create = coroutine.create,
    resume = coroutine.resume,
    status = coroutine.status,
    wrap = coroutine.wrap,
    running = coroutine.running,
    yield = coroutine.yield
  },
  
  -- Allowed string library funcitons
  string = {
    byte = string.byte,
    char = string.char,
    find = string.find,
    format = string.format,
    gmatch = string.gmatch,
    gsub = string.gsub,
    len = string.len,
    lower = string.lower,
    match = string.match,
    rep = string.rep, -- Might be unsafe, 
    reverse = string.reverse,
    sub = string.sub,
    upper = string.upper
  },

  -- Allowed table library functions
  table = {
    insert = table.insert,
    maxn = table.maxn,
    remove = table.remove,
    sort = table.sort
  },

  -- Allowed math library functions
  math = {
    abs = math.abs,
    acos = math.acos,
    asin = math.asin,
    atan = math.atan,
    atan2 = math.atan2,
    ceil = math.ceil,
    cos = math.cos,
    cosh = math.cosh,
    deg = math.deg,
    exp = math.exp,
    floor = math.floor,
    fmod = math.fmod,
    frexp = math.frexp,
    huge = math.huge,
    ldexp = math.ldexp,
    log = math.log,
    log10 = math.log10,
    max = math.max,
    min = math.min,
    modf = math.modf,
    pi = math.pi,
    pow = math.pow,
    rad = math.rad,
    random = math.random,
    sin = math.sin,
    sinh = math.sinh,
    sqrt = math.sqrt,
    tan = math.tan,
    tanh = math.tanh,

    -- Custom
    random_normal = math.random_normal
  },
  
  -- Allowed os library functions
  os = {
    clock = os.clock,
    difftime = os.difftime,
    time = os.time
  },

  -- Allowed basic functions
  assert = assert,
  error = error,
  ipairs = ipairs,
  next = next,
  pairs = pairs,
  pcall = pcall,
  --print = print, -- Redefined elsewhere
  select = select,
  tonumber = tonumber, 
  tostring = tostring,
  type = type,
  unpack = unpack,
  _VERSION = _VERSION,
  xpcall = xpcall,
  

  --[[ Add userdata here ]]--
  --fetchGameState = fetchGameState
  Enum = {
    KeyCode = systems.InputSystem.KeyCode
  },

  Vector2 = Vector2,
  Color4 = Vector4,
  Vector4 = Vector4
  }

return env
