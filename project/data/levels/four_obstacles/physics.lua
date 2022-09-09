
io.write("\n\n**** Script started\n\n")

g_Actions = {
  [1] = { step = 0, index = 0 },
  [2] = { step = 0, index = 0 },  -- 23124, 23127, 23129
  [3] = { step = 0, index = 0 }   -- 23133, 23136, 23138
}

function onstep(a_stepno)
  if a_stepno - g_Actions[1]["step"] > 360 then
    g_Actions[1]["step"] = a_stepno
    
    if g_Actions[1]["index"] == 0 then
      physics:startmotor(23092, 25, 150)
      physics:startmotor(23121, 25, 150)
      
      g_Actions[1]["index"] = 1
    else
      physics:startmotor(23092, -25, 150)
      physics:startmotor(23121, -25, 150)
      
      g_Actions[1]["index"] = 0
    end
  end
  
  if a_stepno - g_Actions[2]["step"] > 240 then
    g_Actions[2]["step"] = a_stepno
    
    if g_Actions[2]["index"] == 0 then
      physics:startmotor(23124, -50, 150)
      g_Actions[2]["index"] = 1
    elseif g_Actions[2]["index"] == 1 then
      physics:startmotor(23127, -50, 150)
      g_Actions[2]["index"] = 2
    elseif g_Actions[2]["index"] == 2 then
      physics:startmotor(23129, -50, 150)
      g_Actions[2]["index"] = 3
    elseif g_Actions[2]["index"] == 3 then
      physics:startmotor(23124, 50, 150)
      g_Actions[2]["index"] = 4
    elseif g_Actions[2]["index"] == 4 then
      physics:startmotor(23127, 50, 150)
      g_Actions[2]["index"] = 5
    elseif g_Actions[2]["index"] == 5 then
      physics:startmotor(23129, 50, 150)
      g_Actions[2]["index"] = 0
    end
  end
  
  if a_stepno - g_Actions[3]["step"] > 180 then
    g_Actions[3]["step"] = a_stepno
    
    io.write(tostring(g_Actions[3]["index"]))
    if g_Actions[3]["index"] == 0 then
      physics:startmotor(23133, -25, 750)
      physics:startmotor(23138,  25, 750)
      g_Actions[3]["index"] = 1
    elseif g_Actions[3]["index"] == 1 then
      physics:startmotor(23136, -25, 750)
      physics:startmotor(23133,  25, 750)
      g_Actions[3]["index"] = 2
    elseif g_Actions[3]["index"] == 2 then
      physics:startmotor(23138, -25, 750)
      physics:startmotor(23136,  25, 750)
      g_Actions[3]["index"] = 0
    end
  end
end