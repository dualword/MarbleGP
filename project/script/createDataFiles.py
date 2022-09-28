import sys, os, shutil

g_TempFolderGl      = "temp_gl/data"
g_TempFolderEs      = "temp_es/data"
g_TempFolderAndroid = "temp_android/data"

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


copyContent("../../project/data"        , g_TempFolderGl)
copyContent("../../project/data"        , g_TempFolderEs)
copyContent("../../project/data_es"     , g_TempFolderEs)
copyContent("../../project/data"        , g_TempFolderAndroid)
copyContent("../../project/data_es"     , g_TempFolderAndroid)
copyContent("../../project/data_android", g_TempFolderAndroid)

os.chdir("temp_gl")
os.system("7z.exe a -r -tzip marblegp.dat data")
os.chdir("../temp_es")
os.system("7z.exe a -r -tzip marblegp.dat data")
os.chdir("../temp_android")
os.system("7z.exe a -r -tzip marblegp.dat data")
os.chdir("..")

g_AndroidPath = "../MarbleGP_Android/MarbleGP_Android/MarbleGP_Android.Packaging/assets/marblegp.dat"

if os.path.exists(g_AndroidPath):
  os.remove(g_AndroidPath)

print("Copy marblegp.dat for Android ...")
shutil.copyfile("temp_android/marblegp.dat", g_AndroidPath)
print("Copy marblegp.dat for Windows ...")
shutil.copyfile("temp_gl/marblegp.dat", "../../MarbleGP/marblegp.dat")
print("Ready.")

shutil.rmtree("temp_es/data")
shutil.rmtree("temp_gl/data")
shutil.rmtree("temp_android/data")