import sys, os, shutil

f = open("../../data/htmlfiles.xml", "w")
f.write("<?xml version=\"1.0\"?>\n")
f.write("<files>\n")

l_Content = os.listdir("../../data/html")

for l_Entry in l_Content:
  f.write("  <file name=\"" + l_Entry + "\" />\n")

f.write("</files>\n")
f.close()