system:executeluascript("data/lua/splitstring.lua")
system:executeluascript("data/lua/serializer.lua")
system:executeluascript("data/lua/dialog_confirm.lua")

g_Time = 0  -- The time of the last "step" message

-- UI Items

g_Items   = { }
g_Buttons = { }
g_Players = { }
g_Edits   = { }

function fillItems()
  g_Items   = { }
  g_Buttons = { }
  g_Edits   = { }
  
  local l_NewId = 30000
  
  for i = 1, 8 do
    local sName = "root_player" .. tostring(i)
    
    local l_Item = dialog:getitemfromname(sName)
    
    local l_MyGui = { }
    l_MyGui["root"] = l_Item
    
    for j = 1, l_Item:getchildcount() do
      local l_Child = l_Item:getchild(j)
      
      io.write("==> " .. l_Child:getname() .. "\n")
      
      if l_Child:getname() == "tab" then
        l_MyGui["tab"] = l_Child
        
        for k = 1, l_Child:getchildcount() do
          local l_GrandChild = l_Child:getchild(k)
          
          if l_GrandChild:getname() == "btn_delete" then
            l_GrandChild:setid(l_NewId)
            g_Buttons[l_NewId] = { }
            g_Buttons[l_NewId]["action"] = "delete"
            g_Buttons[l_NewId]["index" ] = i
            l_NewId = l_NewId + 1
          elseif l_GrandChild:getname() == "btn_controls" then
            l_GrandChild:setid(l_NewId)
            g_Buttons[l_NewId] = { }
            g_Buttons[l_NewId]["action"] = "controls"
            g_Buttons[l_NewId]["index" ] = i
            l_NewId = l_NewId + 1
          elseif l_GrandChild:getname() == "btn_texture" then
            l_GrandChild:setid(l_NewId)
            g_Buttons[l_NewId] = { }
            g_Buttons[l_NewId]["action"] = "texture"
            g_Buttons[l_NewId]["index" ] = i
            l_NewId = l_NewId + 1
          elseif l_GrandChild:getname() == "edit_name" then
            l_GrandChild:setid(l_NewId)
            g_Edits[l_NewId] = { }
            g_Edits[l_NewId]["item" ] = l_GrandChild
            g_Edits[l_NewId]["index"] = i
            
            l_NewId = l_NewId + 1
            l_MyGui["edit_name"] = l_GrandChild
          elseif l_GrandChild:getname() == "label_controls" then
            l_MyGui["label_controls"] = l_GrandChild
          elseif l_GrandChild:getname() == "label_texture" then
            l_MyGui["label_texture"] = l_GrandChild
          elseif l_GrandChild:getname() == "warning_yellow" then
            l_GrandChild:setvisible(false)
            l_MyGui["warning_yellow"] = l_GrandChild
          elseif l_GrandChild:getname() == "warning_red" then
            l_GrandChild:setvisible(false)
            l_MyGui["warning_red"] = l_GrandChild
          end
        end
      elseif l_Child:getname() == "btn_add" then
        l_Child:setid(l_NewId)
        g_Buttons[l_NewId] = { }
        g_Buttons[l_NewId]["action"] = "add"
        g_Buttons[l_NewId]["index" ] = i
        l_NewId = l_NewId + 1
        
        l_MyGui["btn_add"] = l_Child
      end
      
    end
    g_Items[i] = l_MyGui
  end
  
  io.write("****\n" .. serializeTable(g_Items, 2) .. "\n****\n")
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
  dialog:loaddialog("data/menu/dialog_confirm.xml")
  
  dialog:createui();
  audio:startsoundtrack(0)
  
  g_Settings = system:getsettings()
  
  local l_Profiles = system:getsetting("profiles")
  
  if l_Profiles ~= "" then
    local s = "g_Players = " .. l_Profiles
    system:executeluastring(s)
  end
  
  fillItems()
end

function cleanup()
  g_Smgr:clear()
  dialog:clear()  
end

function fillResolution(a_Start)
end

