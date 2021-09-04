-- (w) 2021 by Dustbin::Games / Christian Keimel / This file is licensed under the terms of the zlib license: https://opensource.org/licenses/Zlib

system:executeluascript("data/lua/serializer.lua")

g_GameSetup = { }
g_Players   = { }
g_AddPlayer = -1

function initialize()
  system:createtexture(getDefaultTexture(16, 1))
  io.write("Setup Race script started.\n")
  
  g_Smgr = system:getscenemanager()
  g_Smgr:loadscene("data/scenes/skybox.xml")
  
  g_Camera = g_Smgr:addcamera()

  g_Camera:setposition({ x = 0.0, y = 0.0, z =  0.0 })
  g_Camera:setupvector({ x = 0.0, y = 1.0, z =  0.0 })
  g_Camera:settarget  ({ x = 0.0, y = 0.0, z = 35.0 })
  g_Camera:activate()
  
  -- dialog:addlayoutraster()
  dialog:loaddialog("data/menu/menu_setupgame.xml"      )
  dialog:loaddialog("data/menu/button_cancel.xml"       )  -- The "Cancel" button
  dialog:loaddialog("data/menu/button_ok.xml"           )  -- The "OK" button
  dialog:loaddialog("data/menu/dialog_select_player.xml")
  
  g_GameSetup["players"] = { }
  
  local l_Profiles = system:getsetting("profiles")
  
  if l_Profiles ~= "" then
    local s = "g_Players = " .. l_Profiles
    system:executeluastring(s)
  else
    g_Players = { }
  end
  
  local l_Setup = system:getsetting("game_setup")
  
  if l_Setup ~= "" then
    local s = "g_GameSetup = " .. l_Setup
    system:executeluastring(s)
  end
  
  local l_ToErase = { }
  
  if g_GameSetup["players"] ~= nil then
    local l_Index = #g_GameSetup["players"]
    
    while l_Index >= 1 do
      local l_Erase = true
      
      for i = 1, #g_Players do
        if g_Players[i]["name"] == g_GameSetup["players"][l_Index] then
          l_Erase = false
        end
      end

      if l_Erase then
        table.remove(g_GameSetup["players"], l_Index)
      end
      
      l_Index = l_Index - 1
    end
  end
  
  if g_GameSetup["settings"] == nil then
    g_GameSetup["settings"] = { }
    g_GameSetup["settings"]["reverse_grid"      ] = false
    g_GameSetup["settings"]["randomize_first"   ] = true
    g_GameSetup["settings"]["ai_class"          ] = 3
    g_GameSetup["settings"]["raceclass"         ] = 0
    g_GameSetup["settings"]["gridsize"          ] = 4
    g_GameSetup["settings"]["fillgrid_ai"       ] = true
    g_GameSetup["settings"]["starting_positions"] = 0
  end
  
  dialog:createui();
  audio:startsoundtrack(0)
  fillItems()
  updatePlayerUi()
end

function cleanup()
  g_Smgr:clear()
  dialog:clear()
end

function getUnassignedPlayers()
  local l_Ret = { }
  
  for i = 1, #g_Players do
    local l_Add = true
    
    for j = 1, #g_GameSetup["players"] do
      if g_Players[i]["name"] == g_GameSetup["players"][j] then
        l_Add = false
      end
    end
    
    if l_Add then
      local l_New = { }
      l_New["name" ] = g_Players[i]["name"]
      l_New["index"] = i
      
      table.insert(l_Ret, l_New)
    end
  end
  
  return l_Ret
end

