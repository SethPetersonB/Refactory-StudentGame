
local onFrameSet = parent.TextureFrameSet:Connect(
  function(frame)
    parent.UIFrame.frame = frame
  end)

local onColorSet = parent.ColorSet:Connect(
  function(color)
    parent.UIFrame.color = color
  end)
  
local onRotSet = parent.RotationSet:Connect(
  function(rot)
    parent.UIFrame.rotation = rot
  end)

local onDepthSet = parent.SpriteDepthSet:Connect(
  function(depth)
    parent.UIFrame.depth = depth
  end)
  
local onDepthChange = parent.SpriteDepthChanged:Connect(
  function(delta)
    parent.UIFrame.depth = parent.UIFrame.depth + depth
  end)
  
local onMouseEnter
local onMouseExit

if parent.MouseEntered ~= nil then
  local COLORSHIFT = Color4(0.25, 0.25, 0.25, 0)

  onMouseEnter = parent.MouseEntered:Connect(
    function()
      parent.UIFrame.color = parent.UIFrame.color - COLORSHIFT
    end)
    
    onMouseExit = parent.MouseExit:Connect(
      function()
        parent.UIFrame.color = parent.UIFrame.color + COLORSHIFT
      end)
end

done()