-- (w) 2021 by Dustbin::Games / Christian Keimel / This file is licensed under the terms of the zlib license: https://opensource.org/licenses/Zlib

-- This file contains the functions to calculate the championships

-- **** The data structure for a championship
--
-- {
--   standings = {
--     [1] = <Player Name>,
--     [2] = <Player Name>,
--     ...
--   },
--   players = {
--     [1] = {
--       name = <Player Name>,
--       points = <Championship score of the player>,
--       positions = {
--         [1] = <Counter for wins of the player>,
--         [2] = <Counter for 2nd positions of the player>,
--         ...
--       },
--       respawn = <Number of respawns of the player>,
--       stunned = <Number of stunns of the player>.
--       dnf = <Number of DNFs of the player>,
--       firstrace = <Finish position in the first race>
--     },
--     [2] = { ... }
--     ...
--   }
--   races = {
--     [1] = {
--       track = <Track identifier of the first race>,
--       laps = <Number of laps>,
--       result = {
--         [1] = {
--           player = <Name of the winner of the race>,
--           laps = <Number of laps done>,
--           time = <Finish time of the player, "-1" == DNF>,
--           respawn = <Number of the player's respawn in this race>,
--           stunned = <Number of the player's stunns in this race>
--         },
--         [2] = { ... },
--         ...
--       }
--     }
--     ...
--   }
-- }

-- The rules for the positions in the championship (sorting rules position 1..x):
--
-- - higher score
-- - more wins
-- - more 2nd places
-- - ...
-- - more xth places
-- - less DNFs
-- - less Respawns
-- - Less stuns
--
-- If two players are equal in all of these points (not very likely, but to be sure)
-- the player who has finished in a better position in the first race the the lead

g_Verbose = false   -- Set this to "true" for more console output in the test application

-- Update the standings of the championship with the latest race result
function updateChampionshipStandings(a_Championship, a_Race)
  -- The score for each finish position. The bigger the race was
  -- the more points each player can gain
  l_FinishPoints = {
    [ 1] = { [1] =  0 },
    [ 2] = { [1] =  2, [2] =  0 },
    [ 3] = { [1] =  3, [2] =  1, [3] =  0 },
    [ 4] = { [1] =  4, [2] =  2, [3] =  1, [4] =  0 },
    [ 5] = { [1] =  5, [2] =  3, [3] =  2, [4] =  1, [5] =  0 },
    [ 6] = { [1] =  7, [2] =  4, [3] =  3, [4] =  2, [5] =  1, [6] =  0 },
    [ 7] = { [1] =  9, [2] =  6, [3] =  4, [4] =  3, [5] =  2, [6] =  1, [7] = 0 },
    [ 8] = { [1] = 10, [2] =  7, [3] =  5, [4] =  4, [5] =  3, [6] =  2, [7] = 1, [8] = 0 },
    [ 9] = { [1] = 11, [2] =  9, [3] =  7, [4] =  5, [5] =  4, [6] =  3, [7] = 2, [8] = 1, [9] = 0 },
    [10] = { [1] = 13, [2] = 11, [3] =  9, [4] =  6, [5] =  5, [6] =  4, [7] = 3, [8] = 2, [9] = 1, [10] = 0 },
    [11] = { [1] = 15, [2] = 13, [3] = 11, [4] =  7, [5] =  6, [6] =  5, [7] = 4, [8] = 3, [9] = 2, [10] = 1, [11] = 0 },
    [12] = { [1] = 17, [2] = 14, [3] = 12, [4] =  8, [5] =  7, [6] =  6, [7] = 5, [8] = 4, [9] = 3, [10] = 2, [11] = 1, [12] = 0 },
    [13] = { [1] = 19, [2] = 16, [3] = 13, [4] =  9, [5] =  8, [6] =  7, [7] = 6, [8] = 5, [9] = 4, [10] = 3, [11] = 2, [12] = 1, [13] = 0 },
    [14] = { [1] = 21, [2] = 15, [3] = 14, [4] = 10, [5] =  9, [6] =  8, [7] = 7, [8] = 6, [9] = 5, [10] = 4, [11] = 3, [12] = 2, [13] = 1, [14] = 0 },
    [15] = { [1] = 23, [2] = 16, [3] = 15, [4] = 11, [5] = 10, [6] =  9, [7] = 8, [8] = 7, [9] = 6, [10] = 5, [11] = 4, [12] = 3, [13] = 2, [14] = 1, [15] = 0 },
    [16] = { [1] = 25, [2] = 20, [3] = 16, [4] = 13, [5] = 11, [6] = 10, [7] = 9, [8] = 8, [9] = 7, [10] = 6, [11] = 5, [12] = 4, [13] = 3, [14] = 2, [15] = 1, [16] = 0 }
  }
  
  local l_Players = #a_Race["result"]
  
  if a_Championship["players"] == nil then
    a_Championship["players"] = { }
  end
  
  if g_Verbose then
    io.write("Found " .. tostring(l_Players) .. " Players.\n")
  end
  
  for i = 1, l_Players do
    if g_Verbose then
      io.write("Calculate player " .. tostring(i) .. "...\n")
    end
    
    local l_Player = nil
    
    for k,v in pairs(a_Championship["players"]) do
      if v["name"] == a_Race["result"][i]["name"] then
        l_Player = v
      end
    end
    
    if g_Verbose then
      io.write("Player: " .. tostring(l_Player) .. "\n")
    end
    
    -- Player not found ==> Add new dataset
    if l_Player == nil then
      l_Player = { }
      l_Player["name"     ] = a_Race["result"][i]["name"]
      l_Player["stunned"  ] = 0
      l_Player["respawn"  ] = 0
      l_Player["dnf"      ] = 0
      l_Player["score"    ] = 0
      l_Player["firstrace"] = i
      l_Player["positions"] = { }
      
      table.insert(a_Championship["players"], l_Player)
    end
    
    if l_Player ~= nil then
      -- Update the player's score and other fields
      l_Player["score"  ] = l_Player["score"  ] + l_FinishPoints[l_Players][i]
      l_Player["respawn"] = l_Player["respawn"] + a_Race["result"][i]["respawn"]
      l_Player["stunned"] = l_Player["stunned"] + a_Race["result"][i]["stunned"]
      
      if a_Race["result"][i]["time"] == -1 then
        l_Player["dnf"] = l_Player["dnf"] + 1
      end
      
      for j = 1, l_Players do
        if l_Player["positions"][j] == nil then
          l_Player["positions"][j] = 0
        end
      end
      
      l_Player["positions"][i] = l_Player["positions"][i] + 1
    end
    
    if g_Verbose then
      io.write("Ready.\n\n")
    end
  end
