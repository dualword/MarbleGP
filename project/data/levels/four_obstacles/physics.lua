
g_NextActionKnock = 60
g_NextIndexKnock  = 1
g_ActionTypeKnock = 0

g_NextActionBlock = 90
g_NextIndexBlock  = 1
g_ActionTypeBlock = 0

g_NextActionDoor = 120
g_ActionTypeDoor = 0

g_JointIDsKnock   = {
  [1] = 23884,
  [2] = 23880,
  [3] = 23877
}

g_JointIDsDoor = {
  [1] = 23926,
  [2] = 23923
}

g_JointIDsBlock = {
  [1] = 23887,
  [2] = 23949,
  [3] = 23953
}

g_MarbleIDs = {
  [1] = 23011,
  [2] = 23437,
  [3] = 23439,
  [4] = 23441,
  [5] = 23866,
  [6] = 23868,
  [7] = 23870,
  [8] = 23874
}

g_GuardIDs = {
  [1] = 23943,
  [2] = 23945
}

g_Guards       = { }
g_Marbles      = { }
g_ObjectsKnock = { }
g_ObjectsDoor  = { }
g_ObjectsBlock = { }
g_Trigger      = { }

g_NextBlocker = 1
g_StepNo      = -1

function initialize()
  for k,v in pairs(g_JointIDsKnock) do
    local l_Object = LuaPhysicsObject:new()
    l_Object:initialize(v)
    g_ObjectsKnock[k] = l_Object
  end
  
  for k,v in pairs(g_JointIDsBlock) do
    local l_Object = LuaPhysicsObject:new()
    l_Object:initialize(v)
    g_ObjectsBlock[k] = l_Object
  end
  
  for k,v in pairs(g_JointIDsDoor) do
    local l_Object = LuaPhysicsObject:new()
    l_Object:initialize(v)
    g_ObjectsDoor[k] = l_Object
  end
  
  for k,v in pairs(g_MarbleIDs) do
    g_Trigger[v] = { }
    g_Trigger[v]["step"] = -1
    
    local l_Object = LuaPhysicsObject:new()
    l_Object:initialize(v)
    g_Marbles[v] = l_Object
  end
  
  for k,v in pairs(g_GuardIDs) do
    local l_Object = LuaPhysicsObject:new()
    l_Object:initialize(v)
    g_Guards[k] = { }
    g_Guards[k]["target"] = nil
    g_Guards[k]["guard" ] = l_Object
  end
end

