system:executeluascript("data/lua/helpers_main.lua")

g_Time = 0  -- The time of the last "step" message

g_GameClasses = {
  [1] = "Rookies",
  [2] = "Marble3",
  [3] = "Marble2",
  [4] = "MarbleGP"
}

g_AiClasses = {
  [1] = "Rookies",
  [2] = "Marble3",
  [3] = "Marble2",
  [4] = "MarbleGP",
  [5] = "All"
}

g_GridPositions = {
  [1] = "Last Race",
  [2] = "Championship",
  [3] = "Random"
}

g_Options = {
  gameclass = { value = 1, options = g_GameClasses  , label = "label_class_ui", node = nil },
  aiclass   = { value = 1, options = g_AiClasses  , label = "label_ai_ui"   , node = nil },
  grid      = { value = 1, options = g_GridPositions, label = "label_grid_ui" , node = nil },
  
  gridreverse    = { value = false },
  autoend        = { value = false },
  fillgrid       = { value = true  },
  randomizefirst = { value = true  },
  randomgrid     = { value = true  }
}

g_Buttons = {
  button_class_prev = { key = "gameclass"     , type = "minus" , node = nil },
  button_class_next = { key = "gameclass"     , type = "plus"  , node = nil },
  button_ai_prev    = { key = "aiclass"       , type = "minus" , node = nil },
  button_ai_next    = { key = "aiclass"       , type = "plus"  , node = nil },
  button_grid_prev  = { key = "grid"          , type = "minus" , node = nil },
  button_grid_next  = { key = "grid"          , type = "plus"  , node = nil },
  button_reverse    = { key = "gridreverse"   , type = "toggle", node = nil },
  button_autoend    = { key = "autoend"       , type = "toggle", node = nil },
  button_fillgrid   = { key = "fillgrid"      , type = "toggle", node = nil },
  button_randomize  = { key = "randomizefirst", type = "toggle", node = nil },
  button_randomgrid = { key = "randomgrid"    , type = "toggle", node = nil }
}

g_PlayerNames = {
  [1] = { name = "Brainsaw"      , assigned = false },
  [2] = { name = "DCI Barnaby"   , assigned = false },
  [3] = { name = "Mr. Nukular"   , assigned = false },
  [4] = { name = "Miss Fisher"   , assigned = false },
  [5] = { name = "Red Baron"     , assigned = false },
  [6] = { name = "Dirty Harry"   , assigned = false },
  [7] = { name = "Endeavor"      , assigned = false },
  [8] = { name = "Marblus Rundus", assigned = false }
}

g_PlayerDialog = { }
g_Players      = { }

g_Smgr = nil  -- The Scene Manager LUA object

