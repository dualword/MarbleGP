import sys, os, shutil, time, calendar, pickle

g_CopyiedItems = {
  "rootfolder": "",
  "files": {
  }
}

def createIfNotExisting(a_Folder):
  if not os.path.exists(a_Folder):
    print("Creating folder \"" + a_Folder + "\" ...")
    os.makedirs(a_Folder)

def copyContent(a_Folder, a_Target):
  print(a_Target[-11:])
  createIfNotExisting(a_Target)
  l_Content = os.listdir(a_Folder)
  
  print("Copy content from \"" + a_Folder + "\" to \"" + a_Target + "\"...")
  
  for l_Entry in l_Content:
    if l_Entry != "." and l_Entry != "..":
      l_File = a_Folder + "/" + l_Entry
      
      if os.path.isdir(l_File):
        print(l_Entry)
        copyContent(l_File, a_Target + "/" + l_Entry)
      else:
        print("Copy file \"" + l_File + "\" to \"" + a_Target + "\"...")
        shutil.copyfile(l_File, a_Target + "/" + l_Entry)
        g_CopyiedItems["files"][a_Target + "/" + l_Entry] = calendar.timegm(time.gmtime())

def saveFileList():
  with open("copydata_files.dict", "wb") as l_File:
    pickle.dump(g_CopyiedItems, l_File)