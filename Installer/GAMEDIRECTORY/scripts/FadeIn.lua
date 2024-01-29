parent.Sprite.visible = false -- Prevents flickering caused by wait below
wait() -- Wrong color if no wait
parent.Sprite.visible = true

local goal = parent.Sprite.color
parent.Sprite.color = Color4(goal.r, goal.g, goal.b, 0)

local fadeTime = math.random(125, 500) / 1000

local fadeAmount = goal.a / fadeTime

repeat
  local dt = wait()
  local color = parent.Sprite.color

  parent.Sprite.color = color + Color4(0, 0, 0, dt * fadeAmount)

until parent.Sprite.color.a >= goal.a

parent.Sprite.color = goal