end

function championshipSort(a_Table, a_Item1, a_Item2)
  -- Step 1: Check score: Hight score is better
  if a_Table[a_Item1]["score"] ~= a_Table[a_Item2]["score"] then
    return a_Table[a_Item1]["score"] > a_Table[a_Item2]["score"]
  else
    -- Step 2: Check all finishing positions from 1 to 16: if a player has
    --         has finished more often in a good position he is the better one
    for i = 1, 16 do
      if a_Table[a_Item1]["positions"][i] ~= nil and a_Table[a_Item2]["positions"][i] ~= nil and a_Table[a_Item1]["positions"][i] ~= a_Table[a_Item2]["positions"][i] then
        return a_Table[a_Item1]["positions"][i] > a_Table[a_Item2]["positions"][i]
      elseif a_Table[a_Item1]["positions"][i] ~= nil and a_Table[a_Item2]["positions"][i] == nil then
        return false
      elseif a_Table[a_Item2]["positions"][i] == nil and a_Table[a_Item2]["positions"][i] ~= nil then
        return true
      end
    end
    
    -- Step 3: Less DNFs is better
    if a_Table[a_Item1]["dnf"] ~= a_Table[a_Item2]["dnf"] then
      return a_Table[a_Item1]["dnf"] < a_Table[a_Item2]["dnf"]
    end
    
    -- Step 4: less Respawns is better
    if a_Table[a_Item1]["respawn"] ~= a_Table[a_Item2]["respawn"] then
      return a_Table[a_Item1]["respawn"] < a_Table[a_Item2]["respawn"]
    end
    
    -- Step 5: less Stunns is better
    if a_Table[a_Item1]["stunned"] ~= a_Table[a_Item2]["stunned"] then
      return a_Table[a_Item1]["stunned"] < a_Table[a_Item2]["stunned"]
    end
    
    -- Step 6: if all other tests fail the player who was better 
    --         in the first race is considered better
    return a_Table[a_Item1]["firstrace"] < a_Table[a_Item2]["firstrace"]
  end
end

function calculateStandings(a_Championship)
  a_Championship["standings"] = { }
  
  for k,v in spairs(a_Championship["players"], championshipSort) do
    table.insert(a_Championship["standings"], v["name"])
  end
end
