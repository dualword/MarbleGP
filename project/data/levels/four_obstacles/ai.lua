-- AI script for track "Moving: Four Obstacles"

system:executeluascript("levels/four_obstacles/track_functions.lua")

math.randomseed(os.time())

g_Actions = {
  [1] = 0,
  [2] = 0,  -- 23124, 23127, 23129
  [3] = 0   -- 23133, 23136, 23138
}

g_Step = 3

function onstep(a_Step)
  g_Step = a_Step

  if getGateStep(a_Step) == 0 then
    if g_Actions[1] == 0 then
      g_Actions[1] = 1
    else
      g_Actions[1] = 0
    end
  end
  
  if getBlockStep(a_Step, 1) == 0 then
    if g_Actions[2] == 0 then
      g_Actions[2] = 1
    elseif g_Actions[2] == 1 then
      g_Actions[2] = 2
    elseif g_Actions[2] == 2 then
      g_Actions[2] = 3
    elseif g_Actions[2] == 3 then
      g_Actions[2] = 4
    elseif g_Actions[2] == 4 then
      g_Actions[2] = 5
    elseif g_Actions[2] == 5 then
      g_Actions[2] = 0
    end
  end
  
  if getCrushStep(a_Step, 1) == 0 then
    if g_Actions[3] == 0 then
      g_Actions[3] = 1
    elseif g_Actions[3] == 1 then
      g_Actions[3] = 2
    elseif g_Actions[3] == 2 then
      g_Actions[3] = 0
    end
  end
end

function decide_blocker(a_MarbleId, a_Blocker)
  local l_Ret = true
  
  if a_Blocker == 10 then
    l_Ret = getBlockStep(g_Step, 1) < 420
  elseif a_Blocker == 11 then
    l_Ret = g_Actions[3] ~= 1
  elseif a_Blocker == 12 then
    l_Ret = g_Actions[3] ~= 0
  elseif a_Blocker == 13 then
    l_Ret = g_Actions[3] ~= 2
  elseif a_Blocker == 14 then
    l_Ret = g_Actions[1] ~= 1
  elseif a_Blocker == 15 then
    l_Ret = g_Actions[1] ~= 0
  end
  
  return l_Ret
end

g_Side = -1

function decide_roadsplit(a_MarbleId, a_Split)
  local l_Ret = -1
  
  if a_Split == 79 then
    g_Side = -1
    if g_Actions[3] == 0 then
      l_Ret = 2
    elseif g_Actions[3] == 1 then
      l_Ret = 0
    elseif g_Actions[3] == 2 then
      l_Ret = 1
    end
  elseif a_Split == 100 then
    if g_Actions[1] == 0 then
      if getGateStep(g_Step) < 520 then
        if g_Side == -1 then
          g_Side = math.random(1, 2)
        end
        l_Ret = g_Side
      else
        l_Ret = 0
      end
    elseif g_Actions[1] == 1 then
      if getGateStep(g_Step) < 520 then
        l_Ret = 0
      else
        if g_Side == -1 then
          g_Side = math.random(1, 2)
        end
        l_Ret = g_Side
      end
    end
  end
  
  return l_Ret
end

