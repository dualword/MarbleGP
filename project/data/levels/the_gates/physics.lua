-- trigger 0 / 2
-- trigger 1 / 3

g_Trigger = {
  [1] = { },
  [2] = { }
}

g_Counter = {
  [1] = 0,
  [2] = 0
}

function ontrigger(a_Marble, a_Trigger)
  if a_Trigger == 0 then
    -- check if the marble is currently in the first trigger
    if g_Trigger[1][a_Marble] == nil then
      -- if not add it and increase the counter
      g_Trigger[1][a_Marble] = true
      g_Counter[1] = g_Counter[1] + 1
      physics:startmotor(23095, 50, 500)
    end
  elseif a_Trigger == 1 then
    -- check if the marble is currently in the second trigger
    if g_Trigger[2][a_Marble] == nil then
      -- if not add it and increase the counter
      g_Trigger[2][a_Marble] = true
      g_Counter[2] = g_Counter[2] + 1
      physics:startmotor(23096, 50, 500)
    end
  elseif a_Trigger == 2 then
    -- check if the marble is currently in the first trigger
    if g_Trigger[1][a_Marble] ~= nil then
      -- if so remove it and decrease the counter
      g_Trigger[1][a_Marble] = nil
      g_Counter[1] = g_Counter[1] - 1
      
      if g_Counter[1] <= 0 then
        physics:startmotor(23095, -50, 500)
        g_Counter[1] = 0
      end
    end
  elseif a_Trigger == 3 then
    -- check if the marble is currently in the second trigger
    if g_Trigger[2][a_Marble] ~= nil then
      -- if so remove it and decrease the counter
      g_Trigger[2][a_Marble] = nil
      g_Counter[2] = g_Counter[2] - 1
      
      if g_Counter[2] <= 0 then
        physics:startmotor(23096, -50, 500)
        g_Counter[2] = 0
      end
    end
  end
end

function onplayerwithdrawn(a_Marble)
  ontrigger(a_Marble, 2)
  ontrigger(a_Marble, 3)
end

function onplayerrespawn(a_Marble)
  ontrigger(a_Marble, 2)
  ontrigger(a_Marble, 3)
end