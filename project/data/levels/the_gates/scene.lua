
g_Warnings = {
  [1] = {
    trigger = {
      marbles = { },
      count = 0
    },
    checkpoint = {
      marbles = { },
      count = 0
    },
    checkpointid = 23004,
    trigger_inc = 1,
    trigger_dec = 3,
    sign = 23300,
    sign_small = 25000,
    currenty = 180,
    targety = 180
  },
  [2] = {
    trigger = {
      marbles = { },
      count = 0
    },
    checkpoint = {
      marbles = { },
      count = 0
    },
    checkpointid = 23003,
    trigger_inc = 0,
    trigger_dec = 2,
    sign = 23299,
    sign_small = 25001,
    currenty = 180,
    targety = 180
  }
}

g_Finished = {
}

function adjustRotation(a_Index)
  if g_Warnings[a_Index]["trigger"]["count"] > 0 then
    g_Warnings[a_Index]["targety"] = 60
  elseif g_Warnings[a_Index]["checkpoint"]["count"] > 0 then
    g_Warnings[a_Index]["targety"] = 300
  else
    g_Warnings[a_Index]["targety"] = 180
  end
end

function addMarble(a_Dict, a_Marble, a_Index)
  if a_Dict["marbles"][a_Marble] == nil and g_Finished[a_Marble] == nil then
    a_Dict["marbles"][a_Marble] = true
    a_Dict["count"] = a_Dict["count"] + 1
    
    adjustRotation(a_Index)
  end
end

function removeMarble(a_Dict, a_Marble, a_Index)
  if a_Dict["marbles"][a_Marble] ~= nil then
    a_Dict["marbles"][a_Marble] = nil
    a_Dict["count"] = a_Dict["count"] - 1
    
    if a_Dict["count"] < 0 then
      a_Dict["count"] = 0
    end
    
    adjustRotation(a_Index)
  end
end

function removeFromAll(a_Marble)
  for i = 1, 2 do
    removeMarble(g_Warnings[i]["checkpoint"], a_Marble, i)
    removeMarble(g_Warnings[i]["trigger"   ], a_Marble, i)
  end
end

function onplayerfinished(a_Marble, a_Racetime, a_Laps)
  removeFromAll(a_Marble)
  
  g_Finished[a_Marble] = true
end

function onplayerrespawn(a_Marble, a_State)
  if a_State == 1 then
    removeFromAll(a_Marble)
  end
end

function onplayerwithdrawn(a_Marble)
  removeFromAll(a_Marble)
end

function oncheckpoint(a_Marble, a_Checkpoint)
  for i = 1, 2 do
    if g_Warnings[i]["checkpointid"] == a_Checkpoint then
      addMarble(g_Warnings[i]["checkpoint"], a_Marble, i)
    end
  end
end

function ontrigger(a_Marble, a_Trigger)
  for i = 1, 2 do
    if g_Warnings[i]["trigger_inc"] == a_Trigger then
      removeMarble(g_Warnings[i]["checkpoint"], a_Marble, i)
      addMarble(g_Warnings[i]["trigger"], a_Marble, i)
    elseif g_Warnings[i]["trigger_dec"] == a_Trigger then
      removeMarble(g_Warnings[i]["trigger"], a_Marble, i)
    end
  end
end

function onstep(a_Step)
  for i = 1, 2 do
    if g_Warnings[i]["currenty"] < g_Warnings[i]["targety"] then
      g_Warnings[i]["currenty"] = g_Warnings[i]["currenty"] + 5
      
      if g_Warnings[i]["currenty"] > g_Warnings[i]["targety"] then
        g_Warnings[i]["currenty"] = g_Warnings[i]["targety"]
      end
      
      scene:setrotation(g_Warnings[i]["sign"      ], { x = 0.0, y = g_Warnings[i]["currenty"], z = 0.0 })
      scene:setrotation(g_Warnings[i]["sign_small"], { x = 0.0, y = g_Warnings[i]["currenty"], z = 0.0 })
    elseif g_Warnings[i]["currenty"] > g_Warnings[i]["targety"] then
      g_Warnings[i]["currenty"] = g_Warnings[i]["currenty"] - 5
      
      if g_Warnings[i]["currenty"] < g_Warnings[i]["targety"] then
        g_Warnings[i]["currenty"] = g_Warnings[i]["targety"]
      end
      
      scene:setrotation(g_Warnings[i]["sign"      ], { x = 0.0, y = g_Warnings[i]["currenty"], z = 0.0 })
      scene:setrotation(g_Warnings[i]["sign_small"], { x = 0.0, y = g_Warnings[i]["currenty"], z = 0.0 })
    end
  end
end