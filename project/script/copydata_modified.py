import sys, os, shutil, time, calendar, pickle, math

g_Modified = [ ]
g_DataFolder = "../../data"

def checkForModifications(a_Folder):
  print("Checking \"" + a_Folder + "\"...")
  
  l_Content = os.listdir(a_Folder)
  
  for l_Entry in l_Content:
    if l_Entry != "." and l_Entry != "..":
      l_File = a_Folder + "/" + l_Entry
      
      if os.path.isdir(l_File):
        checkForModifications(l_File)
      else:
        if l_File not in g_CopiedItems["files"]:
          print("New File: " + l_File)
          g_Modified.append(l_File)
        else:
          if math.floor(os.path.getmtime(l_File)) > g_CopiedItems["files"][l_File]:
            print("Modified: " + l_File + " (" + str(g_CopiedItems["files"][l_File]) + ", " + str(math.floor(os.path.getmtime(l_File))) + ")")
            g_Modified.append(l_File)

def copyModified():
  for l_Item in g_Modified:
    l_Target = g_CopiedItems["rootfolder"] + l_Item[len(g_DataFolder):]
    print(l_Item + " --> " + l_Target)
    shutil.copyfile(l_Item, l_Target)

  
if not os.path.exists("copydata_files.dict"):
  print("Copy information file does not exists, exiting.")
  sys.exit(0)
  
with open("copydata_files.dict", "rb") as l_File:
  g_CopiedItems = pickle.load(l_File)
  
checkForModifications(g_DataFolder)
print(g_Modified)
copyModified()