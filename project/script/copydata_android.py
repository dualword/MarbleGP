import sys, os, shutil
import copydata_functions

if os.path.exists("../../data"):
  os.remove("../../data")

copydata_functions.copyContent("../../project/data"        , "../../data")
copydata_functions.copyContent("../../project/data_es"     , "../../data")
copydata_functions.copyContent("../../project/data_android", "../../data")
