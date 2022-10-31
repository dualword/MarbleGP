system:executeluascript("levels/the_gates/track_functions.lua")

function onstep(a_Step)
  g_Step = a_Step
  for i = 1, 2 do
    if g_Warnings[i]["currenty"] < g_Warnings[i]["targety"] then
      g_Warnings[i]["currenty"] = g_Warnings[i]["currenty"] + 5
      
      if g_Warnings[i]["currenty"] > g_Warnings[i]["targety"] then
        g_Warnings[i]["currenty"] = g_Warnings[i]["targety"]
      end
      
      scene:setrotation(g_Warnings[i]["sign"      ], { x = 0.0, y = g_Warnings[i]["currenty"], z = 0.0 })
      scene:setrotation(g_Warnings[i]["sign_small"], { x = 0.0, y = g_Warnings[i]["currenty"], z = 0.0 })
    elseif g_Warnings[i]["currenty"] > g_Warnings[i]["targety"] then
      g_Warnings[i]["currenty"] = g_Warnings[i]["currenty"] - 5
      
      if g_Warnings[i]["currenty"] < g_Warnings[i]["targety"] then
        g_Warnings[i]["currenty"] = g_Warnings[i]["targety"]
      end
      
      scene:setrotation(g_Warnings[i]["sign"      ], { x = 0.0, y = g_Warnings[i]["currenty"], z = 0.0 })
      scene:setrotation(g_Warnings[i]["sign_small"], { x = 0.0, y = g_Warnings[i]["currenty"], z = 0.0 })
    end
  end
end
