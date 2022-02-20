import os, os.path, shutil

l_Path = "../../data/levels"
l_Content = os.listdir(l_Path)

l_File = open("../../data/levels/tracks.dat", "w")
for l_Entry in l_Content:
  if os.path.exists(l_Path + "/" + l_Entry + "/track.xml"):
    l_File.write(l_Entry + "\n");
    print(l_Entry)