
function decide_blocker(a_MarbleId, a_Blocker)
  local l_Rotation = ai:getobjectrotation(23840)
  return not(l_Rotation["z"] <= 135.0 and l_Rotation["z"] >= 87.0)
end
