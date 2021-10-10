-- (w) 2021 by Dustbin::Games / Christian Keimel / This file is licensed under the terms of the zlib license: https://opensource.org/licenses/Zlib
system:executeluascript("data/lua/serializer.lua")

g_Gates   = { }
g_Trigger = {
  [1] = { },
  [2] = { }
}
g_Count = {
  [1] = 0,
  [2] = 0
}

function initialize(a_World)
  for k,v in pairs(a_World) do
    io.write("==> " .. tostring(v:getname()) .. "\n")
    if v:getname() == "PhyGate1" or v:getname() == "PhyGate2" then
      table.insert(g_Gates, v)
    end
  end
end

-- Callback for a trigger hit by a marble
-- @param a_Marble the marble that triggered
-- @param a_Trigger the trigger id
function onTrigger(a_Marble, a_Trigger)
  if a_Trigger == 0 or a_Trigger == 1 then
    if g_Trigger[a_Trigger + 1][a_Marble] == nil then
      g_Count[a_Trigger + 1] = g_Count[a_Trigger + 1] + 1
      g_Trigger[a_Trigger + 1][a_Marble] = true
      g_Gates[a_Trigger + 1]:startmotor(50, 500)
    end
  elseif a_Trigger == 2 or a_Trigger == 3 then
    if g_Trigger[a_Trigger - 1][a_Marble] ~= nil then
      g_Trigger[a_Trigger - 1][a_Marble] = nil
      g_Count[a_Trigger - 1] = g_Count[a_Trigger - 1] - 1
      if g_Count[a_Trigger - 1] == 0 then
        g_Gates[a_Trigger - 1]:startmotor(-50, 500)
      end
    end
  end
end

-- Callback if a marble respawns. Only called once when the respawn starts
-- @param a_Marble the respawning marble
function onRespawn(a_Marble)
  onTrigger(a_Marble, 2)
  onTrigger(a_Marble, 3)
end

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
