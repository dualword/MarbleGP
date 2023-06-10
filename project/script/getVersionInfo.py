import sys, os, shutil

g_File = open("../data/menu/version.xml", "r")
g_Lines = g_File.readlines()
g_File.close()

g_Output = [ ]

g_Tag = "<attribute key=\"Caption\" value=\""
g_End = "\" />"

for l_Line in g_Lines:
  if l_Line.find(g_Tag) != -1 and l_Line.find(g_End):
    l_Version = l_Line[l_Line.find(g_Tag) + len(g_Tag):]
    l_Version = l_Version[:l_Version.find(g_End)]
    
    print(l_Version)
    g_NewFile = open("../release/version.php", "w")
    g_NewFile.writelines(l_Version)
    g_NewFile.close()
