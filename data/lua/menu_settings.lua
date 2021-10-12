-- (w) 2021 by Dustbin::Games / Christian Keimel / This file is licensed under the terms of the zlib license: https://opensource.org/licenses/Zlib

system:executeluascript("data/lua/splitstring.lua")
system:executeluascript("data/lua/serializer.lua")

g_Time = 0  -- The time of the last "step" message

-- UI Items
g_ResolutionList = { }
g_Settings = {
  resolution   = 9,
  fullscreen   = false,
  shadows      = 2,
  ambientlight = 3
}

g_ActiveTab = "gfx"

g_Items    = { }
g_Controls = { }

g_CtrlSettings = ""

function updateSettings()
  -- Update graphics settings
  g_Settings["gfx_shadows"     ] = g_Controls["Shadows"   ]:getselected()
  g_Settings["gfx_ambientlight"] = g_Controls["Ambient"   ]:getselected()
  g_Settings["gfx_fullscreen"  ] = g_Controls["Fullscreen"]:ischecked()
  g_Settings["ingame_rearview" ] = g_Controls["Rearview"  ]:ischecked()
  g_Settings["ingame_laptimes" ] = g_Controls["Laptimes"  ]:ischecked()
  g_Settings["ingame_ranking"  ] = g_Controls["Ranking"   ]:ischecked()
  g_Settings["ingame_racetime" ] = g_Controls["Racetime"  ]:ischecked()
  
  local l_Screen = g_Controls["Resolution"]:gettext()
  
  io.write("Screen: " .. tostring(l_Screen) .. "\n")
  local l_Resolution = split(l_Screen, "x")
  
  g_Settings["gfx_resolution_w"] = tonumber(l_Resolution[1])
  g_Settings["gfx_resolution_h"] = tonumber(l_Resolution[2])
  
  
  -- Update sound settings
  g_Settings["sfx_master"    ] = g_Controls["sfx_master"    ]["edit"]:getvalue()
  g_Settings["sfx_soundtrack"] = g_Controls["sfx_soundtrack"]["edit"]:getvalue()
  g_Settings["sfx_menu"      ] = g_Controls["sfx_menu"      ]["edit"]:getvalue()
  g_Settings["sfx_game"      ] = g_Controls["sfx_game"      ]["edit"]:getvalue()
  
  -- Update misc settings
  g_Settings["misc_usemenuctrl"] = g_Controls["misc_use"]:ischecked()
  g_Settings["misc_menuctrl"   ] = g_Ctrl:gettext()
end

function fillItems()
  g_Items = { }
  
  g_Items["gfx"] = { }
  g_Items["gfx"]["headline"   ] = dialog:getitemfromname("headline_gfx")
  g_Items["gfx"]["gfx_general"] = dialog:getitemfromname("gfx_general" )
  g_Items["gfx"]["gfx_ingame" ] = dialog:getitemfromname("gfx_ingame"  )
  
  g_Items["sfx"] = { }
  g_Items["sfx"]["headline"] = dialog:getitemfromname("sfx_headline")
  g_Items["sfx"]["settings"] = dialog:getitemfromname("sfx_general" )
  
  g_Items["misc"] = { }
  g_Items["misc"]["headline" ] = dialog:getitemfromname("misc_headline")
  g_Items["misc"]["settarget"] = dialog:getitemfromname("misc_general" )
  
  g_Controls = { }
  
  g_Controls["sfx_master"] = { }
  g_Controls["sfx_master"]["edit" ] = dialog:getitemfromname("sfx_master")
  g_Controls["sfx_master"]["label"] = dialog:getitemfromname("sfx_master_value")
  g_Controls["sfx_master"]["edit" ]:setvalue(g_Settings["sfx_master"])
  
  g_Controls["sfx_soundtrack"] = { }
  g_Controls["sfx_soundtrack"]["edit" ] = dialog:getitemfromname("sfx_soundtrack")
  g_Controls["sfx_soundtrack"]["label"] = dialog:getitemfromname("sfx_soundtrack_value")
  g_Controls["sfx_soundtrack"]["edit" ]:setvalue(g_Settings["sfx_soundtrack"])
  
  g_Controls["sfx_menu"] = { }
  g_Controls["sfx_menu"]["edit" ] = dialog:getitemfromname("sfx_menu")
  g_Controls["sfx_menu"]["label"] = dialog:getitemfromname("sfx_menu_value")
  g_Controls["sfx_menu"]["edit" ]:setvalue(g_Settings["sfx_menu"])
  
  g_Controls["sfx_game"] = { }
  g_Controls["sfx_game"]["edit" ] = dialog:getitemfromname("sfx_game")
  g_Controls["sfx_game"]["label"] = dialog:getitemfromname("sfx_game_value")
  g_Controls["sfx_game"]["edit" ]:setvalue(g_Settings["sfx_game"])
  
  g_Controls["Resolution"] = dialog:getitemfromname("Resolution")
  
  local l_Resolution = tostring(g_Settings["gfx_resolution_w"]) .. "x" .. tostring(g_Settings["gfx_resolution_h"])
  
  if g_Controls["Resolution"] ~= nil then
    g_Controls["Resolution"]:clearitems()
    
    local l_List = system:getresolutionlist()
    
    for i = 0, l_List:count() - 1 do
      local s = tostring(l_List:get(i))
      g_Controls["Resolution"]:additem(s)
      
      if s == l_Resolution then
        g_Controls["Resolution"]:setselected(i)
      end
    end
  end
  
  g_Controls["Shadows"] = dialog:getitemfromname("Shadows")
  g_Controls["Shadows"]:setselected(g_Settings["gfx_shadows"])
  
  g_Controls["Ambient"] = dialog:getitemfromname("Ambient")
  g_Controls["Ambient"]:setselected(g_Settings["gfx_ambientlight"])
  
  g_Controls["Fullscreen"] = dialog:getitemfromname("Fullscreen")
  g_Controls["Fullscreen"]:setchecked(g_Settings["gfx_fullscreen"])
  
  g_Controls["Rearview"] = dialog:getitemfromname("RearviewCamera")
  g_Controls["Rearview"]:setchecked(g_Settings["ingame_rearview"])
  
  g_Controls["Ranking"] = dialog:getitemfromname("Ranking")
  g_Controls["Ranking"]:setchecked(g_Settings["ingame_ranking"])
  
  g_Controls["Racetime"] = dialog:getitemfromname("Racetime")
  g_Controls["Racetime"]:setchecked(g_Settings["ingame_racetime"])
  
  g_Controls["Laptimes"] = dialog:getitemfromname("Laptimes")
  g_Controls["Laptimes"]:setchecked(g_Settings["ingame_laptimes"])
  
  g_Controls["misc_use"] = dialog:getitemfromname("misc_use")
  g_Controls["misc_use"]:setchecked(g_Settings["misc_usemenuctrl"])
  
  g_Ctrl = dialog:getitemfromname("controller_ui")
  
  if g_CtrlSettings == "" then
    g_CtrlSettings = g_Settings["misc_menuctrl"]

    if g_CtrlSettings == "" then
      g_CtrlSettings = system:getcontrollerdata_menu()
    end
  end
  
  g_Ctrl:settext(g_CtrlSettings)
  
  showHideUi()
