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

function fillItems()
  g_Items = { }
  
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
  
  dialog:loaddialog("data/menu/menu_profiles.xml")
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
    system:statechange(1)
  elseif a_Name == "cancel" then
    system:statechange(1)
    io.write("Button clicked: \"" .. a_Name .. "\" (" .. tostring(a_Id) .. ")\n")
  end
end

function uivaluechanged(a_Id, a_Name, a_Value)
end

function windowresized()
  fillItems()
end