function onstep(a_Step)
  g_StepNo = a_Step
  
  if a_Step == 1 then
    for k,v in pairs(g_ObjectsBlock) do
      v:startmotor(1500, 15)
    end
  end
  
  if a_Step > g_NextActionKnock then
    physics:sendmessage("1;" .. tostring(g_NextIndexKnock))
    if g_ActionTypeKnock == 0 then
      g_ObjectsKnock[g_NextIndexKnock]:startmotor(1000, 150)
    else
      g_ObjectsKnock[g_NextIndexKnock]:startmotor(1000, -150)
    end
    
    g_NextIndexKnock = g_NextIndexKnock + 1
    if g_NextIndexKnock > 3 then
      g_NextIndexKnock = 1
      
      if g_ActionTypeKnock == 0 then
        g_ActionTypeKnock = 1
      else
        g_ActionTypeKnock = 0
      end
    end
    
    g_NextActionKnock = a_Step + 120
  end
  
  if a_Step > g_NextActionBlock then
    if g_ActionTypeBlock == 0 then
      g_NextBlocker = g_NextBlocker + 1
      
      if g_NextBlocker > 3 then
        g_NextBlocker = 1
      end
      
      physics:sendmessage("2;" .. tostring(g_NextBlocker))
      g_ObjectsBlock[g_NextBlocker]:startmotor(1500, -50)
      g_ActionTypeBlock = 1
      g_NextActionBlock = a_Step + 90
    else
      physics:sendmessage("3;" .. tostring(g_NextBlocker))
      g_ObjectsBlock[g_NextBlocker]:startmotor(1500, 50)
      g_ActionTypeBlock = 0
      g_NextActionBlock = a_Step + 90
    end
  end
  
  if a_Step > g_NextActionDoor then
    if g_ActionTypeDoor == 0 then
      physics:sendmessage("4")
      for k,v in pairs(g_ObjectsDoor) do
        v:startmotor(2500, 15)
      end
      g_ActionTypeDoor = 1
    else
      physics:sendmessage("5")
      for k,v in pairs(g_ObjectsDoor) do
        v:startmotor(2500, -15)
      end
      g_ActionTypeDoor = 0
    end
    
    g_NextActionDoor = a_Step + 180
  end
  
  for k,v in pairs(g_Trigger) do
    if v["step"] ~= -1 then
      if v["step"] < a_Step - 4 then
        g_Trigger[k]["step"] = -1
      end
    end
  end
  
  for k,v in pairs(g_Guards) do
    if v["target"] ~= nil then
      local l_PosT = v["target"]:getposition ()
      local l_PosG = v["guard" ]:getposition ()
      local l_VelT = v["target"]:getlinearvel()
      local l_Torq = { }
      
      l_PosT["x"] = l_PosT["x"] + 0.1 * l_VelT["x"]
      l_PosT["z"] = l_PosT["z"] + 0.1 * l_VelT["z"]
      
      l_Torq["x"] = l_PosG["x"] - l_PosT["x"]
      l_Torq["z"] = l_PosG["z"] - l_PosT["z"]
      
      local l_Length = math.sqrt(l_Torq["x"] * l_Torq["x"] + l_Torq["z"] * l_Torq["z"])
      
      l_Torq["x"] = l_Torq["x"] / l_Length
      l_Torq["z"] = l_Torq["z"] / l_Length
      
      -- v["guard"]:addforce(-2500.0 * l_Torq["x"], 0.0, -2500.0 * l_Torq["z"])
      v["guard"]:setlinearvel(-50.0 * l_Torq["x"], 0.0, -50.0 * l_Torq["z"])
      
      if v["step"] < a_Step - 4 then
        v["target"] = nil
      end
    end
    
    local l_Pos    = v["guard"]:getposition ()
    local l_NewVel = v["guard"]:getlinearvel()
    local l_Out    = false
    
    if l_Pos["x"] > -260 then
      io.write("-260\n")
      l_Pos   ["x"] = -270
      l_NewVel["x"] = -l_NewVel["x"] / 3.0
      l_Out         = true
    end
    
    if l_Pos["x"] < -370 then
      io.write("-370\n")
      l_Pos   ["x"] = -360
      l_NewVel["x"] = -l_NewVel["x"] / 3.0
      l_Out         = true
    end
    
    if l_Pos["z"] > 140 then
      l_Pos   ["z"] = 130
      l_NewVel["z"] = -l_NewVel["z"] / 3.0
      l_Out         = true
    end
    
    if l_Pos["z"] < -90 then
      l_Pos   ["z"] = -80
      l_NewVel["z"] = -l_NewVel["z"] / 3.0
      l_Out         = true
    end
    
    if l_Out then
      v["guard"]:setposition(l_Pos["x"], l_Pos["y"], l_Pos["z"])
      v["guard"]:setlinearvel(l_NewVel["x"], 0.0, l_NewVel["z"])
    end
  end
end

function triggercallback(a_Object, a_Trigger, a_Position)
  if a_Trigger == 99 then
    if g_Trigger[a_Object] ~= nil then
      g_Trigger[a_Object]["step"] = g_StepNo
      
      for k,v in pairs(g_Guards) do
        if v["target"] == g_Marbles[a_Object] then
          return
        end
      end
      
      for k,v in pairs(g_Guards) do
        if v["target"] == nil then
          v["target"] = g_Marbles[a_Object]
          v["step"  ] = g_StepNo
          return
        end
      end
    end
  end
end