system:executeluascript("data/lua/serializer.lua")

g_GameSetup = { }
g_Players   = { }
g_AddPlayer = -1

function initialize()
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
  end
  
  local l_Setup = system:getsetting("game_setup")
  
  if l_Setup ~= "" then
    local s = "g_GameSetup = " .. l_Setup
    system:executeluastring(s)
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
  
  io.write("***** " .. type(g_GameSetup["settings"]["reverse_grid"      ]) .. "\n")
  
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
    local s = serializeTable(g_GameSetup, 2)
    io.write("Saving " .. s)
    system:setsetting("game_setup", s)
  elseif a_Name == "add_player" then
    g_AddPlayer = a_Id
    g_Items["add_player"]["dialog"]:setvisible(true)
    
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
  elseif string.sub(a_Name, 1, 6) == "add_pl" then
    io.write("Add player #" .. tostring(a_Id) .. " to game.\n")
    table.insert(g_GameSetup["players"], g_Players[a_Id]["name"])
    g_Items["add_player"]["dialog"]:setvisible(false)
    updatePlayerUi()
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
  
  io.write("\n\n" .. serializeTable(g_Items) .. "\n\n")
end

function windowresized()
  fillItems()
end

function uivaluechanged(a_Id, a_Name, a_Value)
  io.write("Value Changed: " .. a_Name .. " (" .. tostring(a_Id) .. "): " .. tostring(a_Value) .. "\n")
  g_GameSetup["settings"][a_Name] = math.floor(a_Value)
  
  io.write("\n\n" .. serializeTable(g_GameSetup, 2) .. "\n\n")
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
