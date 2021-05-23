system:executeluascript("data/lua/helpers_main.lua")

g_Time = 0  -- The time of the last "step" message

-- UI Items
g_ResolutionList = { }
g_Settings = {
  resolution   = 9,
  fullscreen   = false,
  shadows      = 2,
  ambientlight = 3
}

g_Items = {
}

g_SpinBoxes = {
  shadow = {
    key   = "shadows",
    plus  = "button_shadows_plus",
    minus = "button_shadows_minus",
    label = {
      name = "label_shadows",
      node = nil
    },
    options = {
      [1] = "off",
      [2] = "low",
      [3] = "medium",
      [4] = "high",
      [5] = "very high"
    }
  },
  ambient = {
    key   = "ambientlight",
    plus  = "button_ambient_plus",
    minus = "button_ambient_minus",
    label = {
      name = "label_ambient",
      node = nil
    },
    options = {
      [1] = "dark",
      [2] = "medium dark",
      [3] = "medium",
      [4] = "medium light",
      [5] = "light"
    }
  }
}

g_Sliders = {
  SfxSlider = {
    item = nil,
    label = nil,
    value = 75,
    key = "LabelSfx"
  },
  MusicSlider = {
    item = nil,
    label = nil,
    value = 25,
    key = "LabelSTrack"
  }
}

g_ResolutionStart = 1

g_FinishTime = -1
g_StartTime  = -1

function initialize()
  io.write("Setup script started.\n")
  
  g_Smgr = system:getscenemanager()
  g_Smgr:loadscene("data/menu3d/menu_settings.xml")
  
  g_Camera = g_Smgr:addcamera()

  g_Camera:setposition({ x = 0.0, y = 0.0, z = 30.0 })
  g_Camera:setupvector({ x = 0.0, y = 1.0, z =  0.0 })
  g_Camera:settarget  ({ x = 0.0, y = 0.0, z = 70.0 })
  g_Camera:activate()
  
  g_Root = g_Smgr:getscenenodefromname("root_menusettings")
  
  local l_List = system:getresolutionlist()
  
  for i = 0, l_List:count() - 1 do
    local s = l_List:get(i)
    table.insert(g_ResolutionList, s)
  end
  
  g_ResolutionWindow = g_Smgr:getscenenodefromname("resolution")
  
  io.write("==> " .. g_ResolutionList[1] .. "\n")
  
  g_Items["resolution"] = { }
  
  for i = 1,6 do
    local l_Item = { }
    local l_Node = g_Smgr:getscenenodefromname("res_" .. tostring(i))
    l_Item["node"] = l_Node
    
    local l_Button = g_Smgr:getguiitemfromname("button_res_" .. tostring(i))
    l_Item["button"] = l_Button
    
    table.insert(g_Items["resolution"], l_Item)
  end
  
  g_Items["label_resolution"   ] = g_Smgr:getguiitemfromname  ("label_resolution")
  g_Items["checkbox_fullscreen"] = g_Smgr:getscenenodefromname("checkbox_fullscreen")
  
  if g_Settings["resolution"] >= 1 and g_Settings["resolution"] <= #g_ResolutionList then
    g_Items["label_resolution"]:settext(g_ResolutionList[g_Settings["resolution"]])
  end
  
  if g_Settings["fullscreen"] then
    g_Items["checkbox_fullscreen"]:setrotation({ x = 0, y = 0, z = 0 })
  else
    g_Items["checkbox_fullscreen"]:setrotation({ x = 0, y = 180, z = 0 })
  end
  
  for k,v in pairs(g_SpinBoxes) do
    if v["label"] ~= nil then
      v["label"]["node"] = g_Smgr:getguiitemfromname(v["label"]["name"])
      if v["label"]["node"] ~= nil and v["options"][g_Settings[v["key"]]] ~= nil then
        v["label"]["node"]:settext(v["options"][g_Settings[v["key"]]])
      end
    end
  end
  
  for k,v in pairs(g_Sliders) do
    v["label"] = g_Smgr:getguiitemfromname(v["key"])
    v["item" ] = g_Smgr:getguiitemfromname(k .. "Item")
    
    if v["item"] ~= nil then
      v["item"]:setvalue(v["value"])
      if v["label"] ~= nil then
        v["label"]:settext(string.format("%.1f %%", v["value"]))
      end
    end
  end
  
  startFadeIn(g_Root)
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

