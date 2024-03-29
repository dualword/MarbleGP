from datetime import date
import sys, os, shutil

g_File = open("../data/menu/version.xml", "r")
g_Lines = g_File.readlines()
g_File.close()

g_Output = [ ]
g_Build  = " Build "

for l_Line in g_Lines:
  if l_Line.find("<attribute key=\"Caption\"") != -1 and l_Line.find(g_Build) != -1:
    l_Index   = l_Line.find(g_Build)
    l_Sub     = l_Line[l_Index + len(g_Build):]
    l_Index2  = l_Sub.find("\" />")
    l_Version = l_Sub[:l_Index2]
    l_NewVersion = str(int(l_Version) + 1)
    print("New Build No: " + l_NewVersion)
    
    l_NewLine = l_Line[:l_Index + len(g_Build)] + l_NewVersion + l_Sub[l_Index2:]
    
    g_Output.append(l_NewLine)
    
    g_NewVersion = l_NewLine[l_NewLine.find("Version ") + len("Version "):l_NewLine.rfind("\"")]
  else:
    g_Output.append(l_Line)

print("Updating version.xml ...")  
g_NewFile = open("../data/menu/version.xml", "w")
g_NewFile.writelines(g_Output)
g_NewFile.close()

g_File = open("../MarbleGP_Android/app/build.gradle", "r")
g_Lines = g_File.readlines()
g_File.close()

g_Output = [ ]

print("New Version: \"" + g_NewVersion + "\"")

for l_Line in g_Lines:
  if l_Line.find("versionCode") != -1:
    l_Index   = l_Line.rfind(" ")
    l_Code    = int(l_Line[l_Index:]) + 1
    g_Output.append(l_Line[:l_Index] + " " + str(l_Code) + "\n")
  elif l_Line.find("versionName") != -1:
    g_Output.append(l_Line[:l_Line.find("versionName ") + len("versionName ")] + "\"" + g_NewVersion + "\"\n")
  else:
    g_Output.append(l_Line)
    
print("Updating build.gradle ...")
g_NewFile = open("../MarbleGP_Android/app/build.gradle", "w")
g_NewFile.writelines(g_Output)
g_NewFile.close()

print("Updating \"version.php\" ...")

g_NewFile = open("../release/version.php", "w")
g_NewFile.write("Version " + g_NewVersion)
g_NewFile.close()

g_File = open("setup.iss");
g_Lines = g_File.readlines()
g_File.close()

g_Output = [ ]

print("Updating setup.iss ...")
for l_Line in g_Lines:
  if l_Line.find("AppVersion=") != -1:
    g_Output.append("AppVersion=" + g_NewVersion + "\n")
  else:
    g_Output.append(l_Line)
    
g_NewFile = open("setup.iss", "w")
g_NewFile.writelines(g_Output)
g_NewFile.close()

print("Updating \"changelog.php\" ...")

g_File = open("../release/changelog.php", "r")
g_Lines = g_File.readlines()
g_File.close()

g_Output = [ ]

for l_Line in g_Lines:
  g_Output.append(l_Line)
  
  if l_Line[:4] == "<ul>":
    l_Now =  date.today()
    print(l_Now)
    g_Output.append("  <li class=\"parent\">\n")
    g_Output.append("    <b>" + str(date.today()) + " [" + g_NewVersion + "]</b>\n")
    g_Output.append("    <ul>\n")
    g_Output.append("      <li>Put some release information here</li>\n")
    g_Output.append("    </ul>\n")
    g_Output.append("  </li>\n")


g_NewFile = open("../release/changelog.php", "w")
g_NewFile.writelines(g_Output)
g_NewFile.close()

g_CopyFile = open("release_backup.bat", "w")
g_CopyFile.write("copy ..\\release\\MarbleGP.zip %RELEASE_BACKUP%\\MarbleGP_" + l_NewVersion + ".zip\n")
g_CopyFile.write("copy ..\\release\\MarbleGP_Setup.exe %RELEASE_BACKUP%\\MarbleGP_Setup_" + l_NewVersion + ".exe\n")
g_CopyFile.write("copy ..\\MarbleGP_Android\\app\\release\\app-release.apk %RELEASE_BACKUP%\\marblegp_" + l_NewVersion + ".apk\n");
g_CopyFile.close()
