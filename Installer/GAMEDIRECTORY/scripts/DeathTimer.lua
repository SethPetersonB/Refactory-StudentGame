
while wait() do
  if parent.Transform.depth <= -1 then
    stage:RemoveInstance(parent)
    break;
  end
end
