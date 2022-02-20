g_SignNamesKick = {
  [1] = "kickwarn1",
  [2] = "kickwarn2",
  [3] = "kickwarn3"
}

g_SignNamesBlock = {
  [1] = "blockwarn1",
  [2] = "blockwarn2",
  [3] = "blockwarn3"
}

g_SignNamesDoor = {
  [1] = "doorwarn1",
  [2] = "doorwarn2",
  [3] = "doorwarn3"
}

g_KickSigns  = { }
g_DoorSigns  = { }
g_BlockSigns = { }

g_KickStart  = 180
g_BlockStart = 0
g_DoorStart  = 270

function split(s, delimiter)
    result = {};
    for match in (s..delimiter):gmatch("(.-)"..delimiter) do
        table.insert(result, match);
    end
    return result;
end

function initialize()
  for k,v in pairs(g_SignNamesKick) do
    local l_Object = LuaSceneObject:new()
    l_Object:initialize(v)
    g_KickSigns[k] = l_Object
  end
  
  for k,v in pairs(g_SignNamesBlock) do
    local l_Object = LuaSceneObject:new()
    l_Object:initialize(v)
    g_BlockSigns[k] = l_Object
  end
  
  for k,v in pairs(g_SignNamesDoor) do
    local l_Object = LuaSceneObject:new()
    l_Object:initialize(v)
    g_DoorSigns[k] = l_Object
  end
  
  io.write("\n\nDoors: " .. tostring(#g_DoorSigns) .. "\n\n\n")
  for k,v in pairs(g_DoorSigns) do
    io.write("  " .. tostring(k) .. ": " .. tostring(v) .. "\n")
  end
end

function onmessage(a_Data)
  local l_Msg = split(a_Data, ";")
  
  if #l_Msg > 0 then
    if l_Msg[1] == "4" then
      g_DoorSigns[2]:setrotation(0.000000, 150.000000, 0.000000)
      g_DoorSigns[1]:setrotation(0.000000, 270.000000, 0.000000)
      g_DoorSigns[3]:setrotation(0.000000, 150.000000, 0.000000)
    elseif l_Msg[1] == "5" then
      g_DoorSigns[2]:setrotation(0.000000, 270.000000, 0.000000)
      g_DoorSigns[1]:setrotation(0.000000, 150.000000, 0.000000)
      g_DoorSigns[3]:setrotation(0.000000, 270.000000, 0.000000)
    elseif l_Msg[1] == "1" then
      for k,v in pairs(g_KickSigns) do
        if tostring(k) == l_Msg[2] then
          v:setrotation(0, g_KickStart - 120.0, 0)
        else
          v:setrotation(0, g_KickStart, 0)
        end
      end
    elseif l_Msg[1] == "2" then
      for k,v in pairs(g_BlockSigns) do
        if tostring(k) == l_Msg[2] then
          v:setrotation(0, g_BlockStart - 120.0, 0)
        else
          v:setrotation(0, g_BlockStart, 0)
        end
      end
    elseif l_Msg[1] == "3" then
      for k,v in pairs(g_BlockSigns) do
        if tostring(k) == l_Msg[2] then
          v:setrotation(0, g_BlockStart + 120.0, 0)
        else
          v:setrotation(0, g_BlockStart, 0)
        end
      end
    end
  end
end
