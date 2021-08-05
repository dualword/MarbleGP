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
  dialog:loaddialog("data/menu/menu_setupgame.xml")
  dialog:loaddialog("data/menu/button_cancel.xml" )  -- The "Cancel" button
  dialog:loaddialog("data/menu/button_ok.xml"     )  -- The "OK" button
  
  dialog:createui();
  audio:startsoundtrack(0)
  fillItems()
end

function cleanup()
  g_Smgr:clear()
  dialog:clear()
end

function uibuttonclicked(a_Id, a_Name)
  if a_Name == "cancel" then
    system:clearscriptstack()
    system:statechange(1)
  end
end

function fillItems()
  g_Items = { }
  
  for i = 1, 8 do
    local l_Name  = "player" .. tostring(i)
    local l_Item  = dialog:getitemfromname(l_Name)
    local l_Items = { }
    
    for j = 1, l_Item:getchildcount() do
      local l_Child = l_Item:getchild(j)
      
      if l_Child:getname() == "player_label" then
        l_Child:settext(tostring(i))
      end
      
      l_Items[l_Child:getname()] = l_Child
      
      if l_Child:getname() == "player_tab" then
        for k = 1, l_Child:getchildcount() do
          local l_GrandChild = l_Child:getchild(k)
          l_Items[l_GrandChild:getname()] = l_GrandChild
        end
      end
    end
    
    table.insert(g_Items, l_Items)
  end
end

function windowresized()
  fillItems()
end
