
system:executeluascript("data/lua/serializer.lua")

io.write("**** Game Logics Script started\n")

function initialize(a_RaceData)
  g_Laps = a_RaceData["laps"]
  
  g_Players = { }
  
  for i = 1, #a_RaceData["marbles"] do
    local l_Id = a_RaceData["marbles"][i]
    g_Players[l_Id] = { }
    g_Players[l_Id]["laps"      ] = { }
    g_Players[l_Id]["currentlap"] = 0
    g_Players[l_Id]["respawn"   ] = 0
    g_Players[l_Id]["stunned"   ] = 0
    g_Players[l_Id]["finished"  ] = false
  end
end

function onPlayerStunned(a_Marble, a_Step)
  if g_Players[a_Marble] ~= nil then
    g_Players[a_Marble]["stunned"] = g_Players[a_Marble]["stunned"] + 1
  end
end

function onPlayerRespawn(a_Marble, a_Step)
  io.write("*** onPlayerRespawn: " .. tostring(a_Marble) .. "\n")
  if g_Players[a_Marble] ~= nil then
    g_Players[a_Marble]["respawn"] = g_Players[a_Marble]["respawn"] + 1
  end
end

function onCheckpoint(a_Marble, a_Checkpoint, a_Step)
  io.write("Lua Checkpoint: " .. tostring(a_Marble) .. ": " .. tostring(a_Checkpoint)  .. " [" .. tostring(a_Step) .. "]\n")
  
  local l_Found = false
  
  for k,v in pairs(g_Players[a_Marble]["laps"][g_Players[a_Marble]["currentlap"]]) do
    if v["id"] == a_Checkpoint then
      v["time"] = a_Step
      l_Found = true
    end
  end
  
  if not l_Found then
    local l_Checkpoint = { }
    l_Checkpoint["id"  ] = a_Checkpoint
    l_Checkpoint["time"] = a_Step
    
    table.insert(g_Players[a_Marble]["laps"][g_Players[a_Marble]["currentlap"]], l_Checkpoint)
  end
end

function onLapStart(a_Marble, a_LapNo, a_Step)
  io.write("Lua Lap Start: " .. tostring(a_Marble) .. ": " .. tostring(a_LapNo)  .. " [" .. tostring(a_Step) .. "]\n")
  
  if a_LapNo > g_Laps then
    dynamics:finishplayer(a_Marble, a_Step, g_Players[a_Marble]["currentlap"])
    
    if g_Players[a_Marble] ~= nil then
      g_Players[a_Marble]["finished"] = true
    end
  else
    g_Players[a_Marble]["currentlap"] = g_Players[a_Marble]["currentlap"] + 1
    g_Players[a_Marble]["laps"      ][g_Players[a_Marble]["currentlap"]] = { }
  end
end

function onCountdown(a_Tick, a_Step)
  if a_Tick == 0 then
    for k,v in pairs(g_Players) do
      dynamics:startplayer(k)
    end
  end
end
