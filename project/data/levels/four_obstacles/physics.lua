
system:executeluascript("levels/four_obstacles/track_functions.lua")

g_HammerInterval = 360
g_HammerVelocity = 4

g_Hammer     = 0
g_HammerTime = 0

g_SlapInterval = 180
g_SlepValocity = 5

g_SlapStep = 0
g_SlapTime = 0

g_GateInterval = 240
g_GateVelocity = 75
g_GateState    = 0
g_GateStep     = 0

g_BlockInterval = 120
g_BlockVecocity = 150
g_BlockState    = 1
g_BlockStep     = 0

g_BlockData = {
  [ 1] = { x =  1.0, y = 0.0, z =  0.0 },
  [ 2] = { x =  0.0, y = 0.0, z = -1.0 },
  [ 3] = { x = -1.0, y = 0.0, z =  0.0 },
  [ 4] = { x = -1.0, y = 0.0, z =  0.0 },
  [ 5] = { x =  0.0, y = 0.0, z = -1.0 },
  [ 6] = { x =  1.0, y = 0.0, z =  0.0 },
  [ 7] = { x =  1.0, y = 0.0, z =  0.0 },
  [ 8] = { x =  0.0, y = 0.0, z =  1.0 },
  [ 9] = { x = -1.0, y = 0.0, z =  0.0 },
  [10] = { x = -1.0, y = 0.0, z =  0.0 },
  [11] = { x =  0.0, y = 0.0, z =  1.0 },
  [12] = { x =  1.0, y = 0.0, z =  0.0 }
}

function checkSlappers()
  local l_RotationSlapOne = physics:getrotation(23384)
  local l_RotationSlapTwo = physics:getrotation(23387)
  
  if g_SlapStep == 0 or g_SlapStep >= 2 then
    if l_RotationSlapOne["y"] >= 87 then
      physics:startmotor(23384, 0, 50)
      physics:setangularvel(23384, { x = 0.0, y = 0.0, z = 0.0 })
    end
  else
    if l_RotationSlapOne["y"] <= -87 then
      physics:startmotor(23384, 0, 50)
      physics:setangularvel(23384, { x = 0.0, y = 0.0, z = 0.0 })
    end
  end

  if g_SlapStep == 3 then
    if l_RotationSlapTwo["y"] >= 87 then
      physics:startmotor(23387, 0, 50)
      physics:setangularvel(23387, { x = 0.0, y = 0.0, z = 0.0 })
    end
  else
    if l_RotationSlapTwo["y"] <= -87 then
      physics:startmotor(23387, 0, 50)
      physics:setangularvel(23387, { x = 0.0, y = 0.0, z = 0.0 })
    end
  end
end

function onstep(a_stepno)
  -- Handle Obstacle One: Hammer
  local l_RotationHammer = physics:getrotation(23331)
  
  if g_Hammer == 0 then
    if l_RotationHammer["x"] <= -90 then
      physics:startmotor(23331, 0, 500)
      physics:setangularvel(23331, { x = 0.0, y = 0.0, z = 0.0 })
    end
    
    if a_stepno - g_HammerTime > g_HammerInterval then
      g_HammerTime = a_stepno
      physics:startmotor(23331, -g_HammerVelocity, 500)
      g_Hammer = 1
    end
  elseif g_Hammer == 1 then
    if l_RotationHammer["x"] >= 90 then
      physics:startmotor(23331, 0, 500)
      physics:setangularvel(23331, { x = 0.0, y = 0.0, z = 0.0 })
    end
    
    if a_stepno - g_HammerTime > g_HammerInterval then
      g_HammerTime = a_stepno
      physics:startmotor(23331, g_HammerVelocity, 500)
      g_Hammer = 0
    end
  end
  
  -- Handle Obstacle Two: Double-Slap
  checkSlappers()
  
  if g_SlapStep == 0 then
    if a_stepno - g_SlapTime > g_SlapInterval then
      g_SlapTime = a_stepno
      physics:startmotor(23384, -g_SlepValocity, 500)
      g_SlapStep = 1
    end
  elseif g_SlapStep == 1 then
    if a_stepno - g_SlapTime > g_SlapInterval then
      g_SlapTime = a_stepno
      physics:startmotor(23384, g_SlepValocity, 500)
      g_SlapStep = 2
    end
  elseif g_SlapStep == 2 then
    if a_stepno - g_SlapTime > g_SlapInterval then
      g_SlapTime = a_stepno
      physics:startmotor(23387, -g_SlepValocity, 500)
      g_SlapStep = 3
    end
  elseif g_SlapStep == 3 then
    if a_stepno - g_SlapTime > g_SlapInterval then
      g_SlapTime = a_stepno
      physics:startmotor(23387, g_SlepValocity, 500)
      g_SlapStep = 0
    end
  end
  
  -- Handle Obstacle three: the Gate
  if g_GateState == 0 then
    if a_stepno - g_GateStep > g_GateInterval then
      physics:sethistop(23420, 71.0)
      physics:startmotor(23420, g_GateVelocity, 500)
      g_GateStep = a_stepno
      g_GateState = 1
    end
  elseif g_GateState == 1 then
    if a_stepno - g_GateStep > g_GateInterval then
      physics:setlostop(23420, 35.5)
      physics:startmotor(23420, -g_GateVelocity, 500)
      g_GateStep = a_stepno
      g_GateState = 2
    end
  elseif g_GateState == 2 then
    if a_stepno - g_GateStep > g_GateInterval then
      physics:setlostop(23420, 0.0)
      physics:startmotor(23420, -g_GateVelocity, 500)
      g_GateStep = a_stepno
      g_GateState = 3
    end
  elseif g_GateState == 3 then
    if a_stepno - g_GateStep > g_GateInterval then
      physics:sethistop(23420, 35.0)
      physics:startmotor(23420, g_GateVelocity, 500)
      g_GateStep = a_stepno
      g_GateState = 0
    end
  end
  
  -- Handle obstacle four: the Blocker
  if a_stepno - g_BlockStep > g_BlockInterval then
    physics:setjointaxis(23473, g_BlockData[g_BlockState])
    g_BlockStep = a_stepno
    g_BlockState = g_BlockState + 1
    
    if g_BlockData[g_BlockState] == nil then
      g_BlockState = 1
    end
  end
end