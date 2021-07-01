system:executeluascript("data/lua/helpers_main.lua")

function initialize()
  io.write("Main Menu script started.\n")
  
  g_Smgr = system:getscenemanager()
  g_Smgr:loadscene("data/menu3d/skybox.xml")
  
  g_Camera = g_Smgr:addcamera()

  g_Camera:setposition({ x = 0.0, y = 0.0, z =  0.0 })
  g_Camera:setupvector({ x = 0.0, y = 1.0, z =  0.0 })
  g_Camera:settarget  ({ x = 0.0, y = 0.0, z = 35.0 })
  g_Camera:activate()
  
  -- dialog:addlayoutraster()
  dialog:loaddialog("data/menu/menu_main.xml")
  
  dialog:createui();
  
  audio:startsoundtrack(0)
end

function cleanup()
  g_Smgr:clear()
  dialog:clear()
end

function uibuttonclicked(a_Id, a_Name)
  io.write("Lua: Button clicked: " .. a_Name .. " (" .. tostring(a_Id) .. ")\n")
end
