
function ontrigger(a_Marble, a_Trigger)
  if a_Trigger == 46 then
    physics:setmarblecameraup(a_Marble, { x = 0.0, y = 1.0, z = 0.0 })
  end
end
