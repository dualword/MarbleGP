system:executeluascript("data/lua/helpers_main.lua")

g_Time = 0  -- The time of the last "step" message

g_FinishTime = -1
g_StartTime  = -1

function initialize()
  io.write("Edit Profiles script started.\n")
  
  g_Smgr = system:getscenemanager()
  g_Smgr:loadscene("data/menu3d/menu_editprofile.xml")
  
  g_Root = g_Smgr:getscenenodefromname("root_editprofile")
  
  g_Smgr:loadscene("data/menu3d/dialog_players.xml")
  
  g_Plrs = g_Smgr:getscenenodefromname("root_players")
  
  g_Root:setrotation({ x = 0, y = 90, z = 0 })
  
  g_Camera = g_Smgr:addcamera()
  
  g_Camera:setposition({ x = 0.0, y = 0.0, z = 30.0 })
  g_Camera:setupvector({ x = 0.0, y = 1.0, z =  0.0 })
  g_Camera:settarget  ({ x = 0.0, y = 0.0, z = 70.0 })
  g_Camera:activate()
  
  startFadeIn(g_Root)
  startFadeIn(g_Plrs)
end

function cleanup()
  g_Smgr:clear()
end

function step(a_Time)
  local l_Time = a_Time - g_Time
  g_Time = a_Time
  
  if g_StartTime == -1 then
    g_StartTime = g_Time
  end
    
  processanimation()
end

function uielementhovered(a_Id, a_Name)
end

function uielementleft(a_Id, a_Name)
end

function uibuttonclicked(a_Id, a_Name)
  if a_Name == "button_ok" then
    g_FinishTime = g_Time
    startFadeOut(g_Root, g_Time, 1)
  end
end

function uivaluechanged(a_Id, a_Name, a_Value)
  if g_Sliders[a_Name] ~= nil then
    g_Sliders[a_Name]["value"] = tonumber(a_Value)
    if g_Sliders[a_Name]["label"] ~= nil then
      g_Sliders[a_Name]["label"]:settext(string.format("%.1f %%", tonumber(a_Value)))
      if g_Sliders[a_Name]["key"] == "LabelSTrack" then
        audio:setsoundtrackvolume(a_Value / 100.0)
      elseif g_Sliders[a_Name]["key"] == "LabelSfx" then
        audio:setsfxvolume(a_Value / 100.0)
      end
    end
  end
end