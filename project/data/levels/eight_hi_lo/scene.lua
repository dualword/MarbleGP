
g_FirstIDs = {
  [1] = 23287,
  [2] = 23290,
  [3] = 23291,
  [4] = 23292,
  [5] = 23293,
  [6] = 23294
}

g_SecondIDs = {
  [ 1] = 23413,
  [ 2] = 23418,
  [ 3] = 23421,
  [ 4] = 23424,
  [ 5] = 23430,
  [ 6] = 23427,
  [ 7] = 23433,
  [ 8] = 23436,
  [ 9] = 23439,
  [10] = 23442,
  [11] = 23447,
  [12] = 23452,
  [13] = 23457,
  [14] = 23462,
  [15] = 23467,
  [16] = 23470,
  [17] = 23473,
  [18] = 23476,
  [19] = 23479,
  [20] = 23482,
  [21] = 23485,
  [22] = 23488,
  [23] = 23491,
  [24] = 23494
}

g_CurrentFirst = 0
g_CurrentSecnd = 0

function initialize()
  for k,v in pairs(g_FirstIDs) do
    scene:setvisible(v, false)
  end
  
  for k,v in pairs(g_SecondIDs) do
    scene:setvisible(v, false)
  end
end

function onstep(a_StepNo)
  if math.fmod(a_StepNo, 30) == 0 then
    if g_CurrentFirst ~= 0 then
      scene:setvisible(g_FirstIDs[g_CurrentFirst], false)
    end
  
    g_CurrentFirst = g_CurrentFirst + 1
    if g_CurrentFirst > 6 then
      g_CurrentFirst = 1
    end
    
    scene:setvisible(g_FirstIDs[g_CurrentFirst], true)
  end
  
  if math.fmod(a_StepNo, 15) == 0 then
    if g_CurrentSecnd ~= 0 then
      scene:setvisible(g_SecondIDs[g_CurrentSecnd], false)
    end
    
    g_CurrentSecnd = g_CurrentSecnd + 1
    if g_CurrentSecnd > 24 then
      g_CurrentSecnd = 1
    end
    
    scene:setvisible(g_SecondIDs[g_CurrentSecnd], true)
  end
end