function fillResolution(a_Start)
  for i = 1,6 do
    if i + a_Start <= #g_ResolutionList then
      g_Items["resolution"][i]["button"]:settext(g_ResolutionList[i + a_Start])
      if i + a_Start == g_Settings["resolution"] then
        g_Items["resolution"][i]["button"]:setbackgroundcolor({ a = 255, r = 192, g = 255, b = 192 })
      else
        g_Items["resolution"][i]["button"]:setbackgroundcolor({ a = 255, r = 255, g = 255, b = 255 })
      end
    end
  end
end

function showResolutionSelector()
  startRotation(g_Root, g_Time, 0, 90)
  
  g_ResolutionStart = 6 * math.floor(g_Settings["resolution"] / 6)
  
  if g_ResolutionStart < 0 then
    g_ResolutionStart = 0
  end
  
  if g_ResolutionStart > #g_ResolutionList - 6 then
    g_ResolutionStart = #g_ResolutionList - 6
  end
  fillResolution(g_ResolutionStart)
end

function uielementhovered(a_Id, a_Name)
end

function uielementleft(a_Id, a_Name)
end

function uibuttonclicked(a_Id, a_Name)
  if a_Name == "button_cancel" then
    g_FinishTime = g_Time
    startFadeOut(g_Root, g_Time, 1)
  elseif a_Name == "button_sound" then
    startRotation(g_Root, g_Time, 0, -90)
  elseif a_Name == "button_gfx" then
    startRotation(g_Root, g_Time, -90, 0)
  elseif a_Name == "button_res_select" then
    showResolutionSelector()
  elseif a_Name == "button_res_cancel" then
    startRotation(g_Root, g_Time, 90, 0)
  elseif a_Name == "button_res_up" then
    g_ResolutionStart = g_ResolutionStart - 6
    if g_ResolutionStart < 0 then
      g_ResolutionStart = 0
    end
    fillResolution(g_ResolutionStart)
  elseif a_Name == "button_res_down" then
    g_ResolutionStart = g_ResolutionStart + 6
    if g_ResolutionStart > #g_ResolutionList - 6 then
      g_ResolutionStart = #g_ResolutionList - 6
    end
    fillResolution(g_ResolutionStart)
  elseif a_Name == "checkbox_fullscreen" then
    g_Settings["fullscreen"] = not g_Settings["fullscreen"]
    if g_Settings["fullscreen"] then
      g_Items["checkbox_fullscreen"]:setrotation({ x = 0, y = 0, z = 0 })
    else
      g_Items["checkbox_fullscreen"]:setrotation({ x = 0, y = 180, z = 0 })
    end
  else
    for k,v in pairs(g_SpinBoxes) do
      local l_Value = g_Settings[v["key"]]
      io.write("Value: " .. tostring(l_Value) .. "\n")
      
      if a_Name == v["plus"] then
        if l_Value < #v["options"] then
          l_Value = l_Value + 1
        end
      elseif a_Name == v["minus"] then
        if l_Value > 1 then
          l_Value = l_Value - 1
        end
      end
      
      if l_Value ~= g_Settings[v["key"]] then
        g_Settings[v["key"]] = l_Value
        if v["label"]["node"] ~= nil then
          v["label"]["node"]:settext(v["options"][l_Value])
        end
        
        return
      end
    end
    
    for i = 1,6 do
      if a_Name == "res_" .. tostring(i) then
        g_Settings["resolution"] = g_ResolutionStart + i
        g_Items["label_resolution"]:settext(g_ResolutionList[g_Settings["resolution"]])
        
        startRotation(g_Root, g_Time, 90, 0)
        
        return
      end
    end
    
    io.write("Button Clicked: \"" .. a_Name .. "\"\n")
  end
end

function uivaluechanged(a_Id, a_Name, a_Value)
  if g_Sliders[a_Name] ~= nil then
    g_Sliders[a_Name]["value"] = tonumber(a_Value)
    if g_Sliders[a_Name]["label"] ~= nil then
      g_Sliders[a_Name]["label"]:settext(string.format("%.1f %%", tonumber(a_Value)))
    end
  end
end