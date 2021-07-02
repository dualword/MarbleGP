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

g_Items = {
}

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
end

function cleanup()
  g_Smgr:clear()
  dialog:clear()  
end

function fillResolution(a_Start)
end

function uibuttonclicked(a_Id, a_Name)
  if a_Name == "ok" then
    system:statechange(1)
  elseif a_Name == "cancel" then
    system:statechange(1)
  else
    io.write("Button clicked: \"" .. a_Name .. "\" (" .. tostring(a_Id) .. ")\n")
  end
end

function uivaluechanged(a_Id, a_Name, a_Value)
  io.write("Value Changed: \"" .. a_Name .. "\" (" .. tostring(a_Id) .. ") = " .. tostring(a_Value) .. "\n")
end