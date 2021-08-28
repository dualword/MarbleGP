import sys, json, os

g_IncludeOutput = "../_source/include/_generated/lua"
g_SourceOutput  = "../_source/source/_generated/lua"
g_IncludePath   = "_generated/lua"

def extract(raw_string, start_marker, end_marker):
  start = raw_string.index(start_marker) + len(start_marker)
  end = raw_string.index(end_marker, start)
  return raw_string[start:end]

def FileComment(a_File):
  a_File.write("// This file was created by the Dustbin::Games LuaBridge Python Script .. (w) 2021 by Christian Keimel\n")

def IsEnum(a_Type, a_Json):
  if "enumerations" in a_Json:
    for l_Value in a_Json["enumerations"]:
      if l_Value == a_Type:
        return True
      
  return False

def CreatePush(a_Name, a_Type, a_Json, a_StatePrefix):
  if a_Type == "int":
    return "lua_pushinteger(" + a_StatePrefix + "pState, " + a_Name + ");\n"
  elif a_Type == "float":
    return "lua_pushnumber(" + a_StatePrefix + "pState, " + a_Name + ");\n"
  elif a_Type == "std::string":
    return "lua_pushstring(" + a_StatePrefix + "pState, " + a_Name + ".c_str());\n"
  elif a_Type == "bool":
    return "lua_pushboolean(" + a_StatePrefix + "pState, " + a_Name + ");\n"
  elif IsEnum(a_Type, a_Json):
    return "lua_pushinteger(" + a_StatePrefix + "pState, (int)" + a_Name + ");\n"
  elif a_Type.startswith("std::vector"):
    l_Ret =  "lua_newtable(" + a_StatePrefix + "pState);\n\n  {\n    int l_iCount = 1;\n    for (" + a_Type + "::const_iterator it = " + a_Name + ".begin(); it != " + a_Name + ".end(); it++) {\n      lua_pushinteger(" + a_StatePrefix + "pState, l_iCount);\n"
    
    l_Type = extract(a_Type, "<", ">")
    
    l_Ret = l_Ret + "      " + CreatePush("(*it)", l_Type, a_Json, a_StatePrefix) + "      lua_settable(" + a_StatePrefix + "pState, -3);\n      l_iCount++;\n    }\n  }\n\n"
    
    return l_Ret
  elif a_Type.startswith("std::map"):
    l_Ret = "lua_newtable(" + a_StatePrefix + "pState);\n\n"
    
    l_Type = extract(a_Type, "<", ">")
    
    l_TypeArray = l_Type.split(",")
    l_Key   = ""
    l_Value = ""
    
    if len(l_TypeArray) == 2:
      l_Key   = l_TypeArray[0].strip()
      l_Value = l_TypeArray[1].strip()
      
    l_Ret = l_Ret + "    for (" + a_Type + "::const_iterator it = " + a_Name + ".begin(); it != " + a_Name + ".end(); it++) {\n"
    l_Ret = l_Ret + "      " + CreatePush("it->first" , l_Key  , a_Json, a_StatePrefix)
    l_Ret = l_Ret + "      " + CreatePush("it->second", l_Value, a_Json, a_StatePrefix)
    l_Ret = l_Ret + "      lua_settable(" + a_StatePrefix + "pState, -3);\n"
    l_Ret = l_Ret + "    }\n"
    
    return l_Ret
  else:
    if a_Type in a_Json["tables"]:
      return a_Name + ".pushToStack(a_pState);\n"
    
