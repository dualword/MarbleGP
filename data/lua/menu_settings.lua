io.write("**** SETTINGS\n")

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
  
  g_Controls["sfx_soundtrack"] = { }
  g_Controls["sfx_soundtrack"]["edit" ] = dialog:getitemfromname("sfx_soundtrack")
  g_Controls["sfx_soundtrack"]["label"] = dialog:getitemfromname("sfx_soundtrack_value")
  
  g_Controls["sfx_menu"] = { }
  g_Controls["sfx_menu"]["edit" ] = dialog:getitemfromname("sfx_menu")
  g_Controls["sfx_menu"]["label"] = dialog:getitemfromname("sfx_menu_value")
  
  g_Controls["sfx_game"] = { }
  g_Controls["sfx_game"]["edit" ] = dialog:getitemfromname("sfx_game")
  g_Controls["sfx_game"]["label"] = dialog:getitemfromname("sfx_game_value")
  
  g_Controls["Resolution"] = dialog:getitemfromname("Resolution")
  
  if g_Controls["Resolution"] ~= nil then
    g_Controls["Resolution"]:clearitems()
    
    local l_List = system:getresolutionlist()
    
    for i = 0, l_List:count() - 1 do
      local s = tostring(l_List:get(i))
      g_Controls["Resolution"]:additem(s)
    end
  end
  
  showHideUi()
end

function initialize()
  io.write("Setup script started.\n")
  
  g_Smgr = system:getscenemanager()
  g_Smgr:loadscene("data/menu3d/skybox.xml")
  
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
  else
    io.write("Value Changed: \"" .. a_Name .. "\" (" .. tostring(a_Id) .. ") = " .. tostring(a_Value) .. "\n")
  end
end

function windowresized()
  fillItems()
end
