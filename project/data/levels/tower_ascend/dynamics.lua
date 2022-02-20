-- (w) 2021 by Dustbin::Games / Christian Keimel / This file is licensed under the terms of the zlib license: https://opensource.org/licenses/Zlib

--[[function initialize(a_World)
end--]]

-- Callback for a trigger hit by a marble
-- @param a_Marble the marble that triggered
-- @param a_Trigger the trigger id
function onTrigger(a_Marble, a_Trigger)
  physics:setmarbleupvector(a_Marble, { x = 0.0, y = 1.0, z = 0.0 })
end

-- Callback if a marble respawns. Only called once when the respawn starts
-- @param a_Marble the respawning marble
--[[function onRespawn(a_Marble)
end--]]

-- Callback for every simulation step (120 per second)
-- @param a_Step the step of the simulation
--[[function onStep(a_Step)
   io.write("Step: " .. tostring(a_Step) .. "\n")
 end--]]

-- Callback for a marble passing a checkpoint
-- @param a_Marble the marble that passed the checkpoint
-- @param a_Checkpoint the checkpoint passed
--[[function onCheckpoint(a_Marble, a_Checkpoint)
  io.write("Marble " .. tostring(a_Marble) .. " has passed checkpoint " .. tostring(a_Checkpoint) .. "\n")
end--]]

-- Callback if the stunned state of a marble changed
-- @param a_Marble the marble
-- @param a_State the stunned state of the marble (1 == stunned, 0 == back to normal)
--[[function onMarbleStunned(a_Marble, a_State)
  io.write("Marble " .. tostring(a_Marble) .. " stunned (" .. tostring(a_State) .. ")\n")
end--]]

-- Callback for marble movement. This is called every step for every marble,
-- so make sure it's not getting too complicated
-- @param a_Marble the moving marble
-- @param a_Position the new position
--[[function onMarbleMoved(a_Marble, a_Position)
  io.write("Marble Moved: " .. tostring(a_Marble) .. " (" .. tostring(a_Position["x"]) .. ", ".. tostring(a_Position["y"]) .. ", ".. tostring(a_Position["z"]) .. ")\n")
end--]]
