g_GateAction = 1

system:executeluascript("levels/four_obstacles/track_functions.lua")

function onstep(a_stepno)
  local l_Block = getBlockStep(a_stepno - 60, 3)
  
  scene:setposition(23287, { x = 0.0, y = -l_Block / 1440.0, z = 0.0 })
  
  local l_Crush = getCrushStep(a_stepno - 360, 3)
  
  scene:setposition(24287, { x = -l_Crush / 1080.0, y = 0.0, z = 0.0 })
  
  local l_Gate = getGateStep(a_stepno)
  
  if l_Gate == 0 then
    if g_GateAction == 0 then
      g_GateAction = 1
    else
      g_GateAction = 0
    end
  end
  
  local l_Angle = -180.0 * (l_Gate / 600)
  
  if g_GateAction == 1 then
    l_Angle = 180.0 - l_Angle
  end
  
  scene:setrotation(24284, { x = 0.0, y = 0.0, z = 180.0 - l_Angle })
  scene:setrotation(24285, { x = 0.0, y = 0.0, z = l_Angle })
end
