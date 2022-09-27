import sys, os, shutil

g_CopyiedItems = { }

def createIfNotExisting(a_Folder):
  if not os.path.exists(a_Folder):
    print("Creating folder \"" + a_Folder + "\" ...")
    os.makedirs(a_Folder)

g_TexturesCopied = False

def copyContent(a_Folder, a_Target):
  print(a_Target[-11:])
  createIfNotExisting(a_Target)
  l_Content = os.listdir(a_Folder)
  
  print("Copy content from \"" + a_Folder + "\" to \"" + a_Target + "\"...")
  
  for l_Entry in l_Content:
    if l_Entry != "." and l_Entry != "..":
      if os.path.isdir(a_Folder + "/" + l_Entry):
        print(l_Entry)
        copyContent(a_Folder + "/" + l_Entry, a_Target + "/" + l_Entry)
      else:
        print("Copy file \"" + a_Folder + "/" + l_Entry + "\" to \"" + a_Target + "\"...")
        shutil.copyfile(a_Folder + "/" + l_Entry, a_Target + "/" + l_Entry)
