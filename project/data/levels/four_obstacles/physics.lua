
system:executeluascript("levels/four_obstacles/track_functions.lua")

g_HammerInterval = 360
g_HammerVelocity = 4

g_Hammer     = 0
g_HammerTime = 0
g_HammerOld  = 0

g_KickState = 1

g_KickData = {
  [1] = { histop = 83, lostop =   0, velocity = -150 },
  [2] = { histop = 83, lostop = -83, velocity = -150 },
  [3] = { histop =  0, lostop = -83, velocity =  150 },
  [4] = { histop = 83, lostop =   0, velocity =  150 }
}

g_GateInterval = 240
g_GateVelocity = 75
g_GateState    = 0

g_BlockInterval = 240
g_BlockVecocity = 150
g_BlockState    = -1
g_BlockStep     = 0

g_BlockData = {
  [ 0] = { x =  1.0, y = 0.0, z =  0.0 },
  [ 1] = { x =  0.0, y = 0.0, z = -1.0 },
  [ 2] = { x = -1.0, y = 0.0, z =  0.0 },
  [ 3] = { x = -1.0, y = 0.0, z =  0.0 },
  [ 4] = { x =  0.0, y = 0.0, z = -1.0 },
  [ 5] = { x =  1.0, y = 0.0, z =  0.0 },
  [ 6] = { x =  1.0, y = 0.0, z =  0.0 },
  [ 7] = { x =  0.0, y = 0.0, z =  1.0 },
  [ 8] = { x = -1.0, y = 0.0, z =  0.0 },
  [ 9] = { x = -1.0, y = 0.0, z =  0.0 },
  [10] = { x =  0.0, y = 0.0, z =  1.0 },
  [11] = { x =  1.0, y = 0.0, z =  0.0 }
}

function onstep(a_stepno)
  local l_State = -1
  
  -- Handle Obstacle One: Hammer
  local l_RotationHammer = physics:getrotation(23331)
  
  
  if g_Hammer == 0 then
    if l_RotationHammer["x"] <= -90 then
      physics:startmotor(23331, 0, 500)
      physics:setangularvel(23331, { x = 0.0, y = 0.0, z = 0.0 })
    elseif g_HammerOld >= 0 and l_RotationHammer["x"] < 0 then
      physics:sendluamessage(0, 0, "")
      g_HammerOld = -1
    end
    
    if a_stepno - g_HammerTime > g_HammerInterval then
      g_HammerTime = a_stepno
      physics:startmotor(23331, -g_HammerVelocity, 500)
      g_Hammer = 1
      physics:sendluamessage(0, 1, "")
    end
  elseif g_Hammer == 1 then
    if l_RotationHammer["x"] >= 90 then
      physics:startmotor(23331, 0, 500)
      physics:setangularvel(23331, { x = 0.0, y = 0.0, z = 0.0 })
    elseif g_HammerOld <= 0 and l_RotationHammer["x"] > 0 then
      physics:sendluamessage(0, 2, "")
      g_HammerOld = 1
    end
    
    if a_stepno - g_HammerTime > g_HammerInterval then
      g_HammerTime = a_stepno
      physics:startmotor(23331, g_HammerVelocity, 500)
      physics:sendluamessage(0, 3, "")
      g_Hammer = 0
    end
  end
  
  l_State = getKickerState(a_stepno) + 1
  
  -- Handle Obstacle Two: The Kicker
  if l_State ~= g_KickState then
    g_KickState = l_State
    
    physics:sethistop(23526, g_KickData[g_KickState]["histop"])
    physics:setlostop(23526, g_KickData[g_KickState]["lostop"])
    
    physics:startmotor(23526, g_KickData[g_KickState]["velocity"], 500)
    
    physics:sendluamessage(3, g_KickState, "")
  end
  
  l_State = getGateState(a_stepno)
  
  -- Handle Obstacle three: the Gate
  if l_State ~= g_GateState then
    g_GateState = l_State
    
    if g_GateState == 0 then
      physics:sethistop(23420, 71.0)
      physics:startmotor(23420, g_GateVelocity, 500)
      
      physics:sendluamessage(2, 0, "")
    elseif g_GateState == 1 then
      physics:setlostop(23420, 35.5)
      physics:startmotor(23420, -g_GateVelocity, 500)
      
      physics:sendluamessage(2, 1, "")
    elseif g_GateState == 2 then
      physics:setlostop(23420, 0.0)
      physics:startmotor(23420, -g_GateVelocity, 500)
      
      physics:sendluamessage(2, 2, "")
    elseif g_GateState == 3 then
      physics:sethistop(23420, 35.0)
      physics:startmotor(23420, g_GateVelocity, 500)
      
      physics:sendluamessage(2, 3, "")
    end
  end
  
  if a_stepno > 240 then
    l_State = getBlockState(a_stepno - 240)
    
    -- Handle obstacle four: the Blocker
    if g_BlockState ~= l_State then
      g_BlockState = l_State
      
      physics:sendluamessage(1, g_BlockState, "")
      physics:setjointaxis(23473, g_BlockData[g_BlockState])
    end
  end
end