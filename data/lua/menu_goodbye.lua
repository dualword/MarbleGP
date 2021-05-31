io.write("\n***************\n")
io.write("** Good Bye! **\n")
io.write("***************\n\n")

system:executeluascript("data/lua/helpers_main.lua")

g_Time  = 0
g_Start = -1

function initialize()
  g_Smgr = system:getscenemanager()
  g_Smgr:loadscene("data/menu3d/menu_bye.xml")
  
  g_Camera = g_Smgr:addcamera()

  g_Camera:setposition({ x = 0.0, y = 0.0, z =  0.0 })
  g_Camera:setupvector({ x = 0.0, y = 1.0, z =  0.0 })
  g_Camera:settarget  ({ x = 0.0, y = 0.0, z = 35.0 })
  g_Camera:activate()
  
  g_Root = g_Smgr:getscenenodefromname("goodbye")
  startFadeIn(g_Root)
  audio:startsoundtrack(2)
end

function step(a_Time)
  if g_Start == -1 then
    g_Start = a_Time
  end
  
  g_Time = a_Time
  processanimation()
  
  if g_Time - g_Start > 2500 then
    io.write("Exit!\n")
    system:statechange(255)
  elseif g_Time - g_Start > 1500 then
    local a_Fade = 1.0 - (g_Time - g_Start - 1500) / 1000
    audio:setsoundtrackfade(a_Fade)
  end
end
