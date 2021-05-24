g_Fadeout = nil
g_Fadein  = nil

-- Rotate the menu
g_Rotation = {
}

function startFadeIn(a_Object)
  g_Fadein = { }
  
  g_Fadein["start"   ] = -1
  g_Fadein["object"  ] = a_Object
  g_Fadein["position"] = a_Object:getposition()
  
  g_Fadein["object"]:setposition({ x = g_Fadein["position"]["x"], y = g_Fadein["position"]["x"] + 100, z = g_Fadein["position"]["z"] })
  end

function startFadeOut(a_Object, a_Time, a_NextState)
  g_Fadeout = { }
  g_Fadeout["start"   ] = a_Time
  g_Fadeout["object"  ] = a_Object
  g_Fadeout["position"] = a_Object:getposition()
  g_Fadeout["state"   ] = a_NextState
end

function startRotation(a_Object, a_Time, a_Start, a_Target)
  local l_Rotation = { }
  l_Rotation["start"    ] = a_Start
  l_Rotation["target"   ] = a_Target
  l_Rotation["starttime"] = a_Time
  l_Rotation["node"     ] = a_Object
  table.insert(g_Rotation, l_Rotation)
end

function processanimation()
  if g_Fadein ~= nil then
    if g_Fadein["start"] == -1 then
      g_Fadein["start"] = g_Time
    end
    
    if g_Time - g_Fadein["start"] < 500 then
      local l_Factor = 1.0 - (g_Time - g_Fadein["start"]) / 500
      l_Factor = l_Factor * l_Factor
      local l_y = 100 * l_Factor
      g_Fadein["object"]:setposition({ x = g_Fadein["position"]["x"], y = g_Fadein["position"]["y"] + l_y, z = g_Fadein["position"]["z"] })
    else
      g_Fadein["object"]:setposition(g_Fadein["position"])
      g_Fadein = nil
    end
  end
  
  if g_Fadeout ~= nil then
    if g_Time > g_Fadeout["start"] + 500 then
      system:statechange(g_Fadeout["state"])
    else
      l_Factor = (g_Time - g_Fadeout["start"]) / 500
      l_Factor = l_Factor * l_Factor
      g_Fadeout["object"]:setposition({ x = g_Fadeout["position"]["x"], y = g_Fadeout["position"]["y"] - 100 * l_Factor, z = g_Fadeout["position"]["z"] })
    end
  end
  
  for k,v in pairs(g_Rotation) do
    if v["starttime"] ~= -1 then
      local l_Diff = (g_Time - v["starttime"]) / 500.0
      
      if l_Diff >= 1 then
        v["node"]:setrotation({ x = 0, y = v["target"], z = 0 })
        g_Rotation[k] = nil
      else
        local l_New = v["target"] - (v["target"] - v["start"]) * math.cos(90.0 * l_Diff * math.pi / 180.0)
        v["node"]:setrotation({ x = 0, y = l_New, z = 0 })
      end
    end
  end
end

function initSpinBox(a_Data, a_Settings)
  for k,v in pairs(a_Data) do
    if v["label"] ~= nil then
      v["label"]["node"] = g_Smgr:getguiitemfromname(v["label"]["name"])
      if v["label"]["node"] ~= nil and v["options"][a_Settings[v["key"]]] ~= nil then
        v["label"]["node"]:settext(v["options"][a_Settings[v["key"]]])
      end
    end
  end
end

function processSpinBoxes(a_Name, a_Data, a_Settings)
  for k,v in pairs(a_Data) do
    local l_Value = a_Settings[v["key"]]
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
    
    if l_Value ~= a_Settings[v["key"]] then
      g_Settings[v["key"]] = l_Value
      if v["label"]["node"] ~= nil then
        v["label"]["node"]:settext(v["options"][l_Value])
      end
      
      return
    end
  end
end