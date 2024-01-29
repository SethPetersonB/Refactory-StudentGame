
local onFrameSet = parent.TextureFrameSet:Connect(
  function(frame)
    Sprite.frame = frame
  end)

local onColorSet = parent.ColorSet:Connect(
  function(color)
    parent.Sprite.color = color
  end)
  
local onRotSet = parent.RotationSet:Connect(
  function(rot)
    parent.Sprite.rotation = rot
  end)

local onDepthSet = parent.SpriteDepthSet:Connect(
  function(depth)
    parent.Sprite.depth = depth
  end)
  
local onDepthChange = parent.SpriteDepthChanged:Connect(
  function(delta)
    parent.Sprite.depth = parent.Sprite.depth + depth
  end)
  
local onMouseEnter
local onMouseExit

if parent.MouseEntered ~= nil then
  local COLORSHIFT = Color4(0.25, 0.25, 0.25, 0)

  onMouseEnter = parent.MouseEntered:Connect(
    function()
      parent.Sprite.color = parent.Sprite.color - COLORSHIFT
    end)
    
    onMouseExit = parent.MouseExit:Connect(
      function()
        parent.Sprite.color = parent.Sprite.color + COLORSHIFT
      end)
end

done()