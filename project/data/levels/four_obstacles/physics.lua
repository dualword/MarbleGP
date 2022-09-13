
system:executeluascript("levels/four_obstacles/track_functions.lua")

g_Actions = {
  [1] = 0,
  [2] = 0,  -- 23124, 23127, 23129
  [3] = 0   -- 23133, 23136, 23138
}

function onstep(a_stepno)
  if getGateStep(a_stepno) == 0 then
    if g_Actions[1] == 0 then
      physics:startmotor(23092, 25, 150)
      physics:startmotor(23121, 25, 150)
      
      g_Actions[1] = 1
    else
      physics:startmotor(23092, -25, 150)
      physics:startmotor(23121, -25, 150)
      
      g_Actions[1] = 0
    end
  end
  
  if getBlockStep(a_stepno, 1) == 0 then
    if g_Actions[2] == 0 then
      physics:startmotor(23124, -50, 150)
      g_Actions[2] = 1
    elseif g_Actions[2] == 1 then
      physics:startmotor(23127, -50, 150)
      g_Actions[2] = 2
    elseif g_Actions[2] == 2 then
      physics:startmotor(23129, -50, 150)
      g_Actions[2] = 3
    elseif g_Actions[2] == 3 then
      physics:startmotor(23124, 50, 150)
      g_Actions[2] = 4
    elseif g_Actions[2] == 4 then
      physics:startmotor(23127, 50, 150)
      g_Actions[2] = 5
    elseif g_Actions[2] == 5 then
      physics:startmotor(23129, 50, 150)
      g_Actions[2] = 0
    end
  end
  
  if getCrushStep(a_stepno, 1) == 0 then
    io.write(tostring(g_Actions[3]))
    if g_Actions[3] == 0 then
      physics:startmotor(23133, -25, 750)
      physics:startmotor(23138,  25, 750)
      g_Actions[3] = 1
    elseif g_Actions[3] == 1 then
      physics:startmotor(23136, -25, 750)
      physics:startmotor(23133,  25, 750)
      g_Actions[3] = 2
    elseif g_Actions[3] == 2 then
      physics:startmotor(23138, -25, 750)
      physics:startmotor(23136,  25, 750)
      g_Actions[3] = 0
    end
  end
end