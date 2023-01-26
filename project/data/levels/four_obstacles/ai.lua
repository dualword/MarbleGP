g_Blockers = {
  [1] = true,
  [2] = true,
  [3] = true,
  [4] = true,
  [5] = true
}

g_Splits = {
  [34] = 1
}

g_BlockState = 1
g_BlockInc   = 1
g_KickState  = 1
g_GateState  = 0

function onluamessage(a_NumberOne, a_NumberTwo, a_Data)
  -- io.write("onluamessage(" .. tostring(a_NumberOne) .. ", " .. tostring(a_NumberTwo) .. ", \"" .. a_Data .. "\")\n")
  if a_NumberOne == 0 then
    io.write("onluamessage(" .. tostring(a_NumberOne) .. ", " .. tostring(a_NumberTwo) .. ", \"" .. a_Data .. "\")\n")
    if a_NumberTwo == 1 then
      g_Splits[34] = 0
      
      g_Blockers[3] = true
      g_Blockers[4] = true
    elseif a_NumberTwo == 3 then
      g_Splits[34] = 1
      
      g_Blockers[3] = true
      g_Blockers[4] = true
    elseif a_NumberTwo == 0 then
      g_Blockers[3] = false
    else
      g_Blockers[4] = false
    end
  elseif a_NumberOne == 1 then
    if a_NumberTwo % 3 == 0 then
      if g_BlockInc == 1 and g_BlockState == 3 then
        g_BlockInc = -1
      elseif g_BlockInc == -1 and g_BlockState == 1 then
        g_BlockInc = 1
      end
      
      g_BlockState = g_BlockState + g_BlockInc
    end
    
    for i = 1, 3 do
      if g_BlockState == i then
        g_Blockers[i] = (a_NumberTwo % 3) ~= 0
      else
        g_Blockers[i] = true
      end
    end
  elseif a_NumberOne == 2 then
    -- io.write("onluamessage(" .. tostring(a_NumberOne) .. ", " .. tostring(a_NumberTwo) .. ", \"" .. a_Data .. "\")\n")
    g_GateState = a_NumberTwo
  elseif a_NumberOne == 3 then
    g_KickState = a_NumberTwo
  end
end

function decide_blocker(a_MarbleId, a_Blocker)
  if a_Blocker == 11 then
    return g_Blockers[1]
  elseif a_Blocker == 12 then
    return g_Blockers[2]
  elseif a_Blocker == 13 then
    return g_Blockers[3]
  elseif a_Blocker == 93 then
    return g_Blockers[4]
  elseif a_Blocker == 94 then
    return g_Blockers[3]
  else
    
  end
  
  return true
end

function decide_roadsplit(a_MarbleId, a_Split)
  if a_Split == 34 then
    return g_Splits[34]
  elseif a_Split == 46 then
    -- 0 == right, 1 == left
    if g_KickState == 1 or g_KickState == 2 then
      return 0
    else
      return 1
    end
  elseif a_Split == 69 then
    -- 0 == middle, 1 == left, 2 == right
    if g_GateState == 3 or g_GateState == 0 then
      -- io.write("Gate: left " .. tostring(g_GateState) .. "\n")
      return 1
    else
      -- io.write("Gate: right " .. tostring(g_GateState) .. "\n")
      return 2
    end
  else
    -- io.write("decide_roadsplit(" .. tostring(a_MarbleId) .. ", " .. tostring(a_Split) .. ")\n")
  end
end