function initialize()
  io.write("Setup Game Menu script started.\n")
  
  g_Smgr = system:getscenemanager()
  g_Smgr:loadscene("data/menu3d/menu_setupgame.xml")
  g_Smgr:loadscene("data/menu3d/dialog_players.xml")
  g_Root = g_Smgr:getscenenodefromname("root_menusetupgame")
  g_Plrs = g_Smgr:getscenenodefromname("root_players")
  
  g_Plrs:setrotation({ x = 0, y = 90, z = 0 })
  
  g_Camera = g_Smgr:addcamera()
  g_Camera:setposition({ x = 0.0, y = 0.0, z = 30.0 })
  g_Camera:setupvector({ x = 0.0, y = 1.0, z =  0.0 })
  g_Camera:settarget  ({ x = 0.0, y = 0.0, z = 70.0 })
  g_Camera:activate()
  
  for k,v in pairs(g_Buttons) do
    v["node"] = g_Smgr:getscenenodefromname(k)
  end
  
  for k,v in pairs(g_Options) do
    if v["label"] ~= nil then
      v["node"] = g_Smgr:getguiitemfromname(v["label"])
    end
  end
  
  for k,v in pairs(g_Options) do
    if v["node"] ~= nil then
      v["node"]:settext(v["options"][v["value"]])
    else
      for k2, v2 in pairs(g_Buttons) do
        if v2["key"] == k and v2["node"] ~= nil then
          if v["value"] then
            v2["node"]:setrotation({ x = 0, y = 0, z = 0 })
          else
            v2["node"]:setrotation({ x = 0, y = 180, z = 0 })
          end
        end
      end
    end
  end
  
  for i = 1,8 do
    local l_Player = { }
    
    l_Player["key_add"] = "button_add_pl" .. tostring(i)
    l_Player["key_del"] = "button_del_pl" .. tostring(i)
    l_Player["label"  ] = g_Smgr:getscenenodefromname("mesh_pl" .. tostring(i))
    l_Player["add"    ] = g_Smgr:getscenenodefromname("button_add_pl" .. tostring(i))
    l_Player["del"    ] = g_Smgr:getscenenodefromname("button_del_pl" .. tostring(i))
    l_Player["name"   ] = g_Smgr:getguiitemfromname  ("label_pl" .. tostring(i))
    
    if l_Player["label"] ~= nil then
      l_Player["label"]:setvisible(false)
    end
    
    if l_Player["del"] ~= nil then
      l_Player["del"]:setvisible(false)
    end
    
    if i ~= 1 and l_Player["add"] ~= nil then
      l_Player["add"]:setvisible(false)
    end
    
    table.insert(g_Players, l_Player)
  end
  
  for i = 1,8 do
    local l_Player = { }
    l_Player["key_select"] = "button_player" .. tostring(i)
    l_Player["select"    ] = g_Smgr:getguiitemfromname(l_Player["key_select"])
    l_Player["key_mesh"  ] = "mesh_player" .. tostring(i)
    l_Player["mesh"      ] = g_Smgr:getscenenodefromname(l_Player["key_mesh"])
    l_Player["name"      ] = ""
    table.insert(g_PlayerDialog, l_Player)
  end
  
  startFadeIn(g_Root)
end

function cleanup()
  g_Smgr:clear()
end

function step(a_Time)
  local l_Time = a_Time - g_Time
  g_Time = a_Time
  processanimation()
end

function uielementhovered(a_Id, a_Name)
end

function uielementleft(a_Id, a_Name)
end

