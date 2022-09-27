import sys, os, shutil
import copydata_functions


if os.path.exists("../../data"):
  os.remove("../../data")

copydata_functions.copyContent("../../project/data", "../../data")
