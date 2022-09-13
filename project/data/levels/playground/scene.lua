
function onstep(a_stepno)
  local l_Angle = -math.fmod(a_stepno - 30, 360)
  
  scene:setrotation(23423, { x = 0.0, y = 0.0, z = l_Angle })
end