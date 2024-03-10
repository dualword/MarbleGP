import json
import os

g_File = open(os.getenv("APPDATA") + "\\DustbinGames\\MarbleGP_debug\\tournament.json")

g_Data    = json.load(g_File)
g_Players = g_Data["players"]

for l_Race in g_Data["races"]:
  g_Results = { }
  print(l_Race["track"])
  
  for l_Result in l_Race["raceresult"]:
    l_Name = ""
    
    
    for l_Player in g_Players:
      if l_Result["playerid"] == l_Player["playerid"]:
        l_Name = l_Player["name"]
        
    if "|" in l_Name:
      l_Name = l_Name[l_Name.index("|") + 1:]
      l_Idx  = 0
      
      while l_Idx < len(l_Result["laps"]) - 1:
        l_Data = l_Name
        l_Cp = 0
        
        l_Checkpoints = [ ]
        
        while l_Cp < len(l_Result["laps"][l_Idx]):
          l_Checkpoints.append(l_Result["laps"][l_Idx][l_Cp])
          l_Cp += 1
        
        l_Checkpoints.append(l_Result["laps"][l_Idx + 1][0])
        
        if l_Name not in g_Results:
          g_Results[l_Name] = l_Checkpoints
        else:
          if g_Results[l_Name][len(g_Results[l_Name]) - 1] - g_Results[l_Name][0] > l_Checkpoints[len(l_Checkpoints) - 1] - l_Checkpoints[0]:
            g_Results[l_Name] = l_Checkpoints
        
        l_Idx += 1
        


  for l_Key in g_Results:
    l_Start = g_Results[l_Key][0]
    l_Cp = 0
    while l_Cp < len(g_Results[l_Key]):
      g_Results[l_Key][l_Cp] -= l_Start
      l_Cp += 1
  
  print(g_Results)
  
  l_File = open("../../data/levels/" + l_Race["track"] + "/laptimes_ai.txt", "w")
  l_File.write(l_Race["track"])
  
  for l_Key in g_Results:
    l_File.write("\n" + l_Key)
    
    l_Cp = 0
    while l_Cp < len(g_Results[l_Key]):
      l_File.write(";" + str(g_Results[l_Key][l_Cp]))
      l_Cp += 1
      