def CreateTypeCheck(a_Name, a_Type, a_Json, a_ReturnValue, a_AllowNil, a_StatePrefix, a_Default):
  l_Ret = ""
  
  l_AllowNil = ""
  
  if a_AllowNil:
    l_AllowNil = "!lua_isnil(" + a_StatePrefix + "pState, -1) && "
  
  if a_Type == "int":
    l_Ret = "if (" + l_AllowNil + "!lua_isinteger(" + a_StatePrefix + "pState, lua_gettop(" + a_StatePrefix + "pState)) && !lua_isnumber(" + a_StatePrefix + "pState, lua_gettop(" + a_StatePrefix + "pState))) { luaL_error(" + a_StatePrefix + "pState, \"\\\"" + a_Name + "\\\" is not a number, integer expected.\"); return " + a_Default
  elif a_Type == "float":
    l_Ret = "if (" + l_AllowNil + "!lua_isinteger(" + a_StatePrefix + "pState, lua_gettop(" + a_StatePrefix + "pState)) && !lua_isnumber(" + a_StatePrefix + "pState, lua_gettop(" + a_StatePrefix + "pState))) { luaL_error(" + a_StatePrefix + "pState, \"\\\"" + a_Name + "\\\" is not a number, float expected.\"); return " + a_Default
  elif a_Type == "std::string":
    l_Ret = "if (" + l_AllowNil + "!lua_isstring(" + a_StatePrefix + "pState, lua_gettop(" + a_StatePrefix + "pState))) { luaL_error(" + a_StatePrefix + "pState, \"\\\"" + a_Name + "\\\" is not a string.\"); return " + a_Default
  elif a_Type == "bool":
    l_Ret = "if (" + l_AllowNil + "!lua_isboolean(" + a_StatePrefix + "pState, lua_gettop(" + a_StatePrefix + "pState))) { luaL_error(" + a_StatePrefix + "pState, \"\\\"" + a_Name + "\\\" is not a boolean.\"); return " + a_Default
  else:
    for l_Class in a_Json["classes"]:
      if l_Class == a_Type:
        l_Ret = "if (" + l_AllowNil + "!lua_isuserdata(" + a_StatePrefix + "pState, lua_gettop(" + a_StatePrefix + "pState))) { luaL_error(" + a_StatePrefix + "pState, \"\\\"" + a_Name + "\\\" is not an object.\"); return " + a_Default
  
  if l_Ret != "":
    if a_ReturnValue:
      l_Ret = l_Ret + " 0"
      
    l_Ret = l_Ret + "; }\n"
    
  return l_Ret

def CreatePull(a_Name, a_Type, a_Json, a_Default, a_StatePrefix):
  l_Ret = ""
  
  if a_Default != "":
    l_Ret = "if (!lua_isnil(" + a_StatePrefix + "pState, -1)) {";
    
  if a_Type == "int":
    l_Ret = l_Ret + a_Name + " = (int)lua_tointeger(" + a_StatePrefix + "pState, lua_gettop(" + a_StatePrefix + "pState)); lua_pop(" + a_StatePrefix + "pState, 1);"
  elif a_Type == "float":
    l_Ret = l_Ret + a_Name + " = (float)lua_tonumber(" + a_StatePrefix + "pState, lua_gettop(" + a_StatePrefix + "pState)); lua_pop(" + a_StatePrefix + "pState, 1);"
  elif a_Type == "std::string":
    l_Ret = l_Ret + a_Name + " = lua_tostring(" + a_StatePrefix + "pState, lua_gettop(" + a_StatePrefix + "pState)); lua_pop(" + a_StatePrefix + "pState, 1);"
  elif a_Type == "bool":
    l_Ret = l_Ret + a_Name + " = lua_toboolean(" + a_StatePrefix + "pState, lua_gettop(" + a_StatePrefix + "pState)); lua_pop(" + a_StatePrefix + "pState, 1);"
  elif IsEnum(a_Type, a_Json):
    l_Ret = l_Ret + a_Name + " = (" + a_Type + ")lua_tointeger(" + a_StatePrefix + "pState, lua_gettop(" + a_StatePrefix + "pState)); lua_pop(" + a_StatePrefix + "pState, 1);"
  elif a_Type.startswith("std::vector"):
    l_Type = extract(a_Type, "<", ">")
    l_Key  = a_Name[2:]
    l_Ret = l_Ret + "\n  {\n"
    l_Ret = l_Ret + "    if (lua_istable(" + a_StatePrefix + "pState, -1)) {\n"
    l_Ret = l_Ret + "      int l_iCount = (int)lua_rawlen(" + a_StatePrefix + "pState, -1);\n\n"
    l_Ret = l_Ret + "      for (int l_iIndex = 0; l_iIndex < l_iCount; l_iIndex++) {\n"
    l_Ret = l_Ret + "        int l_iLuaIndex = l_iIndex + 1;\n"
    l_Ret = l_Ret + "        lua_pushinteger(" + a_StatePrefix + "pState, l_iLuaIndex);\n"
    l_Ret = l_Ret + "        lua_gettable(" + a_StatePrefix + "pState, -2);\n"
    l_Ret = l_Ret + "        " + l_Type + " l_Value;\n"
    l_Ret = l_Ret + "        " + CreatePull("l_Value", l_Type, a_Json, "", "a_")
    l_Ret = l_Ret + "        " + a_Name + ".push_back(l_Value);\n"
    l_Ret = l_Ret + "      }\n"
    l_Ret = l_Ret + "      lua_pop(" + a_StatePrefix + "pState, 1);\n"
    l_Ret = l_Ret + "    }\n"
    l_Ret = l_Ret + "  }\n"
  else:
    l_Found = False
    
    for l_Class in a_Json["classes"]:
      if l_Class == a_Type:
        l_Found = True
        l_Ret = l_Ret + a_Name + " = reinterpret_cast<C" + a_Type + " *>(lua_touserdata(" + a_StatePrefix + "pState, lua_gettop(" + a_StatePrefix + "pState))); lua_pop(" + a_StatePrefix + "pState, 1);"
    
    if not l_Found:
      l_Ret = l_Ret + a_Name + ".loadFromStack(" + a_StatePrefix + "pState); lua_pop(" + a_StatePrefix + "pState, 1);"
  
  if a_Default != "":
    l_Ret = l_Ret + " } else " + a_Name + " = " + a_Default + ";"
  
  return l_Ret + "\n"