function showHideUi()
  for i = 1, #g_Items do
    if i <= #g_Players then
      g_Items[i]["root"   ]:setvisible(true )
      g_Items[i]["tab"    ]:setvisible(true )
      g_Items[i]["btn_add"]:setvisible(false)
      
      g_Items[i]["edit_name"]:settext(g_Players[i]["name"])
    elseif i == #g_Players + 1 then
      g_Items[i]["root"   ]:setvisible(true )
      g_Items[i]["tab"    ]:setvisible(false)
      g_Items[i]["btn_add"]:setvisible(true )
    else
      g_Items[i]["root"   ]:setvisible(false)
      g_Items[i]["tab"    ]:setvisible(false)
      g_Items[i]["btn_add"]:setvisible(false)
    end
  end
end

function uibuttonclicked(a_Id, a_Name)
  confirmDialog_handleButton(a_Id, a_Name)
  
  if a_Name == "ok" then
    local l_Alright = true
    local l_Message = ""
    
    for i = 1, #g_Players do
      for j = 1, #g_Players do
        if i ~= j then
          if g_Players[i]["name"] == g_Players[j]["name"] then
            l_Message = "The name \"" .. g_Players[i]["name"] .. "\" is used twice.\n"
            l_Alright = false
            break
          end
        end
      end
      
      io.write(g_Players[i]["name"] .. " --> " .. tostring(#g_Players[i]["name"]) .. "\n")
      if #g_Players[i]["name"] < 4 then
        l_Message = "The name \"" .. g_Players[i]["name"] .. "\" is too short (minimum is 4 character)."
        l_Alright = false
      end
    end
    
    if l_Alright then
      system:setsetting("profiles", serializeTable(g_Players, 2))
      system:statechange(1)
    else
      io.write("l_Message: " .. l_Message .. "\n")
      showConfirmDialog(l_Message)
    end
  elseif a_Name == "cancel" then
    system:statechange(1)
  else
    if g_Buttons[a_Id] ~= nil then
      if g_Buttons[a_Id]["action"] == "add" then
        local l_Name  = "Player 1"
        local l_Count = 1
        
        for i = 1, #g_Players do
          if g_Players[i]["name"] == l_Name then
            l_Count = l_Count + 1
            l_Name  = "Player " .. tostring(l_Count)
          end
        end
        
        local l_Player = { }
        l_Player["name"    ] = l_Name
        l_Player["controls"] = ""
        l_Player["texture" ] = ""
        
        table.insert(g_Players, l_Player)
        showHideUi()
      elseif g_Buttons[a_Id]["action"] == "delete" then
        table.remove(g_Players, g_Buttons[a_Id]["index"])
        showHideUi()
      else
        io.write("Button with action \"" .. g_Buttons[a_Id]["action"] .. "\" for player #" .. tostring(g_Buttons[a_Id]["index"]) .. " clicked.\n")
      end
    end
  end
end

function resetWarnings()
  for i = 1, #g_Items do
    g_Items[i]["warning_yellow"]:setvisible(false)
    g_Items[i]["warning_red"   ]:setvisible(false)
  end
end

function uitextchanged(a_Id, a_Name, a_NewValue)
  if g_Edits[a_Id] ~= nil then
    local l_Player  = g_Edits[a_Id]["index"]
    local l_Alright = true
    if g_Items[l_Player] ~= nil then
      resetWarnings()
      
      if #a_NewValue < 4 then
        if g_Items[l_Player]["warning_yellow"] ~= nil then
          g_Items[l_Player]["warning_yellow"]:settooltip("Name too short. 4 Characters are minimum.")
          g_Items[l_Player]["warning_yellow"]:setvisible(true)
          l_Alright = false
        end
      end
      
      for i = 1, #g_Players do
        if i ~= l_Player and g_Players[i]["name"] == a_NewValue then
          g_Items[l_Player]["warning_red"]:settooltip("Player names must be unique.")
          g_Items[l_Player]["warning_red"]:setvisible(true)
          
          g_Items[i]["warning_yellow"]:settooltip("Player names must be unique.")
          g_Items[i]["warning_yellow"]:setvisible(true)
        end
      end
      
      g_Players[l_Player]["name"] = a_NewValue
    end
  end
end

function uivaluechanged(a_Id, a_Name, a_Value)
end

function windowresized()
  fillItems()
end