function uibuttonclicked(a_Id, a_Name)
  if g_Buttons[a_Name] ~= nil then
    local l_Key  = g_Buttons[a_Name]["key" ]
    local l_Type = g_Buttons[a_Name]["type"]

    if g_Options[l_Key] ~= nil then
      if l_Type == "plus" or l_Type == "minus" then
        if l_Type == "plus" then
          if g_Options[l_Key]["value"] < #g_Options[l_Key]["options"] then
            g_Options[l_Key]["value"] = g_Options[l_Key]["value"] + 1
          end
        else
          if g_Options[l_Key]["value"] > 1 then
            g_Options[l_Key]["value"] = g_Options[l_Key]["value"] - 1
          end
        end
        
        if g_Options[l_Key]["node"] ~= nil then
          g_Options[l_Key]["node"]:settext(g_Options[l_Key]["options"][g_Options[l_Key]["value"]])
        end
      elseif l_Type == "toggle" then
        g_Options[l_Key]["value"] = not g_Options[l_Key]["value"]
        if g_Buttons[a_Name]["node"] ~= nil then
          if g_Options[l_Key]["value"] then
            g_Buttons[a_Name]["node"]:setrotation({ x = 0, y = 0, z = 0 })
          else
            g_Buttons[a_Name]["node"]:setrotation({ x = 0, y = 180, z = 0 })
          end
        end
      else
        io.write("Unknown button type \"" .. tostring(l_Type) .. "\"\n")
      end
    else
      io.write("Unknown option \"" .. tostring(l_Key) .. "\"\n")
    end
  elseif a_Name == "button_cancel_players" then
    startRotation(g_Root, g_Time, -90,  0)
    startRotation(g_Plrs, g_Time,   0, 90)
  else
    if a_Name == "button_ok" then
      system:pushscript("data/lua/menu_trackselect.lua")
      startFadeOut(g_Root, g_Time, 1)
    elseif a_Name == "button_cancel" then
      startFadeOut(g_Root, g_Time, 1)
    else
      local l_Unknown = true
      for i = 1, #g_Players do
        if a_Name == g_Players[i]["key_add"] then
          io.write("Add Player #" .. tostring(i) .. "\n")
          
          local l_Index = 1
          
          for i = 1, #g_PlayerNames do
            io.write("Player \"" .. g_PlayerNames[i]["name"] .. "\" assigned: " .. tostring(g_PlayerNames[i]["assigned"]) .. "\n")
            if not g_PlayerNames[i]["assigned"] then
              g_PlayerDialog[l_Index]["mesh"]:setvisible(true)
              if g_PlayerDialog[l_Index]["select"] ~= nil then
                g_PlayerDialog[l_Index]["select"]:settext(g_PlayerNames[i]["name"])
                g_PlayerDialog[l_Index]["name"  ] = g_PlayerNames[i]["name"]
                l_Index = l_Index + 1
              end
            end
          end
          
          -- Now hide all unnecessary players
          for i = l_Index, #g_PlayerDialog do
            g_PlayerDialog[i]["mesh"]:setvisible(false)
          end
          
          startRotation(g_Root, g_Time,  0, -90)
          startRotation(g_Plrs, g_Time, 90,   0)
          l_Unknown = false
        elseif a_Name == g_Players[i]["key_del"] then
          io.write("Del Player #" .. tostring(i) .. "\n")
          
          local l_Name = g_Players[i]["name"]:gettext()
          
          for j = i, 8 do
            if j ~= 8 then
              g_Players[j]["label"]:setvisible(g_Players[j + 1]["label"]:isvisible())
              g_Players[j]["add"  ]:setvisible(g_Players[j + 1]["add"  ]:isvisible())
              g_Players[j]["del"  ]:setvisible(g_Players[j + 1]["del"  ]:isvisible())
              
              g_Players[j]["name"]:settext(g_Players[j + 1]["name"]:gettext())
            else
              g_Players[j]["label"]:setvisible(false )
              g_Players[j]["add"  ]:setvisible(g_Players[7]["label"]:isvisible())
              g_Players[j]["del"  ]:setvisible(false )
            end
          end
          
          for j = 1, #g_PlayerNames do
            if g_PlayerNames[j]["name"] == l_Name then
              g_PlayerNames[j]["assigned"] = false
              break
            end
          end
          
          l_Unknown = false
        end
      end
      
      for k,v in pairs(g_PlayerDialog) do
        -- Player was added to the starting grid
        if a_Name == v["key_mesh"] then
          -- Now we iterate the player list UI...
          for i = 1, 8 do
            -- ...and the first invisble label is the next to add
            if not g_Players[i]["label"]:isvisible() then
              g_Players[i]["label"]:setvisible(true)
              g_Players[i]["del"  ]:setvisible(true)
              g_Players[i]["add"  ]:setvisible(false)
              g_Players[i]["name" ]:settext(v["name"])
              
              -- Mark the player in the player name list as "assigned"
              for k2, v2 in pairs(g_PlayerNames) do
                if v2["name"] == v["name"] then
                  v2["assigned"] = true
                  break
                end
              end
              
              if i < 8 then
                g_Players[i + 1]["add"]:setvisible(true)
              end
              
              startRotation(g_Root, g_Time, -90,  0)
              startRotation(g_Plrs, g_Time,   0, 90)
              break
            end
          end
          
          l_Unknown = false
        end
      end
      
      if l_Unknown then
        io.write("Unknown button \"" .. tostring(a_Name) .. "\"\n")
      end
    end
  end
end