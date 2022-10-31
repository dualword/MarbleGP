g_Step = 0

function onstep(a_stepno)
  g_Step = a_stepno
end

function decide_blocker(a_MarbleId, a_Blocker)
  local l_Move = math.fmod(g_Step, 360)
  return l_Move > 35 and l_Move < 315
end
