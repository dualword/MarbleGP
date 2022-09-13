
function getBlockStep(a_stepno, a_factor)
  return math.fmod(a_stepno, 480 * a_factor)
end

function getCrushStep(a_stepno, a_factor)
  return math.fmod(a_stepno, 360 * a_factor)
end

function getGateStep(a_stepno)
  return math.fmod(a_stepno, 600)
end
