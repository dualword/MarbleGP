g_Time = 0  -- The time of the last "step" message

g_GameClasses = {
  [1] = "Rookies",
  [2] = "Marble3",
  [3] = "Marble2",
  [4] = "MarbleGP"
}

g_GridPositions = {
  [1] = "Last Race",
  [2] = "Championship",
  [3] = "Random"
}

g_Options = {
  gameclass = { value = 1, options = g_GameClasses  , label = "label_class_ui", node = nil },
  aiclass   = { value = 1, options = g_GameClasses  , label = "label_ai_ui"   , node = nil },
  grid      = { value = 1, options = g_GridPositions, label = "label_grid_ui" , node = nil },
  
  gridreverse    = { value = false },
  autoend        = { value = false },
  fillgrid       = { value = true  },
  randomizefirst = { value = true  }
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
  button_randomize  = { key = "randomizefirst", type = "toggle", node = nil }
}

g_Smgr = nil  -- The Scene Manager LUA object

function initialize()
  io.write("Setup Game Menu script started.\n")
  
  g_Smgr = system:getscenemanager()
  g_Smgr:loadscene("data/menu3d/menu_setupgame.xml")
  
  g_Camera = g_Smgr:addcamera()
  g_Camera:setposition(0, 0, 0)
  g_Camera:settarget(0, 0, 35)
  g_Camera:setupvector(0, 1, 0)
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
            v2["node"]:setrotation(0, 0, 0)
          else
            v2["node"]:setrotation(0, 180, 0)
          end
        end
      end
    end
  end
end

function cleanup()
  g_Smgr:clear()
end

function step(a_Time)
  local l_Time = a_Time - g_Time
  g_Time = a_Time
end

function uielementhovered(a_Id, a_Name)
  io.write("uielementhovered: " .. tostring(a_Id) .. ", " .. tostring(a_Name) .. "\n")
end

function uielementleft(a_Id, a_Name)
  io.write("uielementleft: " .. tostring(a_Id) .. ", " .. tostring(a_Name) .. "\n")
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
            g_Buttons[a_Name]["node"]:setrotation(0, 0, 0)
          else
            g_Buttons[a_Name]["node"]:setrotation(0, 180, 0)
          end
        end
      else
        io.write("Unknown button type \"" .. tostring(l_Type) .. "\"\n")
      end
    else
      io.write("Unknown option \"" .. tostring(l_Key) .. "\"\n")
    end
  else
    if a_Name == "button_ok" then
    elseif a_Name == "button_cancel" then
      system:pushscript("data/lua/menu_main.lua")
      system:statechange(1)
    else
      io.write("Unknown button \"" .. tostring(a_Name) .. "\"\n")
    end
  end
end