def CreateArguments(a_Arguments, a_Json, a_Header):
  l_Tables = a_Json["tables"]
  
  for l_Arg in a_Arguments:
    l_Prefix  = ""
    l_Postfix = ""
    
    for l_Table in l_Tables:
      if l_Table == l_Arg["type"]:
        if "prefix" in l_Tables[l_Table]:
          l_Prefix = l_Tables[l_Table]["prefix"]
          
        if "postfix" in l_Tables[l_Table]:
          l_Postfix = l_Tables[l_Table]["postfix"]
          
    for l_BaseType in a_Json:
      for l_Type in a_Json["basetypes"]:
        if l_Type == l_Arg["type"]:
          l_Fixes = a_Json["basetypes"][l_Type]
          if "prefix" in l_Fixes:
            l_Prefix = l_Fixes["prefix"]
          if "postfix" in l_Fixes:
            l_Postfix = l_Fixes["postfix"]
    
    if "prefix" in l_Arg:
      l_Prefix = l_Arg["prefix"]
      
    if "postfix" in l_Arg:
      l_Postfix = l_Arg["postfix"]
    
    a_Header.write(l_Prefix + l_Arg["type"] + " " + l_Postfix)
    
    a_Header.write("a_" + l_Arg["name"])
    if l_Arg != a_Arguments[-1]:
      a_Header.write(", ")
  
def CreateFunctionPrototypes(a_Methods, a_Json, a_Header):
  for l_MethodKey in a_Methods:
    l_Method = a_Methods[l_MethodKey]
    l_Return = "void"
    
    if "return" in l_Method and l_Method["return"]["type"] != "":
      l_Return = l_Method["return"]["type"]
    
    a_Header.write("    /** " + l_Method["comment"] + " **/\n")
    a_Header.write("    " + l_Return + " " + l_MethodKey + "(" )
    
    if "arguments" in l_Method:
      CreateArguments(l_Method["arguments"], a_Json, a_Header)
    
    a_Header.write(");\n")
  
