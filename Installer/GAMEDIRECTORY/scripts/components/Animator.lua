local onFrameSet = parent.SetAnimationFrame:Connect(
  function(frame)
    parent.Animator.frame = frame
  end)

local onStartSet = parent.SetAnimationStart:Connect(
  function(frame)
    parent.Animator.startFrame = frame
  end)

local frameCountSet = parent.SetAnimationFrameCount:Connect(
  function(count)
    parent.Animator.startFrame = count
  end)

local frameCountSet = parent.SetAnimationHoldTime:Connect(
  function(delay)
    parent.Animator.delay = delay
  end)

local frameCountSet = parent.SetAnimationPaused:Connect(
  function(paused)
    parent.Animator.paused = paused
  end)

local onAdvance = parent.AdvanceAnimation:Connect(
  function()
    parent.Animator:Advance()
  end)

local onReset = parent.ResetAnimation:Connect(
  function()
    parent.Animator:Reset()
  end)

--while wait() do end
done()