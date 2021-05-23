system:executeluascript("data/lua/helpers_main.lua")

g_Time = 0  -- The time of the last "step" message

g_Movement = {
}

function initialize()
  io.write("Main Menu script started.\n")
  
  g_Smgr = system:getscenemanager()
  g_Smgr:loadscene("data/menu3d/menu_main.xml")
  
  g_Camera = g_Smgr:addcamera()

  g_Camera:setposition({ x = 0.0, y = 0.0, z =  0.0 })
  g_Camera:setupvector({ x = 0.0, y = 1.0, z =  0.0 })
  g_Camera:settarget  ({ x = 0.0, y = 0.0, z = 35.0 })
  g_Camera:activate()
  
  g_Root = g_Smgr:getscenenodefromname("root_menumain")
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
  io.write("uielementhovered: " .. tostring(a_Id) .. ", " .. tostring(a_Name) .. "\n")
end

function uielementleft(a_Id, a_Name)
  io.write("uielementleft: " .. tostring(a_Id) .. ", " .. tostring(a_Name) .. "\n")
end

function uibuttonclicked(a_Id, a_Name)
  if a_Name == "BtnSingleRace" then
    system:pushscript("data/lua/menu_setupgame.lua")
    startFadeOut(g_Root, g_Time, 1)
  elseif a_Name == "BtnSettings" then
    system:pushscript("data/lua/menu_settings.lua")
    startFadeOut(g_Root, g_Time, 1)
  elseif a_Name == "BtnExit" then
    system:pushscript("data/lua/menu_goodbye.lua")
    startFadeOut(g_Root, g_Time, 1)
  else
    io.write("uibuttonclicked: " .. tostring(a_Id) .. ", " .. tostring(a_Name) .. "\n")
  end
end