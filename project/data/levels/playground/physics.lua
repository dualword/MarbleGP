
g_LastStep   = 0
g_LastAction = 0

function onstep(a_stepno)
  if a_stepno - g_LastStep > 360 then
    g_LastStep = a_stepno
    
    if g_LastAction == 0 then
      physics:startmotor(23170, -40, 100)
      physics:startmotor(23328,  40, 100)
      g_LastAction = 1
    else
      physics:startmotor(23170,  40, 100)
      physics:startmotor(23328, -40, 100)
      g_LastAction = 0
    end
  end
end