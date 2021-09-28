-- (w) 2021 by Dustbin::Games / Christian Keimel / This file is licensed under the terms of the zlib license: https://opensource.org/licenses/Zlib

system:executeluascript("data/lua/default_texture.lua")
system:executeluascript("data/lua/dialog_confirm.lua")
system:executeluascript("data/lua/helpers_main.lua")
system:executeluascript("data/lua/serializer.lua")

function initialize()
  io.write("Main Menu script started.\n")
  
  g_Smgr = system:getscenemanager()
  g_Smgr:loadscene("data/scenes/skybox.xml")
  
  g_Camera = g_Smgr:addcamera()

  g_Camera:setposition({ x = 0.0, y = 0.0, z =  0.0 })
  g_Camera:setupvector({ x = 0.0, y = 1.0, z =  0.0 })
  g_Camera:settarget  ({ x = 0.0, y = 0.0, z = 35.0 })
  g_Camera:activate()
  
  -- dialog:addlayoutraster()
  dialog:loaddialog("data/menu/menu_main.xml"     )
  dialog:loaddialog("data/menu/version.xml"       )
  dialog:loaddialog("data/menu/dialog_confirm.xml")
  
  dialog:createui();
  
  audio:startsoundtrack(0)
end

function cleanup()
  g_Smgr:clear()
  dialog:clear()
end

function uibuttonclicked(a_Id, a_Name)
  confirmDialog_handleButton(a_Id, a_Name)
  
  if a_Name == "settings" then
    system:pushscript("data/lua/menu_settings.lua")
    system:statechange(1)
  elseif a_Name == "profiles" then
    system:pushscript("data/lua/menu_profiles.lua")
    system:statechange(1)
  elseif a_Name == "free_racing" then
    local l_Profiles = system:getsetting("profiles")
    
    if l_Profiles ~= "" then
      local s = "g_Players = " .. l_Profiles
      system:executeluastring(s)
    else
      g_Players = { }
    end
    
    if #g_Players == 0 then
      showConfirmDialog("No player profiles found.\n\nPlease go to the \"Profiles\" dialog and add player profiles. When you have added one or more profiles you can start playing.")
    else
      system:pushscript("data/lua/menu_setupgame.lua")
      system:statechange(1)
    end
  elseif a_Name == "cup" then
    system:pushscript("data/lua/menu_setupgame.lua")
    system:statechange(1)
  elseif a_Name == "view_track" then
    local l_ViewData = system:createtable("SChampionShip")
    
    local l_Viewport = system:createtable("SViewPort")
    l_Viewport["playerid"] = 0
    
    local l_Size = system:getscreensize()
    
    l_Viewport["rect"]["upperleftcorner" ]["x"] = 0
    l_Viewport["rect"]["upperleftcorner" ]["y"] = 0
    l_Viewport["rect"]["lowerrightcorner"]["x"] = l_Size["x"]
    l_Viewport["rect"]["lowerrightcorner"]["y"] = l_Size["y"]
    
    table.insert(l_ViewData["viewports"], l_Viewport)
    
    l_ViewData["players"] = { }
    
    for i = 1, 16 do
      local l_Player = { }
      l_Player["name"] = "Dummy #" .. tostring(i)
      l_Player["controls"] = ""
      l_Player["playerid"] = i
      l_Player["texture" ] = getDefaultTexture(i, 0)
      
      table.insert(l_ViewData["players"], l_Player)
    end
    
    l_ViewData["races"] = { }
    l_ViewData["class"] = ""
    
    system:setglobal("championship", serializeTable(l_ViewData, 2))
    
    system:pushscript("data/lua/menu_selecttrack.lua")
    system:statechange(1)
  elseif a_Name == "exit" then
    system:statechange(255)
  else
    io.write("Lua: Button clicked: " .. a_Name .. " (" .. tostring(a_Id) .. ")\n")
  end
end
