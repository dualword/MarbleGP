system:executeluascript("levels/the_gates/track_functions.lua")

function decide_blocker(a_MarbleId, a_Blocker)
  local l_Ret = true
  
  -- Left block == 10, Right Block == 11
  if a_Blocker == 10 then
    l_Ret = g_Warnings[2]["targety"] ~= 60
  elseif a_Blocker == 11 then
    l_Ret = g_Warnings[1]["targety"] ~= 60
  end
  
  if not l_Ret then
    io.write("Blocker " .. tostring(a_Blocker) .. " blocked (".. tostring(g_Step) .. ").\n")
  end
  
  return l_Ret
end

function decide_roadsplit(a_MarbleId, a_Split)
  local l_Ret = -1
  
  if a_Split == 23 then
    -- 0 == right, 1 == left
    if g_Warnings[1]["last_checkpoint"] < g_Warnings[2]["last_checkpoint"] then
      l_Ret = 0
    else
      l_Ret = 1
    end
  end
  
  return l_Ret
end

function onstep(a_Step)
  g_Step = a_Step
end
