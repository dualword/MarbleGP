-- This function fills the championship data structure with 
-- the information for the next race

function startNextRace(a_Championship, a_Track, a_Laps, a_GameSettings)
  a_Championship["thisrace"] = system:createtable("SRace")
  a_Championship["thisrace"]["track"] = a_Track
  a_Championship["thisrace"]["class"] = a_GameSettings["raceclass"]
  a_Championship["thisrace"]["laps" ] = a_Laps
  
  -- First we fill a "grid" table with the
  -- number of players
  local l_Grid = { }
  io.write("\n\n\nGridsize: " .. tostring(#a_Championship["players"]) .. "\n\n\n")
  for i = 1, #a_Championship["players"] do
    table.insert(l_Grid, i)
  end
  
  -- Depending on the game settings and the championship situation we re-arrange the starting grid
  if a_GameSettings["settings"]["starting_positions"] == 2 or (a_GameSettings["settings"]["randomize_first"] and #a_Championship["races"] == 0) then
    l_Grid = shuffle(l_Grid)
  elseif a_StartingPositions == 1 then
    -- todo: championship standings
  elseif a_StartingPositions == 0 then
    -- todo: last race
  end
  
  -- If the user wishes we can reverse the trid
  if a_GameSettings["reverse_grid"] then
    local l_Reverse = { }
    local l_Index   = #l_Grid
    
    while l_Index >= 1 do
      table.insert(l_Reverse, l_Grid[l_Index])
      l_Index = l_Index - 1
    end
    
    l_Grid = l_Reverse
  end
  
  -- If it's the first race we need to add the starting number
  if #a_Championship["races"] == 0 then
    for i = 1, #l_Grid do
      local l_Texture = tostring(a_Championship["players"][l_Grid[i]]["texture"])
      local l_Split   = split(l_Texture, "://")
      
      if l_Split[1] == "generate" then
        a_Championship["players"][l_Grid[i]]["texture"] = a_Championship["players"][l_Grid[i]]["texture"] .. "&number=" .. tostring(i)
      end
    end
  end
  
  a_Championship["thisrace"]["grid"] = l_Grid
end