function fillGlobalVpData()
  if g_ViewportData == nil then
    local s = system:getsetting("viewportdata")
    if s ~= "" then
      s = "g_ViewportData = " .. s
      system:executeluastring(s)
    end
  end
  
  if g_ViewportData == nil or g_ViewportData["viewports"] == nil or g_ViewportData["distribution"] == nil then
    g_ViewportData = {
      -- The definition of the distribution
      -- of the viewports for [key] players
      viewports = {
        [1] = { columns = 1, rows = 1 },
        [2] = { columns = 1, rows = 2 },
        [3] = { columns = 2, rows = 2 },
        [4] = { columns = 2, rows = 2 },
        [5] = { columns = 3, rows = 2 },
        [6] = { columns = 3, rows = 2 },
        [7] = { columns = 3, rows = 3 },
        [8] = { columns = 3, rows = 3 }
      },
      -- Which of the viewports are used for which player?
      distribution = {
        [1] = { [1] = 1 },
        [2] = { [1] = 1, [2] = 2 },
        [3] = { [1] = 1, [2] = 2, [3] = 3 },
        [4] = { [1] = 1, [2] = 2, [3] = 3, [4] =  4 },
        [5] = { [1] = 1, [2] = 2, [3] = 3, [4] =  4, [5] = 5 },
        [6] = { [1] = 1, [2] = 2, [3] = 3, [4] =  4, [5] = 5, [6] = 6 },
        [7] = { [1] = 1, [2] = 2, [3] = 3, [4] =  5, [5] = 7, [6] = 8, [7] = 9 },
        [8] = { [1] = 1, [2] = 2, [3] = 3, [4] =  4, [5] = 6, [6] = 7, [7] = 8, [8] = 9 }
      }
    }
  end
end

function createViewport(a_PlayerIndex, a_NumberOfPlayers)
  local l_Size = system:getscreensize()
  
  fillGlobalVpData()
  
  local l_Viewport = system:createtable("SViewPort")
  l_Viewport["playerid"] = a_PlayerIndex
  
  local l_Pos = g_ViewportData["distribution"][a_NumberOfPlayers][a_PlayerIndex] - 1
  local l_Row = math.floor(l_Pos / g_ViewportData["viewports"][a_NumberOfPlayers]["columns"])
  local l_Col = math.fmod (l_Pos,  g_ViewportData["viewports"][a_NumberOfPlayers]["columns"])
  
  l_Viewport["rect"]["upperleftcorner" ]["x"] = math.floor( l_Col      * l_Size["x"] / g_ViewportData["viewports"][a_NumberOfPlayers]["columns"])
  l_Viewport["rect"]["upperleftcorner" ]["y"] = math.floor( l_Row      * l_Size["y"] / g_ViewportData["viewports"][a_NumberOfPlayers]["rows"   ])
  l_Viewport["rect"]["lowerrightcorner"]["x"] = math.floor((l_Col + 1) * l_Size["x"] / g_ViewportData["viewports"][a_NumberOfPlayers]["columns"])
  l_Viewport["rect"]["lowerrightcorner"]["y"] = math.floor((l_Row + 1) * l_Size["y"] / g_ViewportData["viewports"][a_NumberOfPlayers]["rows"   ])
  
  return l_Viewport
end

function getDefaultTexture(a_PlayerIndex, a_Class)
  local l_Textures = {
    [ 1] = "numbercolor=000000&numberback=4b64f9&numberborder=4b64f9&ringcolor=3548b7&patterncolor=000000&patternback=4b64f9",
    [ 2] = "numbercolor=000000&numberback=fd5320&numberborder=fd5320&ringcolor=ba3b15&patterncolor=000000&patternback=fd5320",
    [ 3] = "numbercolor=000000&numberback=3aec1e&numberborder=3aec1e&ringcolor=28ae13&patterncolor=000000&patternback=3aec1e",
    [ 4] = "numbercolor=000000&numberback=c0c000&numberborder=c0c000&ringcolor=8d8d00&patterncolor=000000&patternback=c0c000",
    [ 5] = "numbercolor=000000&numberback=ffc0cb&numberborder=ffc0cb&ringcolor=bc8d95&patterncolor=000000&patternback=ffc0cb",
    [ 6] = "numbercolor=ffffff&numberback=6a0dad&numberborder=6a0dad&ringcolor=c1bbcc&patterncolor=ffffff&patternback=6a0dad",
    [ 7] = "numbercolor=000000&numberback=00ffff&numberborder=00ffff&ringcolor=00bcbc&patterncolor=000000&patternback=00ffff",
    [ 8] = "numbercolor=ffffff&numberback=000000&numberborder=000000&ringcolor=bbbbbb&patterncolor=ffffff&patternback=000000",
    [ 9] = "numbercolor=000000&numberback=ffdab9&numberborder=ffdab9&ringcolor=b9906b&patterncolor=000000&patternback=ffdab9",
    [10] = "numbercolor=000000&numberback=87cefa&numberborder=87cefa&ringcolor=8ebfdd&patterncolor=000000&patternback=87cefa",
    [11] = "numbercolor=000000&numberback=daa520&numberborder=daa520&ringcolor=b8860b&patterncolor=000000&patternback=daa520",
    [12] = "numbercolor=000000&numberback=9932cc&numberborder=9932cc&ringcolor=da70d6&patterncolor=000000&patternback=9932cc",
    [13] = "numbercolor=ffffff&numberback=00008b&numberborder=00008b&ringcolor=0000ff&patterncolor=ffffff&patternback=00008b",
    [14] = "numbercolor=000000&numberback=ffdead&numberborder=ffdead&ringcolor=ffefd5&patterncolor=000000&patternback=ffdead",
    [15] = "numbercolor=000000&numberback=20b2aa&numberborder=20b2aa&ringcolor=7fffd4&patterncolor=000000&patternback=20b2aa",
    [16] = "numbercolor=000000&numberback=ffffff&numberborder=ffffff&ringcolor=dddddd&patterncolor=000000&patternback=ffffff"
  }
  
  local l_ClassTexture = "texture_marblegp.png"
  
  if a_Class == 0 then
    l_ClassTexture = "texture_marbles3.png"
  elseif a_Class == 1 then
    l_ClassTexture = "texture_marbles2.png"
  end
  
  return "generate://pattern=" .. l_ClassTexture .. "&" .. l_Textures[a_PlayerIndex]
