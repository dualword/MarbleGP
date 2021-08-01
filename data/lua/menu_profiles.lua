system:executeluascript("data/lua/splitstring.lua")
system:executeluascript("data/lua/serializer.lua")
system:executeluascript("data/lua/dialog_confirm.lua")

g_Time = 0  -- The time of the last "step" message

-- UI Items

g_Items   = { }
g_Buttons = { }
g_Players = { }
g_Edits   = { }
g_Control = nil
g_CtrlGui = nil
g_Texture = { }

g_TextureMode  = 0
g_PatternStart = 0

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
            if g_Players[i] ~= nil and g_Players[i]["controls"] ~= nil then
              l_GrandChild:settext(system:getfirstcontroller(g_Players[i]["controls"]))
            end
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
  
  g_Control = dialog:getitemfromname("controllerDialog")
  g_CtrlGui = dialog:getitemfromname("controller_ui")
  
  g_Color = { }
  
  g_Color["dialog"      ] = dialog:getitemfromname("color_dialog"   )
  g_Color["red"         ] = dialog:getitemfromname("value_red"      )
  g_Color["green"       ] = dialog:getitemfromname("value_green"    )
  g_Color["blue"        ] = dialog:getitemfromname("value_blue"     )
  g_Color["scroll_red"  ] = dialog:getitemfromname("scrollbar_red"  )
  g_Color["scroll_green"] = dialog:getitemfromname("scrollbar_green")
  g_Color["scroll_blue" ] = dialog:getitemfromname("scrollbar_blue" )
  g_Color["show"        ] = dialog:getitemfromname("color_display"  )
  
  g_Texture = { }
  
  g_Texture["dialog"        ] = dialog:getitemfromname("texture_dialog"     )
  g_Texture["tab_generate"  ] = dialog:getitemfromname("texture_generated"  )
  g_Texture["tab_imported"  ] = dialog:getitemfromname("texture_imported"   )
  g_Texture["pattern"       ] = dialog:getitemfromname("texture_pattern"    )
  g_Texture["nb_foreground" ] = dialog:getitemfromname("texture_fg_nb"      )
  g_Texture["nb_background" ] = dialog:getitemfromname("texture_bg_nb"      )
  g_Texture["pt_foreground" ] = dialog:getitemfromname("texture_fg_pt"      )
  g_Texture["pt_background" ] = dialog:getitemfromname("texture_bg_pt"      )
  g_Texture["number_frame"  ] = dialog:getitemfromname("texture_nf"         )
  g_Texture["number_ring"   ] = dialog:getitemfromname("texture_nr"         )
  g_Texture["texture"       ] = dialog:getitemfromname("imported_texture"   )
  g_Texture["texture_image" ] = dialog:getitemfromname("texture_image"      )
  g_Texture["pattern_dialog"] = dialog:getitemfromname("pattern_dialog"     )
  g_Texture["texture_mode"  ] = dialog:getitemfromname("texture_mode"       )
  g_Texture["imported_back" ] = dialog:getitemfromname("imported_name_back" )
  g_Texture["imported_name" ] = dialog:getitemfromname("imported_name_color")
  g_Texture["patterns"      ] = { }
  
  g_TextureColors = { }
  g_TextureColors["btn_select_fg_nb"     ] = dialog:getitemfromname("texture_fg_nb"      )
  g_TextureColors["btn_select_bg_nb"     ] = dialog:getitemfromname("texture_bg_nb"      )
  g_TextureColors["btn_select_nr"        ] = dialog:getitemfromname("texture_nr"         )
  g_TextureColors["btn_select_nf"        ] = dialog:getitemfromname("texture_nf"         )
  g_TextureColors["btn_select_fg_pt"     ] = dialog:getitemfromname("texture_fg_pt"      )
  g_TextureColors["btn_select_bg_pt"     ] = dialog:getitemfromname("texture_bg_pt"      )
  g_TextureColors["btn_select_name_color"] = dialog:getitemfromname("imported_name_color")
  g_TextureColors["btn_select_name_back" ] = dialog:getitemfromname("imported_name_back" )
  
  for i = 1, 9 do
    local l_Item = dialog:getitemfromnameandid("texture_pattern_image", i)
    g_Texture["patterns"][i] = l_Item
  end
  
  g_Patterns = system:gettexturepatterns()
  
  if g_Patterns["patterns"] ~= nil then
    for k,v in pairs(g_Patterns["patterns"]) do
      g_Texture["pattern"]:additem(v)
    end
    g_Texture["pattern"]:setselected(0)
  end
  
  fillPatterns()
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
  
  dialog:loaddialog("data/menu/menu_profiles.xml"        )  -- The main dialog for adding, editing and removing profiles
  dialog:loaddialog("data/menu/button_cancel.xml"        )  -- The "Cancel" button
  dialog:loaddialog("data/menu/button_ok.xml"            )  -- The "OK" button
  dialog:loaddialog("data/menu/dialog_confirm.xml"       )  -- The "confirm" dialog used for error messages
  dialog:loaddialog("data/menu/dialog_controls.xml"      )  -- The dialog to edit a player's controls
  dialog:loaddialog("data/menu/dialog_texture.xml"       )  -- The dialog to edit a player's custom texture
  dialog:loaddialog("data/menu/dialog_select_pattern.xml")  -- The dialog to select a pattern for a player's custom texture
  dialog:loaddialog("data/menu/dialog_color.xml"         )  -- The "Select Color" dialog
  
  dialog:createui();
  audio:startsoundtrack(0)
  
  g_Settings = system:getsettings()
  
  local l_Profiles = system:getsetting("profiles")
  
  if l_Profiles ~= "" then
    local s = "g_Players = " .. l_Profiles
    system:executeluastring(s)
  end
  
  fillItems()
  updateTexture()
  
  for i = 1, #g_Players do
    if #g_Players[i]["texture"] < 12 then
      g_Items[i]["label_texture"]:settext("Default")
    elseif string.sub(g_Players[i]["texture"], 1, 11) == "generate://" then
      g_Items[i]["label_texture"]:settext("Generated")
    elseif string.sub(g_Players[i]["texture"], 1, 11) == "imported://" then
      g_Items[i]["label_texture"]:settext("Imported")
    end
  end
