
system:executeluascript("levels/four_obstacles/track_functions.lua")

g_BlockRotation = {
  [ 0] = 270,
  [ 1] = 30,
  [ 2] = 150,
  [ 3] = 270,
  [ 4] = 30,
  [ 5] = 150,
  [ 6] = 270,
  [ 7] = 30,
  [ 8] = 150,
  [ 9] = 270,
  [10] = 30,
  [11] = 150
}

g_GateAngle = 0.0
g_GateDir   = -0.375

g_BlockAngle = -90.0
g_BlockDir   = -0.25

function onluamessage(a_NumberOne, a_NumberTwo, a_Data)
  if a_NumberOne == 0 then
    if a_NumberTwo == 0 then
      scene:setrotation(23601, { x = 0.0, y = 180.0, z = 0.0 })
      scene:setrotation(23579, { x = 0.0, y =  60.0, z = 0.0 })
    elseif a_NumberTwo == 1 then
      scene:setrotation(23601, { x = 0.0, y = 180.0, z = 0.0 })
      scene:setrotation(23579, { x = 0.0, y = 300.0, z = 0.0 })
    elseif a_NumberTwo == 2 then
      scene:setrotation(23601, { x = 0.0, y =  60.0, z = 0.0 })
      scene:setrotation(23579, { x = 0.0, y = 180.0, z = 0.0 })
    elseif a_NumberTwo == 3 then
      scene:setrotation(23601, { x = 0.0, y = 300.0, z = 0.0 })
      scene:setrotation(23579, { x = 0.0, y = 180.0, z = 0.0 })
    end
  elseif a_NumberOne == 1 then
    if g_BlockRotation[a_NumberTwo] ~= nil then
      scene:setrotation(23623, { x = 0.0, y = g_BlockRotation[a_NumberTwo], z = 0.0 })
    else
      io.write("No rotation for block state #" .. tostring(a_NumberTwo) .. "\n")
    end
  end
end

function onstep(a_stepno)
  local l_BlockPosition = scene:getposition(23504)
  local l_WarnPosition  = scene:getposition(23623)
  
  l_WarnPosition["z"] = l_BlockPosition["z"]
  
  scene:setposition(23623, l_WarnPosition)
  
  local l_State = getGateState(a_stepno)
  local l_Steps = getGateTime (a_stepno)
  
  if l_State == 0 then
    g_GateAngle = 180.0 + l_Steps * 90.0 / g_GateTime
  elseif l_State == 1 then
    g_GateAngle = 180.0 + (90 + (l_Steps * 90.0 / g_GateTime))
  elseif l_State == 2 then
    g_GateAngle = 180.0 + (180 - (l_Steps * 90.0 / g_GateTime))
  elseif l_State == 3 then
    g_GateAngle = 180.0 + (90 - (l_Steps * 90.0 / g_GateTime))
  end  
  
  scene:setrotation(23648, { x = 0.0, y = 0.0, z = g_GateAngle })
  
  g_BlockAngle = g_BlockAngle + g_BlockDir
  
  l_State = getKickerState(a_stepno)
  l_Steps = getKickerTime (a_stepno)
  
  if l_State == 0 then
    g_BlockAngle = 180.0 + l_Steps * 90.0 / g_KickerTime
  elseif l_State == 1 then
    g_BlockAngle = 180.0 + (90 + (l_Steps * 90.0 / g_KickerTime))
  elseif l_State == 2 then
    g_BlockAngle = 180.0 + (180 - (l_Steps * 90.0 / g_KickerTime))
  elseif l_State == 3 then
    g_BlockAngle = 180.0 + (90 - (l_Steps * 90.0 / g_KickerTime))
  end
  
  scene:setrotation(23672, { x = 0.0, y = 0.0, z = g_BlockAngle })
end
