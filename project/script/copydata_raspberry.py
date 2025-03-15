import sys, os, shutil
import copydata_functions

if os.path.exists("../../data"):
  shutil.rmtree("../../data")

copydata_functions.copyContent("../../project/data"          , "../../data")
copydata_functions.copyContent("../../project/data_es"       , "../../data")
copydata_functions.copyContent("../../project/data_raspberry", "../../data")
os.remove("../../data/grid_layout.xml") 
copydata_functions.saveFileList()