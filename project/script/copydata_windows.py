import sys, os, shutil
import copydata_functions


if os.path.exists("../../data"):
  os.remove("../../data")

copydata_functions.g_CopyiedItems["rootfolder"] = "../../project/data"
copydata_functions.copyContent("../../project/data", "../../data")
copydata_functions.saveFileList()