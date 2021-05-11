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
  gameclass = { value = 1, options = g_GameClasses  , label = "label_class" },
  aiclass   = { value = 1, options = g_GameClasses  , label = "label_ai"    },
  grid      = { value = 1, options = g_GridPositions, label = "label_grid"  },
  
  gridreverse    = { value = true },
  autoend        = { value = true },
  fillgrid       = { value = true },
  randomizefirst = { value = true }
}

g_Buttons = {
  button_class_prev = { key = "gameclass"     , type = "minus"  },
  button_class_next = { key = "gameclass"     , type = "plus"   },
  button_ai_prev    = { key = "aiclass"       , type = "minus"  },
  button_ai_next    = { key = "aiclass"       , type = "plus"   },
  button_grid_prev  = { key = "grid"          , type = "minus"  },
  button_grid_next  = { key = "grid"          , type = "plus"   },
  button_reverse    = { key = "gridreverse"   , type = "toggle" },
  button_autoend    = { key = "autoend"       , type = "toggle" },
  button_fillgrid   = { key = "fillgrid"      , type = "toggle" },
  button_randomize  = { key = "randomizefirst", type = "toggle" }
}

g_Smgr = nil  -- The Scene Manager LUA object

function initialize()
  io.write("Setup Game Menu script started.\n")
  dialog:loadscene("data/menu3d/menu_setupgame.xml")
  
  g_Smgr = LuaSceneManager:new()
  
  io.write("Smgr: " .. tostring(g_Smgr) .. "\n")
  
  g_Camera = g_Smgr:addcamera()
  g_Camera:setposition({ x = 0, y = 0, z = 0 })
  g_Camera:settarget({ x = 0, y = 0, z = 35 })
  g_Camera:setupvector({ x = 0, y = 1, z = 0 })
  g_Camera:activate()
  
  dialog:init3dgui()
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
      if l_Type == "plus" then
        if g_Options[l_Key]["value"] < #g_Options[l_Key]["options"] then
          g_Options[l_Key]["value"] = g_Options[l_Key]["value"] + 1
        end
        io.write(l_Key .. ": " .. tostring(g_Options[l_Key]["value"]) .. " (" .. g_Options[l_Key]["options"][g_Options[l_Key]["value"]] .. ")\n")
      elseif l_Type == "minus" then
        if g_Options[l_Key]["value"] > 1 then
          g_Options[l_Key]["value"] = g_Options[l_Key]["value"] - 1
        end
        io.write(l_Key .. ": " .. tostring(g_Options[l_Key]["value"]) .. " (" .. g_Options[l_Key]["options"][g_Options[l_Key]["value"]] .. ")\n")
      elseif l_Type == "toggle" then
        g_Options[l_Key]["value"] = not g_Options[l_Key]["value"]
        io.write(l_Key .. ": " .. tostring(g_Options[l_Key]["value"]) .. "\n")
      else
        io.write("Unknown button type \"" .. tostring(l_Type) .. "\"\n")
      end
    else
      io.write("Unknown option \"" .. tostring(l_Key) .. "\"\n")
    end
  else
    io.write("Unknown button \"" .. tostring(a_Name) .. "\"\n")
  end
end