end

function cleanup()
  g_Smgr:clear()
  dialog:clear()  
end

function fillResolution(a_Start)
end

function fillPatterns()
  for i = 1, 9 do
    if i + g_PatternStart <= #g_Patterns["patterns"] then
      io.write(tostring(i) .. ": " .. tostring(g_Patterns["patterns"][i + g_PatternStart]) .. "\n")
      g_Texture["patterns"][i]:setvisible(true)
      g_Texture["patterns"][i]:settext(g_Patterns["patterns"][i + g_PatternStart])
      g_Texture["patterns"][i]:setimage("file://data/patterns/" .. g_Patterns["patterns"][i + g_PatternStart])
    else
      g_Texture["patterns"][i]:setvisible(false)
    end
  end
end

function patternSwitchRight()
  if g_PatternStart < #g_Patterns["patterns"] - 6 then
    g_PatternStart = g_PatternStart + 3
    fillPatterns()
  end
  io.write("patternSwitchRight: " .. tostring(g_PatternStart) .. " (" .. tostring(#g_Patterns["patterns"]) .. ")\n")
end

function patternSwitchLeft()
  g_PatternStart = g_PatternStart - 3
  if g_PatternStart < 0 then
    g_PatternStart = 0
  end
  fillPatterns()
  io.write("patternSwitchLeft: " .. tostring(g_PatternStart) .. " (" .. tostring(#g_Patterns["patterns"]) .. ")\n")
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
    if g_Control:isvisible() then
      uibuttonclicked(-1, "btn_ctrl_ok")
      return
    end
    
    if g_Texture["dialog"]:isvisible() then
      if g_ColorEditor ~= nil then
        uibuttonclicked(-1, "btn_color_ok")
        return
      else
        uibuttonclicked(-1, "btn_texture_ok")
        return
      end
    end
    
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
    if g_Texture["dialog"]:isvisible() then
      if g_ColorEditor ~= nil then
        uibuttonclicked(-1, "btn_color_cancel")
        return
      else
        uibuttonclicked(-1, "btn_texture_cancel")
        return
      end
    end
    
    if g_Control:isvisible() then
      uibuttonclicked(-1, "btn_ctrl_cancel")
    elseif g_ConfirmDialog ~= nil and g_ConfirmDialog:isvisible() then
      if confirmDialog_handleButton(-1, "btn_ohisee") then
        return
      end
    else
      system:statechange(1)
    end
  elseif a_Name == "btn_select_pattern" then
    g_Texture["pattern_dialog"]:setvisible(true)
  elseif a_Name == "btn_pattern_close" then
    g_Texture["pattern_dialog"]:setvisible(false)
  elseif a_Name == "btn_pattern_right" then
    patternSwitchRight()
  elseif a_Name == "btn_pattern_left" then
    patternSwitchLeft()
  elseif a_Name == "texture_pattern_image" then
    g_Texture["pattern"]:settext(g_Texture["patterns"][a_Id]:gettext())
    g_Texture["pattern_dialog"]:setvisible(false)
    updateTexture()
  elseif a_Name == "pick_color" then
    local l_Color = string.format("%x", a_Id)
    
    while #l_Color < 6 do
      l_Color = "0" .. l_Color
    end
    
    local r = tonumber(string.sub(l_Color, 1, 2), 16)
    local g = tonumber(string.sub(l_Color, 3, 4), 16)
    local b = tonumber(string.sub(l_Color, 5, 6), 16)
    
    g_Color["red"  ]:settext(tostring(r))
    g_Color["green"]:settext(tostring(g))
    g_Color["blue" ]:settext(tostring(b))
    
    g_Color["scroll_red"  ]:setvalue(r)
    g_Color["scroll_green"]:setvalue(g)
    g_Color["scroll_blue" ]:setvalue(b)
  elseif g_TextureColors[a_Name] ~= nil then
    uibuttonclicked(tonumber(g_TextureColors[a_Name]:gettext(), 16), "pick_color")
    g_ColorEditor = g_TextureColors[a_Name]
    g_Color["dialog"]:setvisible(true)
  elseif a_Name == "btn_color_cancel" then
    g_Color["dialog"]:setvisible(false)
    g_ColorEditor = nil
  elseif a_Name == "btn_color_ok" then
    if g_ColorEditor ~= nil then
      local l_UpdateNf = false
      if g_ColorEditor == g_TextureColors["btn_select_bg_nb"] then
        l_UpdateNf = g_TextureColors["btn_select_nf"]:gettext() == g_TextureColors["btn_select_bg_nb"]:gettext()
      end
      
      local l_Color = string.format("%02x%02x%02x", tonumber(g_Color["red"]:gettext()), tonumber(g_Color["green"]:gettext()), tonumber(g_Color["blue"]:gettext()))
      g_ColorEditor:settext(l_Color)
      
      if l_UpdateNf then
        g_TextureColors["btn_select_nf"]:settext(l_Color)
      end
    end
    g_Color["dialog"]:setvisible(false)
    g_ColorEditor = nil
    updateTexture()
  elseif a_Name == "btn_texture_cancel" then
    g_Texture["dialog"]:setvisible(false)
  elseif a_Name == "btn_texture_ok" then
    io.write("Mode: " .. g_Texture["texture_mode"]:gettext() .. "\n")
    
    if g_Texture["texture_mode"]:gettext() == "Generated" or g_Texture["texture_mode"]:gettext() == "Imported" then
      g_Players[g_TexturePlr]["texture"] = getTextureString()
    else
      g_Players[g_TexturePlr]["texture"] = ""
    end
    
    g_Items[g_TexturePlr]["label_texture"]:settext(g_Texture["texture_mode"]:gettext())
    g_Texture["dialog"]:setvisible(false)
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
      elseif g_Buttons[a_Id]["action"] == "controls" then
        g_CtrlPlr = g_Buttons[a_Id]["index"]
        
        if g_CtrlPlr > 0 and g_CtrlPlr <= #g_Players then
          io.write("Modify controls of player \"" .. tostring(g_Players[g_CtrlPlr]["name"]) .. "\" (#" .. tostring(g_CtrlPlr) .. ")\n")
          g_Control:setvisible(true)
          system:setzlayer(23)
          
          g_CtrlGui:setproperty("headline", "Game controls player \"" .. g_Players[g_CtrlPlr]["name"] .. "\"")
          
          if g_Players[g_CtrlPlr]["controls"] ~= "" then
            g_CtrlGui:settext(g_Players[g_CtrlPlr]["controls"])
          else
            g_CtrlGui:settext(system:getcontrollerdata_game())
          end
        end
      elseif g_Buttons[a_Id]["action"] == "texture" then
        g_TexturePlr = g_Buttons[a_Id]["index"]
        if g_TexturePlr >= 1 and g_TexturePlr <= #g_Players then
          if #g_Players[g_TexturePlr]["texture"] < 12 then
            g_Texture["texture_mode"]:setselected(0)
            uivaluechanged(0, "texture_mode", 0)
          elseif string.sub(g_Players[g_TexturePlr]["texture"], 1, 11) == "generate://" then
            g_Texture["texture_mode"]:setselected(1)
            
            local l_Params = split(string.sub(g_Players[g_TexturePlr]["texture"], 12), "&")
            
            for i = 1, #l_Params do
              local l_Param = split(l_Params[i], "=")
              
              if l_Param[1] == "pattern" then
                g_Texture["pattern"]:settext(l_Param[2])
              elseif l_Param[1] == "numbercolor" then
                g_Texture["nb_foreground"]:settext(l_Param[2])
              elseif l_Param[1] == "numberback" then
                g_Texture["nb_background"]:settext(l_Param[2])
              elseif l_Param[1] == "numberborder" then
                g_Texture["number_frame"]:settext(l_Param[2])
              elseif l_Param[1] == "ringcolor" then
                g_Texture["number_ring"]:settext(l_Param[2])
              elseif l_Param[1] == "patterncolor" then
                g_Texture["pt_foreground"]:settext(l_Param[2])
              elseif l_Param[1] == "patternback" then
                g_Texture["pt_background"]:settext(l_Param[2])
              elseif l_Param[1] == "color" then
                g_Texture["imported_name"]:settext(l_Param[2])
              elseif l_Param[1] == "background" then
                g_Texture["imported_back"]:settext(l_Param[2])
              end
            end
            
            uivaluechanged(0, "texture_mode", 1)
          elseif string.sub(g_Players[g_TexturePlr]["texture"], 1, 11) == "imported://" then
            g_Texture["texture_mode"]:setselected(2)
            uivaluechanged(0, "texture_mode", 2)
          end
          
          updateTexture()
          g_Texture["dialog"]:setvisible(true)
        end
      else
        io.write("Button with action \"" .. g_Buttons[a_Id]["action"] .. "\" for player #" .. tostring(g_Buttons[a_Id]["index"]) .. " clicked.\n")
      end
    elseif a_Name == "btn_ctrl_ok" then
      io.write("Saving controls for player #" .. tostring(g_CtrlPlr) .. " (" .. g_Players[g_CtrlPlr]["name"] .. ")\n")
      if g_CtrlPlr > 0 and g_CtrlPlr <= #g_Players then
        g_Players[g_CtrlPlr]["controls"] = g_CtrlGui:gettext()
      end
      
      if g_Items[g_CtrlPlr] ~= nil and g_Items[g_CtrlPlr]["label_controls"] ~= nil then
        g_Items[g_CtrlPlr]["label_controls"]:settext(system:getfirstcontroller(g_CtrlGui:gettext()))
      else
        io.write("**** Player UI not found\n")
      end
      
      g_Control:setvisible(false)
      system:setzlayer(0)
    elseif a_Name == "btn_ctrl_cancel" then
      g_Control:setvisible(false)
      system:setzlayer(0)
    else
      io.write("Button clicked: \"" .. a_Name .. "\" (" .. tostring(a_Id) .. ")\n")
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
  else
    io.write("Text changed: \"" .. tostring(a_Name) .. "\" = \"" .. tostring(a_NewValue) .. "\"\n")
  end
end

function getTextureString()
  if g_Texture["texture_mode"]:gettext() == "Generated" then
    return "generate://pattern=" .. tostring(g_Texture["pattern"]:gettext()) .. 
           "&numbercolor="  .. tostring(g_Texture["nb_foreground"]:gettext()) ..
           "&numberback="   .. tostring(g_Texture["nb_background"]:gettext()) ..
           "&numberborder=" .. tostring(g_Texture["number_frame" ]:gettext()) ..
           "&ringcolor="    .. tostring(g_Texture["number_ring"  ]:gettext()) ..
           "&patterncolor=" .. tostring(g_Texture["pt_foreground"]:gettext()) ..
           "&patternback="  .. tostring(g_Texture["pt_background"]:gettext()) ..
           "&number="       .. "1"
  elseif g_Texture["texture_mode"]:gettext() == "Imported" then
    return "imported://file=data/textures/texture_marblemann.png&color=" .. tostring(g_Texture["imported_name"]) .. "&background=" .. tostring(g_Texture["imported_back"])
  else
    return "generate://pattern=texture_marbles2.png&numbercolor=000000&numberback=4b64f9&numberborder=4b64f9&ringcolor=3548b7&patterncolor=000000&patternback=4b64f9&number=1"
  end
end

function updateTexture()
  local l_Generate = getTextureString()
  
  io.write("Generate: " .. l_Generate .. "\n")
  g_Texture["texture_image"]:setimage(l_Generate) 
  
  if g_OldTexture ~= nil then
    system:removetexture(g_OldTexture)
  end
  g_OldTexture = l_Generate
end

function uivaluechanged(a_Id, a_Name, a_Value)
  if a_Name == "texture_mode" then
    if a_Value == 0 then
      g_Texture["tab_generate"]:setvisible(false)
      g_Texture["tab_imported"]:setvisible(false)
      
      updateTexture()
    elseif a_Value == 1 then
      g_Texture["tab_generate"]:setvisible(true )
      g_Texture["tab_imported"]:setvisible(false)
      
      updateTexture()
    elseif a_Value == 2 then
      g_Texture["tab_generate"]:setvisible(false)
      g_Texture["tab_imported"]:setvisible(true )
      
      updateTexture()
    end
  elseif a_Name == "texture_pattern" then
    updateTexture()
  elseif a_Name == "scrollbar_red" or a_Name == "scrollbar_green" or a_Name == "scrollbar_blue" then
    if a_Name == "scrollbar_red" then
      g_Color["red"]:settext(string.format("%.0f", a_Value))
    elseif a_Name == "scrollbar_green" then
      g_Color["green"]:settext(string.format("%.0f", a_Value))
    elseif a_Name == "scrollbar_blue" then
      g_Color["blue"]:settext(string.format("%.0f", a_Value))
    end
    
    local l_Color = "255, " .. g_Color["red"  ]:gettext() .. ", " .. g_Color["green"]:gettext() .. ", " .. g_Color["blue" ]:gettext()
    
    g_Color["show"]:setproperty("BackColor", l_Color)
  else
    io.write("Value Changed: " .. tostring(a_Name) .. " (" .. tostring(a_Id) .. ") = " .. tostring(a_Value) .. "\n")
  end
end

function windowresized()
  fillItems()
end