end

function initialize()
  io.write("Setup script started.\n")
  
  g_Smgr = system:getscenemanager()
  g_Smgr:loadscene("data/scenes/skybox.xml")
  
  g_Camera = g_Smgr:addcamera()

  g_Camera:setposition({ x = 0.0, y = 0.0, z = 30.0 })
  g_Camera:setupvector({ x = 0.0, y = 1.0, z =  0.0 })
  g_Camera:settarget  ({ x = 0.0, y = 0.0, z = 70.0 })
  g_Camera:activate()
  
  dialog:loaddialog("data/menu/menu_settings.xml")
  dialog:loaddialog("data/menu/button_cancel.xml")
  dialog:loaddialog("data/menu/button_ok.xml")
  
  dialog:createui();
  audio:startsoundtrack(0)
  
  g_Settings = system:getsettings()
  
  fillItems()
end

function cleanup()
  g_Smgr:clear()
  dialog:clear()  
end

function fillResolution(a_Start)
end

function showHideUi()
  for k,v in pairs(g_Items) do
    for k2, v2 in pairs(v) do
      if k == g_ActiveTab then
        v2:setvisible(true)
      else
        v2:setvisible(false)
      end
    end
  end
end

function uibuttonclicked(a_Id, a_Name)
  if a_Name == "ok" then
    updateSettings()
    io.write(serializeTable(g_Settings, 2))
    system:setsettings(g_Settings)
    system:statechange(1)
  elseif a_Name == "cancel" then
    system:statechange(1)
  elseif a_Name == "gfx" or a_Name == "sfx" or a_Name == "misc" then
    g_ActiveTab = a_Name
    showHideUi()
  else
    io.write("Button clicked: \"" .. a_Name .. "\" (" .. tostring(a_Id) .. ")\n")
  end
end

function uivaluechanged(a_Id, a_Name, a_Value)
  if g_Controls[a_Name] ~= nil and g_Controls[a_Name]["label"] ~= nil then
    g_Controls[a_Name]["label"]:settext(string.format("%.0f%%", a_Value / 10.0))
    
    if a_Name == "sfx_master" then
      audio:setmastervolume(a_Value / 1000.0)
    elseif a_Name == "sfx_soundtrack" then
      audio:setsoundtrackvolume(a_Value / 1000.0)
    elseif a_Name == "sfx_menu" then
      audio:setsfxvolumemenu(a_Value / 1000.0)
    elseif a_Name == "sfx_game" then
      audio:setsfxvolumegame(a_Value / 1000.0)
    else
      io.write("Unkown scrollbar \"" .. a_Name .. "\"\n")
    end
  else
    io.write("Value Changed: \"" .. a_Name .. "\" (" .. tostring(a_Id) .. ") = " .. tostring(a_Value) .. "\n")
  end
end

function windowresized()
  fillItems()
end

function beforeresize()
  -- Save the controller settings, they might have
  -- changed and need to be restored when the
  -- UI is rebuilt due to window resize
  g_CtrlSettings = g_Ctrl:gettext()
end