def CreateTables(a_Json):
  l_Tables    = a_Json["tables"]
  l_OutHeader = g_IncludeOutput + "/lua_tables.h"
  l_OutSource = g_SourceOutput  + "/lua_tables.cpp";
  
  print("Writing LUA table structures to \"" + l_OutHeader + "\"...")
  l_Header = open(l_OutHeader, "w")
  
  print("Writing LUA table structures to \"" + l_OutSource + "\"...")
  l_Source = open(l_OutSource, "w")
  
  l_Header.write("#pragma once\n\n")
  
  FileComment(l_Header)
  
  l_Header.write("#include <lua.hpp>\n")
  l_Header.write("#include <string>\n")
  l_Header.write("#include <vector>\n")
  l_Header.write("#include <map>\n")
  l_Header.write("\n")
  
  FileComment(l_Source)
  l_Source.write("#include <" + g_IncludePath + "/lua_tables.h>\n\n")
  
  for l_Table in l_Tables:
    l_Value = l_Tables[l_Table]
    print("  Creating table \"" + l_Table + "\"")
    l_Header.write("// " + l_Value["comment"] + "\n")
    l_Header.write("struct " + l_Table + " {\n")
    
    for l_Field in l_Value["fields"]:
      l_Header.write("  " + l_Field["type"])
      
      if l_Field["type"] == "std::vector":
        l_Header.write("<" + l_Field["element"] + ">")
      elif l_Field["type"] == "std::map":
        l_Header.write("<" + l_Field["key"] + ", " + l_Field["element"] + ">")
      
      l_Header.write(" m_" + l_Field["name"] + ";\n")
    
    l_Header.write("\n")
    l_Header.write("  " + l_Table + "();\n\n")
    l_Header.write("  void pushToStack(lua_State *a_pState) const;\n")
    l_Header.write("  void loadFromStack(lua_State *a_pState);\n")
    l_Header.write("  void copyFrom(const " + l_Table + " &a_cOther);\n")
    l_Header.write("};\n")
    l_Header.write("\n")
    
  for l_Table in l_Tables:
    l_Value = l_Tables[l_Table]
    l_Source.write("// " + l_Value["comment"] + "\n")
    l_Source.write(l_Table + "::" + l_Table + "() {\n")
    
    for l_Field in l_Value["fields"]:
      if l_Field["type"] == "int":
        l_Source.write("  m_" + l_Field["name"] + " = 0;\n")
      elif l_Field["type"] == "float":
        l_Source.write("  m_" + l_Field["name"] + " = 0.0;\n")
      elif l_Field["type"] == "bool":
        l_Source.write("  m_" + l_Field["name"] + " = false;\n")
      elif l_Field["type"] == "std::string":
        l_Source.write("  m_" + l_Field["name"] + " = \"\";\n")
    
    l_Source.write("}\n\n")
    
    l_Source.write("void " + l_Table + "::pushToStack(lua_State *a_pState) const {\n")
    
    l_Source.write("  lua_newtable(a_pState);\n")
    for l_Field in l_Value["fields"]:
      l_Source.write("  lua_pushstring(a_pState, \"" + l_Field["name"] + "\");\n")
      
      if l_Field["type"] != "std::vector" and l_Field["type"] != "std::map":
        l_Source.write("  " + CreatePush("m_" + l_Field["name"], l_Field["type"], a_Json, "a_"))
        l_Source.write("  lua_rawset(a_pState, -3);\n")
      elif l_Field["type"] == "std::vector":
        l_Source.write("  int l_iIndex = 1;\n")
        l_Source.write("  for (std::vector<" + l_Field["element"] + ">::const_iterator it = m_" + l_Field["name"] + ".begin(); it != m_" + l_Field["name"] + ".end(); it++) {\n")
        l_Source.write("    lua_newtable(a_pState);\n")
        l_Source.write("    lua_pushinteger(a_pState, l_iIndex);\n")
        l_Source.write("    " + CreatePush("(*it)", l_Field["element"], a_Json, "a_"))
        l_Source.write("    lua_rawset(a_pState, -3);\n")
        l_Source.write("    l_iIndex++;\n")
        l_Source.write("  }\n")
      elif l_Field["type"] == "std::map":
        l_Source.write("  for (std::map<" + l_Field["key"] + ", " + l_Field["element"] + ">::const_iterator it = m_" + l_Field["name"] + ".begin(); it != m_" + l_Field["name"] + ".end(); it++) {\n")
        l_Source.write("    lua_newtable(a_pState);\n")
        l_Source.write("    " + CreatePush("it->first" , l_Field["key"    ], a_Json, "a_"))
        l_Source.write("    " + CreatePush("it->second", l_Field["element"], a_Json, "a_"))
        l_Source.write("    lua_rawset(a_pState, -3);\n")
        l_Source.write("  }\n")
      
      
    l_Source.write("}\n\n")
    
    l_Source.write("void " + l_Table + "::loadFromStack(lua_State *a_pState) {\n")
    l_Source.write("  luaL_checktype(a_pState, -1, LUA_TTABLE);\n")
    for l_Field in l_Value["fields"]:
      if l_Field["type"] != "std::vector" and l_Field["type"] != "std::map":
        l_Source.write("  lua_pushstring(a_pState, \"" + l_Field["name"] + "\");\n")
        l_Source.write("  lua_gettable(a_pState, -2);\n")
        l_Source.write("  " + CreateTypeCheck("m_" + l_Field["name"], l_Field["type"], a_Json, False, "default" in l_Field, "a_", ""))
        
        if "default" in l_Field:
          l_Source.write("  " + CreatePull     ("m_" + l_Field["name"], l_Field["type"], a_Json, l_Field["default"], "a_"))
        else:
          l_Source.write("  " + CreatePull     ("m_" + l_Field["name"], l_Field["type"], a_Json, "", "a_"))
        
    l_Source.write("}\n\n")
    
    l_Source.write("void " + l_Table + "::copyFrom(const " + l_Table + " &a_cOther) {\n")
    
    for l_Field in l_Value["fields"]:
      l_Source.write("  m_" + l_Field["name"] + " = a_cOther.m_" + l_Field["name"] + ";\n")
    
    l_Source.write("}\n\n")
    
    
  l_Header.close()
  l_Source.close()

