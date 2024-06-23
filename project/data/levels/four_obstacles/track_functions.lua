g_BlockTime  = 240
g_HammerTime = 360
g_KickerTime = 360
g_GateTime   = 240

function getBlockState(a_stepNo)
  return (a_stepNo // g_BlockTime) % 12
end

function getBlockTime(a_stepNo)
  return a_stepNo % g_BlockTime
end

function getHammerState(a_stepNo)
  return (a_stepNo // g_HammerTime) % 2
end

function getHammerTime(a_stepNo)
  return a_stepNo % g_HammerTime
end

function getKickerState(a_stepNo)
  return (a_stepNo // g_KickerTime) % 4
end

function getKickerTime(a_stepNo)
  return a_stepNo % g_KickerTime
end

function getGateState(a_stepNo)
  return (a_stepNo // g_GateTime) % 4
end

function getGateTime(a_stepNo)
  return a_stepNo % g_GateTime
end