end

function uibuttonclicked(a_Id, a_Name)
  if a_Name == "cancel" then
    if g_Items["add_player"]["dialog"]:isvisible() then
      g_AddPlayer = -1
      g_Items["add_player"]["dialog"]:setvisible(false)
    else
      system:clearscriptstack()
      system:statechange(1)
    end
  elseif a_Name == "ok" then
    if #g_GameSetup["players"] == 0 then
      -- ToDo: add message
    else
      local l_Championship = system:createtable("SChampionShip")
      
      for i = 1, #g_GameSetup["players"] do
        local l_Player = g_GameSetup["players"][i]
        for j = 1, #g_Players do
          if g_Players[j]["name"] == l_Player then
            local l_Copy = { }
            
            for k,v in pairs(g_Players[j]) do
              l_Copy[k] = v
            end
            
            if l_Copy["texture"] == "" then
              l_Copy["texture"] = getDefaultTexture(j, 0)
            else
              l_Copy["texture"] = l_Copy["texture"]
            end
            
            l_Copy["playerid"] = i
            table.insert(l_Championship["players"], l_Copy)
            
            table.insert(l_Championship["viewports"], createViewport(#l_Championship["players"], #g_GameSetup["players"]))
          end
        end
      end
      
      local s = serializeTable(g_GameSetup, 2)
      system:setsetting("game_setup", s)
      
      s = serializeTable(l_Championship, 2)
      system:setglobal("championship", s)
      
      system:pushscript("data/lua/menu_setupgame.lua")
      system:pushscript("data/lua/menu_selecttrack.lua")
      system:statechange(1)
    end
  elseif a_Name == "add_player" then
    g_AddPlayer = a_Id
    g_Items["add_player"]["dialog"]:setvisible(true)
    system:setzlayer(46)
    
    local l_Unassigned = getUnassignedPlayers()
    
    for i = 1, 8 do
      if i <= #l_Unassigned then
        g_Items["add_player"]["add_pl" .. tostring(i)]:setvisible(true)
        g_Items["add_player"]["add_pl" .. tostring(i)]:settext(l_Unassigned[i]["name" ])
        g_Items["add_player"]["add_pl" .. tostring(i)]:setid  (l_Unassigned[i]["index"])
      else
        g_Items["add_player"]["add_pl" .. tostring(i)]:setvisible(false)
      end
    end
  elseif a_Name == "close_addplayer" then
    g_AddPlayer = -1
    g_Items["add_player"]["dialog"]:setvisible(false)
    system:setzlayer(0)
  elseif string.sub(a_Name, 1, 6) == "add_pl" then
    io.write("Add player #" .. tostring(a_Id) .. " to game.\n")
    table.insert(g_GameSetup["players"], g_Players[a_Id]["name"])
    g_Items["add_player"]["dialog"]:setvisible(false)
    updatePlayerUi()
    system:setzlayer(0)
  elseif a_Name == "remove_player" then
    table.remove(g_GameSetup["players"], a_Id)
    updatePlayerUi()
  else
    io.write("==> " .. a_Name .. " (" .. tostring(a_Id) .. ") | " .. string.sub(a_Name, 1, 6) .. "\n")
  end
end

function updatePlayerUi()
  local l_Unassigned = getUnassignedPlayers()
  
  for i = 1, #g_Items do
    if i <= #g_GameSetup["players"] then
      g_Items[i]["player_name"]:settext(g_GameSetup["players"][i])
      
      g_Items[i]["player_tab"]:setvisible(true )
      g_Items[i]["add_player"]:setvisible(false)
      
      g_Items[i]["root"]:setproperty("UseOverrideColor", "false")
    elseif i - 1 == #g_GameSetup["players"] and #l_Unassigned ~= 0 then
      g_Items[i]["player_tab"]:setvisible(false)
      g_Items[i]["add_player"]:setvisible(true )
      
      g_Items[i]["root"]:setproperty("UseOverrideColor", "false")
    else
      g_Items[i]["player_tab"]:setvisible(false)
      g_Items[i]["add_player"]:setvisible(false)
      
      g_Items[i]["root"]:setproperty("UseOverrideColor", "true")
    end
  end
end

function fillItems()
  g_Items = { }
  
  for i = 1, 8 do
    local l_Name  = "player" .. tostring(i)
    local l_Item  = dialog:getitemfromname(l_Name)
    local l_Items = { }
    
    l_Items["root"] = l_Item
    
    for j = 1, l_Item:getchildcount() do
      local l_Child = l_Item:getchild(j)
      
      l_Child:setid(i)
      
      l_Items[l_Child:getname()] = l_Child
      
      if l_Child:getname() == "player_tab" then
        for k = 1, l_Child:getchildcount() do
          local l_GrandChild = l_Child:getchild(k)
          l_Items[l_GrandChild:getname()] = l_GrandChild
          l_GrandChild:setid(i)
      
          if l_GrandChild:getname() == "player_label" then
            l_GrandChild:settext(tostring(i))
          end
        end
      end
    end
    
    table.insert(g_Items, l_Items)
  end
  
  g_Items["add_player"] = { }
  g_Items["add_player"]["dialog"] = dialog:getitemfromname("addplayer_dialog")
  
  for i = 1, 8 do
    local l_Name = "add_pl" .. tostring(i)
    g_Items["add_player"][l_Name] = dialog:getitemfromname(l_Name)
  end
  
  -- Key for the GUI items with the race settings
  local l_Settings = {
    [1] = "reverse_grid",
    [2] = "randomize_first",
    [3] = "ai_class",
    [4] = "raceclass",
    [5] = "gridsize",
    [6] = "fillgrid_ai",
    [7] = "starting_positions"
  }
  
  g_Items["settings"] = { }
  
  -- Fill the table with the GUI items for the race settings
  for k,v in pairs(l_Settings) do
    g_Items["settings"][v] = dialog:getitemfromname(v)
    
    if g_GameSetup["settings"] ~= nil  and g_GameSetup["settings"][v] ~= nil then
      if type(g_GameSetup["settings"][v]) == "boolean" then
        g_Items["settings"][v]:setchecked(g_GameSetup["settings"][v])
      else
        g_Items["settings"][v]:setselected(g_GameSetup["settings"][v])
      end
    end
  end
  
  updatePlayerUi()
end

function windowresized()
  fillItems()
end

function uivaluechanged(a_Id, a_Name, a_Value)
  io.write("Value Changed: " .. a_Name .. " (" .. tostring(a_Id) .. "): " .. tostring(a_Value) .. "\n")
  g_GameSetup["settings"][a_Name] = math.floor(a_Value)
  
  if a_Name == "starting_positions" then
    if math.floor(a_Value) == 2 then
      g_Items["settings"]["randomize_first"]:setenabled(false)
    else
      g_Items["settings"]["randomize_first"]:setenabled(true)
    end
  end
end

function uicheckboxchanged(a_Id, a_Name, a_Checked)
  io.write("Checkbox changed: " .. a_Name .. " (" .. tostring(a_Id) .. "): " .. tostring(a_Checked) .. "\n")
  g_GameSetup["settings"][a_Name] = a_Checked
  
  io.write("\n\n" .. serializeTable(g_GameSetup, 2) .. "\n\n")
  
  if a_Name == "fillgrid_ai" then
    g_Items["settings"]["ai_class"]:setenabled(a_Checked)
    g_Items["settings"]["gridsize"]:setenabled(a_Checked)
  end
end
