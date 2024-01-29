function module()
  local input = game:GetSystem("InputSystem")

  local this = {}

  local fadeSpeed = 0.125 -- How long it takes the object to fade out (in seconds)

  function this:CheckHide(instance, key)
    while wait() do
      if input:KeyPressed(key) then

        local fadeStart = instance.UIFrame.color.a
        local fadeAmount = fadeStart / fadeSpeed

        -- Fade out each frame until fully transparent
        repeat
          local dt = wait()
          local color = instance.UIFrame.color

          instance.UIFrame.color = color - Color4(0, 0, 0, dt * fadeAmount)

        until instance.UIFrame.color.a <= 0

        instance.UIFrame.visible = false
        break
      end
    end
  end


  return this
end

return module