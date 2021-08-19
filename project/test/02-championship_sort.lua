system:executeluascript("data/lua/championship_functions.lua")
system:executeluascript("data/lua/serializer.lua")
system:executeluascript("data/lua/spairs.lua")

io.write("Test 02: Championship Sorting Function\n")
io.write("**************************************\n\n")

l_Championship = { 
  players = {
    [1] = {
      name = "Player 1",
      score = 6,
      positions = { [1] = 0, [2] = 3, [3] = 0, [4] = 0 },
      respawn = 1,
      stunned = 3,
      dnf = 1,
      firstrace = 3
    },
    [2] = {
      name = "Player 2",
      score = 8,
      positions = { [1] = 2, [2] = 2, [3] = 0, [4] = 0 },
      respawn = 2,
      stunned = 2,
      dnf = 0,
      firstrace = 1
    },
    [3] = {
      name = "Player 3",
      score = 4,
      positions = { [1] = 1, [2] = 0, [3] = 2, [4] = 0 },
      respawn = 3,
      stunned = 1,
      dnf = 3,
      firstrace = 2
    },
    [4] = {
      name = "Player 4",
      score = 10,
      positions = { [1] = 2, [2] = 1, [3] = 0, [4] = 0 },
      respawn = 4,
      stunned = 0,
      dnf = 2,
      firstrace = 4
    }
  },
  races = {
    [1] = {
      track = "TestTrack",
      laps = 4,
      result = {
        [1] = {
          player = "Player 2",
          laps = 4,
          time = 1000,
          respawn = 0,
          stunned = 3
        },
        [2] = {
          player = "Player 3",
          laps = 4,
          time = 1010,
          respawn = 0,
          stunned = 2
        },
        [3] = {
          player = "Player 1",
          laps = 3,
          time = 990,
          respawn = 1,
          stunned = 1
        },
        [4] = {
          player = "Player 4",
          laps = 3,
          time = 1005,
          respawn = 0,
          stunned = 0
        }
      }
    },
    [2] = {
      track = "Track 2"
    },
    [3] = {
      track = "Track 3"
    }
  }
}

io.write("\n************\n")
io.write("Test Result:\n")
io.write("************\n\n")

io.write("\n\n**** Test 1: Points sort\n\n")
calculateStandings(l_Championship)
io.write(serializeTable(l_Championship["standings"], 2))

io.write("\n\n")
for i = 1, #l_Championship["players"] do
  io.write(tostring(i) .. ": " .. l_Championship["players"][i]["name"] .. " == " .. tostring(l_Championship["players"][i]["score"]) .. "\n")
end



io.write("\n\n**** Test 2: Position sort\n\n")

for k,v in pairs(l_Championship["players"]) do
  v["score"] = 5
end

calculateStandings(l_Championship)
io.write(serializeTable(l_Championship["standings"], 2))

io.write("\n\n**** Test 2: DNF sort\n\n")

for k,v in pairs(l_Championship["players"]) do
  for i = 1, 4 do
    v["positions"][i] = 1
  end
end

calculateStandings(l_Championship)
io.write(serializeTable(l_Championship["standings"], 2))

io.write("\n\n**** Test 3: Respawn sort\n\n")

for k,v in pairs(l_Championship["players"]) do
  v["dnf"] = 0
end

calculateStandings(l_Championship)
io.write(serializeTable(l_Championship["standings"], 2))


io.write("\n\n**** Test 4: Stunned sort\n\n")

for k,v in pairs(l_Championship["players"]) do
  v["respawn"] = 0
end

calculateStandings(l_Championship)
io.write(serializeTable(l_Championship["standings"], 2))


io.write("\n\n**** Test 5: First Race sort\n\n")

for k,v in pairs(l_Championship["players"]) do
  v["stunned"] = 0
end

calculateStandings(l_Championship)
io.write(serializeTable(l_Championship["standings"], 2))

