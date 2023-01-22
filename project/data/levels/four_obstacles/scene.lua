
g_BlockRotation = {
  [ 1] = 270,
  [ 2] = 30,
  [ 3] = 150,
  [ 4] = 270,
  [ 5] = 30,
  [ 6] = 150,
  [ 7] = 270,
  [ 8] = 30,
  [ 9] = 150,
  [10] = 270,
  [11] = 30,
  [12] = 150
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
  
  g_GateAngle = g_GateAngle + g_GateDir
  
  if g_GateAngle <= -180.0 then
    g_GateDir = -g_GateDir
    g_GateAngle = -180.0
  elseif g_GateAngle >= 0.0 then
    g_GateDir = -g_GateDir 
    g_GateAngle = 0.0
  end
  
  scene:setrotation(23648, { x = 0.0, y = 0.0, z = g_GateAngle })
  
  g_BlockAngle = g_BlockAngle + g_BlockDir
  
  if g_BlockAngle <= -180.0 then
    g_BlockDir = -g_BlockDir 
    g_BlockAngle = -180.0
  elseif g_BlockAngle >= 0.0 then
    g_BlockDir = -g_BlockDir 
    g_BlockAngle = 0.0
  end
  
  scene:setrotation(23672, { x = 0.0, y = 0.0, z = g_BlockAngle })
end