def CreateEnums(a_Json):
  if "enumerations" in a_Json:
    l_Enums     = a_Json["enumerations"]
    l_OutHeader = g_IncludeOutput + "/lua_enums.h"
    l_OutSource = g_SourceOutput  + "/lua_enums.cpp";

    print("Writing LUA enumerations to \"" + l_OutHeader + "\"...")
    l_Header = open(l_OutHeader, "w")
    
    print("Writing LUA enumerations to \"" + l_OutSource + "\"...")
    l_Source = open(l_OutSource, "w")
    
    l_Header.write("#pragma once\n\n")
    
    FileComment(l_Header)
    
    l_Header.write("#include <lua.hpp>\n")
    l_Header.write("\n")
    
    FileComment(l_Source)
    l_Source.write("#include <_generated/lua/lua_enums.h>\n")
    l_Source.write("\n")
    
    for l_Value in l_Enums:
      l_Header.write("enum " + l_Value + "{\n")
      
      l_First = True
      
      for l_Item in l_Enums[l_Value]:
        l_Header.write("  " + l_Item["name"])
        
        if l_Item != l_Enums[l_Value][-1]:
          l_Header.write(",")
        
        l_Header.write(" // " + l_Item["comment"] + "\n")
      
      l_Header.write("};\n\n")
      l_Header.write("void enumMakeGlobal_" + l_Value + "(lua_State *a_pState);\n\n")
      
      l_Source.write("void enumMakeGlobal_" + l_Value + "(lua_State *a_pState) {\n")
      
      for l_Item in l_Enums[l_Value]:
        l_Source.write("  lua_pushinteger(a_pState, (int)" + l_Item["name"] + "); // " + l_Item["comment"] + "\n")
        l_Source.write("  lua_setglobal(a_pState, \"" + l_Item["name"] + "\");\n")
        
      l_Source.write("}\n\n")

def CreateObjects(a_Json):
  if "objects" in a_Json:
    l_Objects = a_Json["objects"]
    
    for l_Value in l_Objects:
      l_Object = l_Objects[l_Value]
      l_OutHeader = g_IncludeOutput + "/Lua" + l_Value + ".h"
      l_OutSource = g_IncludeOutput + "/Lua" + l_Value + ".cpp"
      
      print("Creating LUA object files \"" + l_OutHeader + "\" and \"" + l_OutSource)
      
      l_Header = open(l_OutHeader, "w")
      l_Source = open(l_OutSource, "w")
      
      l_Header.write("#pragma once\n\n")
      FileComment(l_Header)
      FileComment(l_Source)
      
        


if not os.path.exists(g_IncludeOutput):
  print("Creating folder \"" + g_IncludeOutput + "\"...")
  os.makedirs(g_IncludeOutput)
  
if not os.path.exists(g_SourceOutput):
  print("Creating folder \"" + g_SourceOutput + "\"...")
  os.makedirs(g_SourceOutput)
  
print("Loading LUA wrapper definitions...")

g_Definitions = json.load(open("luabind.json"))

CreateTables (g_Definitions)
CreateEnums  (g_Definitions)
# CreateObjects(g_Definitions)
