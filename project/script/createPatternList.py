import sys, os, shutil

f = open("../../data/patterns/patterns.xml", "w")
f.write("<?xml version=\"1.0\"?>\n")
f.write("<patterns>\n")

l_Content = os.listdir("../../data/patterns")

for l_Entry in l_Content:
  if ".png" in l_Entry:
    f.write("  <pattern file=\"" + l_Entry + "\" />\n")

f.write("</patterns>\n")
f.close()