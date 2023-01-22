g_Blockers = {
  [11] = true
}

function onluamessage(a_NumberOne, a_NumberTwo, a_Data)
  if a_NumberOne == 0 then
    if a_NumberTwo == 0 then
    elseif a_NumberTwo == 1 then
    elseif a_NumberTwo == 2 then
    elseif a_NumberTwo == 3 then
    end
  elseif a_NumberOne == 1 then
    g_Blockers[11] = (a_NumberTwo % 3 == 1)
    io.write(tostring(a_NumberTwo % 3) .. "\n")
  end
end

function decide_blocker(a_MarbleId, a_Blocker)
  -- io.write("Blocker: " .. tostring(a_Blocker) .. "\n")
  if a_Blocker == 11 then
  
    return g_Blockers[11]
  else
    
  end
end
