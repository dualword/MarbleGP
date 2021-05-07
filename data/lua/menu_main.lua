g_Time = 0  -- The time of the last "step" message

function initialize()
  io.write("Main Menu script started.\n")
  -- dialog:loadscene("data/menu3d/menu_main.xml")
  -- dialog:loadscene("data/menu3d/menu_trackselect.xml")
  dialog:loadscene("data/menu3d/menu_setupgame.xml")
  
  g_Camera = LuaCameraObject:new()
  g_Camera:initialize()
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
  io.write("uibuttonclicked: " .. tostring(a_Id) .. ", " .. tostring(a_Name) .. "\n")
end