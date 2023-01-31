
function decide_blocker(a_MarbleId, a_Blocker)
  local l_Rotation = ai:getobjectrotation(23840)
  local l_Vector = { }
  
  l_Vector["x"] = 1.0
  l_Vector["y"] = 0.0
  l_Vector["z"] = 0.0
  
  local l_Out = ai:rotatevector(l_Vector, l_Rotation)
  
  if a_Blocker == 3 then
    if l_Out["y"] < -0.7 and l_Out["x"] < 0.0 then
      return false
    else
      return true
    end
  elseif a_Blocker == 11 then
    if l_Out["x"] > 0.0 then
      if l_Out["y"] >= -0.85 and l_Out["y"] < -0.65 then
        return false
      else
        return true
      end
    end
  elseif a_Blocker == 12 then
    if l_Out["x"] < 0.0 then
      if l_Out["y"] < 0.65 and l_Out["y"] > 0.45 then
        return false
      else
        return true
      end
    end
  end
  
  return true
end
