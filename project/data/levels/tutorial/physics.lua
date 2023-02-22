
function ontrigger(a_Marble, a_Trigger)
  io:write("LUA: Trigger\n")
  physics:sendtrigger(a_Marble, a_Trigger)
end