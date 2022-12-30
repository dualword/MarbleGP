g_Route = -1

function marbleclass(a_Class)
  g_MarbleClass = a_Class
  io.write("Marbleclass: " .. tostring(g_MarbleClass) .. "\n")
end

-- 0 == straight, 1 == left, 2 == right
function decide_roadsplit(a_MarbleId, a_Split)
  if g_Route == -1 then
    local l_Dice = math.random(100)
    
    if g_MarbleClass == 0 then
      -- MarbleGP
      if l_Dice < 15 then
        g_Route = 0
      else
        g_Route = 1
      end
    elseif g_MarbleClass == 1 then
      -- Marble2
      if l_Dice < 10 then
        g_Route = 2
      elseif l_Dice < 50 then
        g_Route = 1
      else
        g_Route = 0
      end
    elseif g_MarbleClass == 2 then
      -- Marble3
      if l_Dice < 10 then
        g_Route = 1
      elseif l_Dice < 40 then
        g_Route = 0
      else
        g_Route = 2
      end
    end
  end
    
  return g_Route
end

function oncheckpoint(a_MarbleId, a_Checkpoint)
  g_Route = -1
end