-- (w) 2021 by Dustbin::Games / Christian Keimel / This file is licensed under the terms of the zlib license: https://opensource.org/licenses/Zlib

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
  dialog:loaddialog("data/menu/menu_main.xml")
  dialog:loaddialog("data/menu/version.xml")
  
  dialog:createui();
  
  audio:startsoundtrack(0)
  
  local l_Test = {
    [1] = "SPlayerRank",
    [2] = "SPlayerResult",
    [3] = "SRace",
    [4] = "SChampionShip"
  }
  
  for k,v in pairs(l_Test) do
    io.write("\n*** Table test: " .. v .. "\n")
    local l_Test = system:tabletesttolua(v)
    
    if l_Test ~= nil then
      io.write("\n*********************\n")
      io.write(serializeTable(l_Test, 2))
      io.write("\n*********************\n")
      system:tabletesttocpp(l_Test, v)
    end
  end
end

function cleanup()
  g_Smgr:clear()
  dialog:clear()
end

function uibuttonclicked(a_Id, a_Name)
  if a_Name == "settings" then
    system:pushscript("data/lua/menu_settings.lua")
    system:statechange(1)
  elseif a_Name == "profiles" then
    system:pushscript("data/lua/menu_profiles.lua")
    system:statechange(1)
  elseif a_Name == "single_race" then
    system:pushscript("data/lua/menu_setupgame.lua")
    system:statechange(1)
  elseif a_Name == "cup" then
    system:pushscript("data/lua/menu_setupgame.lua")
    system:statechange(1)
  elseif a_Name == "exit" then
    system:statechange(255)
  else
    io.write("Lua: Button clicked: " .. a_Name .. " (" .. tostring(a_Id) .. ")\n")
  end
end
