local onMouseEnter = parent.MouseEntered:Connect(
  function()
    parent.Animator.paused = true
  end)

  local onMouseExit = parent.MouseExit:Connect(
  function()
    parent.Animator.paused = false
  end)

  local onMouseExit = parent.Clicked:Connect(
  function()
    parent.Animator:Advance()
  end)

  --while wait() do end
  done()