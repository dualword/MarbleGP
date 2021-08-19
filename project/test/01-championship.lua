system:executeluascript("data/lua/championship_functions.lua")
system:executeluascript("data/lua/serializer.lua")

io.write("Test 01: Basic Championship Functions\n")
io.write("*************************************\n\n")

l_Race = {
  track = "Test01",
  laps = 4,
  result = { }
}

for i = 1, 16 do
  local l_Player = { }
  l_Player["name"] = "Player " .. tostring(i)
  l_Player["laps"] = 4
  l_Player["time"] = 1000 + 5 * i
  
  if math.fmod(i, 8) == 0 then
    l_Player["respawn"] = 1
  else
    l_Player["respawn"] = 0
  end
  
  if math.fmod(i, 4) == 0 then
    l_Player["stunned"] = 1
  else
    l_Player["stunned"] = 0
  end
  
  table.insert(l_Race["result"], l_Player)
end


l_Championship = { }

g_Verbose = true

updateChampionshipStandings(l_Championship, l_Race)
updateChampionshipStandings(l_Championship, l_Race)

io.write("\n************\n")
io.write("Test Result:\n")
io.write("************\n\n")
io.write(serializeTable(l_Championship, 2))