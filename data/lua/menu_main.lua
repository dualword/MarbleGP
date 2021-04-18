
function initialize()
  io.write("Main Menu script started.\n")
  dialog:loadscene("data/menu3d/menu_main.xml")
  
  g_Camera = LuaCameraObject:new()
  g_Camera:initialize()
  g_Camera:setposition({ x = 0, y = 0, z = 10 })
  g_Camera:settarget({ x = 0, y = 0, z = 0 })
  g_Camera:setupvector({ x = 0, y = 1, z = 0 })
end