-- This AI script is necessary to avoid most
-- respawns when the new AI is playing agains the old one

function decide_roadsplit(a_MarbleId, a_Split)
  if a_Split == 10 then
    return 0
  else
    return 1
  end
end