import sys, json, os

g_IncludeOutput = "../_source/include/_generated/lua"
g_SourceOutput  = "../_source/source/_generated/lua"
g_IncludePath   = "_generated/lua"

def extract(raw_string, start_marker, end_marker):
  start = raw_string.index(start_marker) + len(start_marker)
  end = raw_string.index(end_marker, start)
  return raw_string[start:end]
    
def IsEnum(a_Type, a_Json):
  for l_Value in a_Json["enumerations"]:
    if l_Value == a_Type:
      return True
      
  return False

def TypeWithPrefix(a_Type, a_Json):
  if "classes" in a_Json:
    for l_Class in a_Json["classes"]:
      if a_Type.startswith(l_Class):
        return "C" + a_Type
      
  return a_Type
    
def CreatePush(a_Name, a_Type, a_Json, a_StatePrefix):
  if a_Type == "int":
    return "    lua_pushinteger(" + a_StatePrefix + "pState, " + a_Name + ");\n"
  elif a_Type == "float":
    return "    lua_pushnumber(" + a_StatePrefix + "pState, " + a_Name + ");\n"
  elif a_Type == "std::string":
    return "    lua_pushstring(" + a_StatePrefix + "pState, " + a_Name + ".c_str());\n"
  elif a_Type == "bool":
    return "    lua_pushboolean(" + a_StatePrefix + "pState, " + a_Name + ");\n"
  elif IsEnum(a_Type, a_Json):
    return "    lua_pushinteger(" + a_StatePrefix + "pState, (int)" + a_Name + ");\n"
  elif a_Type.startswith("std::vector"):
    l_Ret =  "    lua_newtable(" + a_StatePrefix + "pState);\n\n    int l_iCount = 1;\n    for (" + a_Type + "::iterator it = " + a_Name + ".begin(); it != " + a_Name + ".end(); it++) {\n      lua_pushinteger(" + a_StatePrefix + "pState, l_iCount);\n"
    
    l_Type = extract(a_Type, "<", ">")
    
    l_Ret = "    " + l_Ret + "      " + CreatePush("(*it)", l_Type, a_Json, a_StatePrefix) + "      lua_settable(" + a_StatePrefix + "pState, -3);\n      l_iCount++;\n    }\n"
    
    return l_Ret
  elif a_Type.startswith("std::map"):
    l_Ret = "    lua_newtable(" + a_StatePrefix + "pState);\n\n"
    
    l_Type = extract(a_Type, "<", ">")
    
    l_TypeArray = l_Type.split(",")
    l_Key   = ""
    l_Value = ""
    
    if len(l_TypeArray) == 2:
      l_Key   = l_TypeArray[0].strip()
      l_Value = l_TypeArray[1].strip()
      
    l_Ret = l_Ret + "        for (" + a_Type + "::iterator it = " + a_Name + ".begin(); it != " + a_Name + ".end(); it++) {\n"
    l_Ret = l_Ret + "          " + CreatePush("it->first" , l_Key  , a_Json, a_StatePrefix)
    l_Ret = l_Ret + "          " + CreatePush("it->second", l_Value, a_Json, a_StatePrefix)
    l_Ret = l_Ret + "          lua_settable(" + a_StatePrefix + "pState, -3);\n"
    l_Ret = l_Ret + "        }\n"
    
    return l_Ret
  else:
    return "    " + a_Name + ".pushToStack(" + a_StatePrefix + "pState);\n"
    
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
    
    l_Ret = "\n    // Found an array of type \"" + l_Type + "\"\n"
    
    l_Ret = l_Ret + "      if (lua_istable(a_pState, lua_gettop(a_pState))) {\n"
    l_Ret = l_Ret + "        lua_pushnil(a_pState);\n\n"
    l_Ret = l_Ret + "        while (lua_next(a_pState, -2)) {\n"
    l_Ret = l_Ret + "          " + l_Type + " l_temp;\n"
    l_Ret = l_Ret + "          " + CreatePull("l_temp", l_Type, a_Json, a_Default, a_StatePrefix) + "\n"
    l_Ret = l_Ret + "          l_" + a_Name + ".push_back(l_temp);\n"
    l_Ret = l_Ret + "        }\n"
    l_Ret = l_Ret + "        lua_pop(a_pState, 1);\n"
    l_Ret = l_Ret + "      }\n"
    l_Ret = l_Ret + "      else {\n"
    l_Ret = l_Ret + "        luaL_error(a_pState, \"Argument is not an array!\");\n"
    l_Ret = l_Ret + "        return 0;\n"
    l_Ret = l_Ret + "      }\n\n"
    
    return l_Ret
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
    
    l_IsClass = False
    
    for l_ArgClass in a_Json["classes"]:
      if l_ArgClass == l_Arg["type"]:
        a_Header.write("C")
        l_IsClass = True
    
    if "prefix" in l_Arg:
      l_Prefix = l_Arg["prefix"]
      
    if "postfix" in l_Arg:
      l_Postfix = l_Arg["postfix"]
    
    a_Header.write(l_Prefix + l_Arg["type"] + " " + l_Postfix)
    
    if l_IsClass:
      a_Header.write("*")
    
    a_Header.write("a_" + l_Arg["name"])
    if l_Arg != a_Arguments[-1]:
      a_Header.write(", ")
  
def CreateFunctionPrototypes(a_Methods, a_Json, a_Header):
  for l_MethodKey in a_Methods:
    l_Method = a_Methods[l_MethodKey]
    l_Return = "void"
    
    if "return" in l_Method and l_Method["return"]["type"] != "":
      l_Return = l_Method["return"]["type"]
    
    a_Header.write("\n    /** " + l_Method["comment"] + " **/\n")
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
  l_Header.write("/** This file was generated by the Dustbin::Games LUAWrapGenerator (w) 2020 by Christian Keimel **/\n\n")
  l_Header.write("#include <lua/luawrap.hpp>\n")
  l_Header.write("#include <string>\n")
  l_Header.write("#include <vector>\n")
  l_Header.write("#include <map>\n")
  l_Header.write("\n")
  l_Header.write("namespace dustbin {\n")
  l_Header.write("  namespace lua {\n\n")
  
  l_Source.write("/** This file was generated by the Dustbin::Games LUAWrapGenerator (w) 2020 by Christian Keimel **/\n\n")
  l_Source.write("#include <" + g_IncludePath + "/lua_tables.h>\n\n")
  
  l_Source.write("namespace dustbin {\n")
  l_Source.write("  namespace lua {\n\n")
  
  for l_Table in l_Tables:
    l_Value = l_Tables[l_Table]
    l_Header.write("    // " + l_Value["comment"] + "\n")
    l_Header.write("    struct " + l_Table + " {\n")
    
    for l_Field in l_Value["fields"]:
      l_Header.write("      " + l_Field["type"])
      
      if l_Field["type"] == "std::vector":
        l_Header.write("<" + l_Field["element"] + ">")
      elif l_Field["type"] == "std::map":
        l_Header.write("<" + l_Field["key"] + ", " + l_Field["element"] + ">")
      
      l_Header.write(" m_" + l_Field["name"] + ";\n")
    
    l_Header.write("\n")
    l_Header.write("      " + l_Table + "();\n\n")
    l_Header.write("      void pushToStack(lua_State *a_pState) const;\n")
    l_Header.write("      void loadFromStack(lua_State *a_pState);\n")
    l_Header.write("      void copyFrom(const " + l_Table + " &a_cOther);\n")
    l_Header.write("    };\n")
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
        l_Source.write("  for (std::vector<" + l_Field["element"] + ">::iterator it = m_" + l_Field["name"] + ".begin(); it != m_" + l_Field["name"] + ".end(); it++) {\n")
        l_Source.write("    lua_newtable(a_pState);\n")
        l_Source.write("    lua_pushinteger(a_pState, l_iIndex);\n")
        l_Source.write("    " + CreatePush("(*it)", l_Field["element"], a_Json, "a_"))
        l_Source.write("    lua_rawset(a_pState, -3);\n")
        l_Source.write("    l_iIndex++;\n")
        l_Source.write("  }\n")
      elif l_Field["type"] == "std::map":
        l_Source.write("  for (std::map<" + l_Field["key"] + ", " + l_Field["element"] + ">::iterator it = m_" + l_Field["name"] + ".begin(); it != m_" + l_Field["name"] + ".end(); it++) {\n")
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
    
    
  l_Header.write("  }\n")
  l_Header.write("}\n")
  
  l_Source.write("  }\n")
  l_Source.write("}\n")
  
  l_Header.close()
  l_Source.close()

def CreateEnums(a_Json):
  l_Enums     = a_Json["enumerations"]
  l_OutHeader = g_IncludeOutput + "/lua_enums.h"
  l_OutSource = g_SourceOutput  + "/lua_enums.cpp";

  print("Writing LUA enumerations to \"" + l_OutHeader + "\"...")
  l_Header = open(l_OutHeader, "w")
  
  print("Writing LUA enumerations to \"" + l_OutSource + "\"...")
  l_Source = open(l_OutSource, "w")
  
  l_Header.write("#pragma once\n\n")
  l_Header.write("/** This file was generated by the Dustbin::Games LUAWrapGenerator (w) 2020 by Christian Keimel **/\n\n")
  l_Header.write("#include <lua/luawrap.hpp>\n")
  l_Header.write("\n")
  
  l_Source.write("/** This file was generated by the Dustbin::Games LUAWrapGenerator (w) 2020 by Christian Keimel **/\n\n")
  l_Source.write("#include <_generated/lua/lua_enums.h>\n")
  l_Source.write("\n")
  
  l_Source.write("namespace dustbin {\n")
  l_Source.write("  namespace lua {\n\n")
  
  l_Header.write("namespace dustbin {\n")
  l_Header.write("  namespace lua {\n\n")
  
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
    
  l_Header.write("  }\n");
  l_Header.write("}\n")
    
  l_Source.write("  }\n");
  l_Source.write("}\n")
  
  l_Source.close()
  l_Header.close()

def FindHeaderFor(a_Type, a_Json, a_Array):
  for l_BaseType in a_Json["datatypes"]:
    if a_Type.startswith(l_BaseType["name"]):
      if l_BaseType["include"] != "" and l_BaseType["include"] not in a_Array:
        a_Array.append(l_BaseType["include"])
      
  for l_BaseType in a_Json["tables"]:
    if a_Type.startswith(l_BaseType):
      l_Include = g_IncludePath + "/lua_tables.h"
      if l_Include not in a_Array:
        a_Array.append(l_Include)
      
  for l_BaseType in a_Json["enumerations"]:
    if a_Type.startswith(l_BaseType):
      l_Include = g_IncludePath + "/lua_enums.h"
      if l_Include not in a_Array:
        a_Array.append(l_Include)
        
  for l_Class in a_Json["classes"]:
    if a_Type[1:].startswith(l_Class):
      l_Include = g_IncludePath + "/C" + l_Class + ".h"
      if l_Include not in a_Array:
        a_Array.append(l_Include)
        
  for l_Script in a_Json["scripts"]:
    if a_Type[11:].startswith(l_Script):
      l_Include = g_IncludePath + "/CLuaScript_" + l_Script + ".h"
      if l_Include not in a_Array:
        a_Array.append(l_Include)
        
  
  if a_Type.startswith("std::vector"):
    l_Type = extract(a_Type, "<", ">").strip()
    FindHeaderFor(l_Type, a_Json, a_Array)
    
  if a_Type.startswith("std::map"):
    l_Type = extract(a_Type, "<", ",").strip()
    FindHeaderFor(l_Type, a_Json, a_Array)
    l_Type = extract(a_Type, ",", ">").strip()
    FindHeaderFor(l_Type, a_Json, a_Array)
  
  
def FillHeaders(a_Class, a_Json, a_Array):
  if "inherits" in a_Class and a_Class["inherits"] != "":
    for l_BaseType in a_Json["datatypes"]:
      if l_BaseType["name"].startswith(a_Class["inherits"]):
        if l_BaseType["include"] not in a_Array:
          a_Array.append(l_BaseType["include"])
        
    for l_BaseType in a_Json["classes"]:
      if l_BaseType == a_Class["inherits"]:
        if g_IncludePath + "/C" + l_BaseType + ".h" not in a_Array:
          a_Array.append(g_IncludePath + "/C" + l_BaseType + ".h")
        
    
  if "members" in a_Class:
    for l_Member in a_Class["members"]:
      FindHeaderFor(l_Member["type"], a_Json, a_Array)
            
  if "implements" in a_Class:
    for l_Parent in a_Class["implements"]:
      FindHeaderFor(l_Parent, a_Json, a_Array)
        
  if "methods" in a_Class:
    for l_Type in a_Class["methods"]:
      for l_Name in a_Class["methods"][l_Type]:
        l_Method = a_Class["methods"][l_Type][l_Name]
        if "arguments" in l_Method:
          for l_Argument in l_Method["arguments"]:
            FindHeaderFor(l_Argument["type"], a_Json, a_Array)
              
        if "return" in l_Method:
          FindHeaderFor(l_Method["return"]["type"], a_Json, a_Array)
            

def CreateHeader(a_FilePath, a_Name, a_Class, a_Type, a_Json):
  print("Writing \"" + a_FilePath + "...")
  l_Header = open(a_FilePath, "w")
  
  l_Header.write("/* Generated by the Dustbin::Games LuaBind Python Script (w) 2020 by Christian Keimel / Dustbin::Games */\n")
  l_Header.write("#pragma once\n\n")
  
  l_Includes = [ ]
  FillHeaders(a_Class, a_Json, l_Includes)
  l_Includes.append("lua/luawrap.hpp")
  l_Includes.append("lua/ILuaClass.h")
  
  for l_Include in l_Includes:
    l_Header.write("#include <" + l_Include + ">\n")
    
  if "singletons" in a_Class:
    for l_Singleton in a_Class["singletons"]:
      l_Header.write("#include <" + g_IncludePath + "/CLuaSingleton_" + l_Singleton + ".h>\n")
    
  l_Header.write("\n")
  l_Header.write("namespace dustbin {\n")
  l_Header.write("  namespace lua {\n\n")
  
  l_Header.write("    /**\n")
  l_Header.write("     * @class C" + a_Name + "\n")
  l_Header.write("     * @author Dustbin::Games LuaBind Python Script\n")
  
  if "comment" in a_Class:
    l_Header.write("     * " + a_Class["comment"] + "\n")
    
  l_Header.write("     */\n")
  
  l_Header.write("    class " + a_Name)
  
  if "inherits" in a_Class and a_Class["inherits"] != "":
    l_Header.write(" : public C" + a_Class["inherits"])
  else:
    l_Header.write(" : public ILuaClass")
    
  if "implements" in a_Class:
    for l_Parent in a_Class["implements"]:
      l_Header.write(", public " + l_Parent)
      
  
  l_Header.write(" {\n")
  if "members" in a_Class or a_Type == "singleton" or a_Type == "script":
    l_Header.write("      protected:\n")
  
  if "members" in a_Class:
    for l_Member in a_Class["members"]:
      l_Header.write("        " + l_Member["type"] + " m_" + l_Member["name"])
      
      if "arraysize" in l_Member:
        l_Header.write("[" + l_Member["arraysize"] + "]")
      
      l_Header.write(";")
      if "comment" in l_Member:
        l_Header.write("  /**< " + l_Member["comment"] + " */")
        
      l_Header.write("\n")
  
  l_Header.write("\n")
  
  if "singletons" in a_Class:
    for l_Singleton in a_Class["singletons"]:
      l_Header.write("        CLuaSingleton_" + l_Singleton + " *m_LuaSgt_" + l_Singleton + ";\n")
    l_Header.write("\n")
  
  l_Header.write("      private:\n")
  l_Header.write("        void *getObjectFromLuaStack(const std::string &a_sIdentifier);\n\n")
  l_Header.write("      public:\n")
  l_Header.write("        " + a_Name)
  
  if a_Type == "singleton":
    l_Header.write("(lua_State *a_State);\n")
  elif a_Type == "script":
    l_Header.write("(const std::string &a_sScript);\n")
  else:
    l_Header.write("();\n")
    
  l_Header.write("        virtual ~" + a_Name + "();\n\n")
  
  if "methods" in a_Class:
    for l_Type in a_Class["methods"]:
      for l_Name in a_Class["methods"][l_Type]:
        l_Method = a_Class["methods"][l_Type][l_Name]
        l_Header.write("        /**\n")
        if "comment" in l_Method:
          l_Header.write("         * " + l_Method["comment"] + "\n")
          
        if "arguments" in l_Method:
          for l_Argument in l_Method["arguments"]:
            l_Header.write("         * @param ")
            
            if "comment" in l_Argument:
              l_Header.write(l_Argument["comment"])
              
            l_Header.write("\n")
            
        l_Header.write("         */\n")
        l_Header.write("        ")
        
        if "type" in l_Method:
          l_Header.write("    " + l_Method["type"] + " ")
          
        if "return" in l_Method:
          l_Header.write(l_Method["return"]["type"])
        else:
          l_Header.write("void")
          
        l_Header.write(" " + l_Name + "(")
        
        if "arguments" in l_Method:
          CreateArguments(l_Method["arguments"], a_Json, l_Header)
          
        l_Header.write(");\n")
        
  if "implements" in a_Class:
    for l_Parent in a_Class["implements"]:
      l_Header.write("\n")
      l_Header.write("    // Overriding virtual methods inherited from " + l_Parent + "\n")
      for l_BaseType in a_Json["baseclasses"]:
        if l_BaseType == l_Parent:
          for l_Virtual in a_Json["baseclasses"][l_BaseType]:
            l_Header.write("    " + l_Virtual + "\n")
  
  if a_Type == "class":
    l_Header.write("\n")
    l_Header.write("    static void registerMethods(lua_State *a_pState);\n")
  l_Header.write("    };\n")
  
  l_Header.write("  }\n")
  l_Header.write("}\n")
  l_Header.close()

  

def CreateClassHeaders(a_Json):
  l_Includes = [ ]
  
  for l_ClassName in a_Json["classes"]:
    l_Class = a_Json["classes"][l_ClassName]
    
    if "virtual" in l_Class and l_Class["virtual"] == 1:
      CreateHeader(g_IncludeOutput + "/C" + l_ClassName + ".h", "C" + l_ClassName, l_Class, "class", a_Json)
      l_Includes.append(g_IncludeOutput + "/C" + l_ClassName + ".h")

  for l_ClassName in a_Json["classes"]:
    l_Class = a_Json["classes"][l_ClassName]
    
    if "virtual" not in l_Class or l_Class["virtual"] != 1:
      CreateHeader(g_IncludeOutput + "/C" + l_ClassName + ".h", "C" + l_ClassName, l_Class, "class", a_Json)
      l_Includes.append(g_IncludeOutput + "/C" + l_ClassName + ".h")
      
def CreateSingletonHeaders(a_Json):
  for l_Name in a_Json["singletons"]:
    l_Singleton = a_Json["singletons"][l_Name]
    CreateHeader(g_IncludeOutput + "/CLuaSingleton_" + l_Name + ".h", "CLuaSingleton_" + l_Name, l_Singleton, "singleton", a_Json)
    
def CreateScriptHeaders(a_Json):
  for l_Name in a_Json["scripts"]:
    l_Script = a_Json["scripts"][l_Name]
    CreateHeader(g_IncludeOutput + "/CLuaScript_" + l_Name + ".h", "CLuaScript_" + l_Name, l_Script, "script", a_Json)

def WriteConstructorPrefix(a_Class, a_Source):
  if "prefix" in a_Class:
    a_Source.write("  // Commands defined in the \"prefix\" section of the JSON class definition\n")
    for l_Prefix in a_Class["prefix"]:
      a_Source.write("  " + l_Prefix + "\n")
    a_Source.write("\n")

def CreateClassBinding(a_Source, a_Name, a_Class, a_Type, a_Json):
  a_Source.write("\n")
  
  l_Comment = "LUA Binding for class \"" + a_Name + "\""
  
  l_Dummy = ""
  
  while len(l_Dummy) < len(l_Comment):
    l_Dummy = l_Dummy + "*"
  
  a_Source.write("    // " + l_Dummy   + "\n")
  a_Source.write("    // " + l_Comment + "\n")
  a_Source.write("    // " + l_Dummy   + "\n")
  
  if "methods" in a_Class:
    if "lua_from" in a_Class["methods"]:
      for l_Name in a_Class["methods"]["lua_from"]:
        l_Method = a_Class["methods"]["lua_from"][l_Name]
        
        if "comment" in l_Method:
          a_Source.write("    // " + l_Method["comment"] + "\n")
          
        a_Source.write("    static int " + a_Name + "_" + l_Name + "(lua_State *a_pState) {\n")
        
        if a_Type == "singleton":
          a_Source.write("      " + a_Name + " *l_pObject = nullptr;\n")
          a_Source.write("      lua_getglobal(a_pState, \"" + a_Class["globalidentifier"] + "\");\n")
          a_Source.write("      if (lua_islightuserdata(a_pState, lua_gettop(a_pState))) {\n")
          a_Source.write("        l_pObject = reinterpret_cast<" + a_Name + " *>(lua_touserdata(a_pState, lua_gettop(a_pState)));\n")
          a_Source.write("        lua_pop(a_pState, 1);\n")
          a_Source.write("      }\n")
        else:
          a_Source.write("      if (!lua_isuserdata(a_pState, 1)) {\n")
          a_Source.write("        luaL_error(a_pState, \"Invalid receiver.\");\n")
          a_Source.write("        return 0;\n  }\n")
          a_Source.write("\n")
          a_Source.write("      " + a_Name + " *l_pObject = (" + a_Name + " *)lua_touserdata(a_pState, 1);\n")
        
        if "virtual" not in a_Class or a_Class["virtual"] != 1:
          a_Source.write("      l_pObject->setLuaState(a_pState);\n")
        
        if "arguments" in l_Method:
          l_Method["arguments"].reverse()
          l_MinArgs = 1
          l_MaxArgs = 1
          
          for l_Arg in l_Method["arguments"]:
            l_MaxArgs = l_MaxArgs + 1
            
            if "default" not in l_Arg:
              l_MinArgs = l_MinArgs + 1
          
          if "prefix" in l_Method:
            print("Prefix found in method " + l_Name + " of object " + a_Name)
            a_Source.write("\n      // Code defined in \"Prefix\" node\n")
            for l_Prefix  in l_Method["prefix"]:
              a_Source.write("  " + l_Prefix + "\n")
            a_Source.write("      // Code defined in \"Prefix\" node (end)\n\n")
          
          a_Source.write("      int l_iArgC = lua_gettop(a_pState);\n")
          a_Source.write("      if (l_iArgC < " + str(l_MinArgs) + ") { luaL_error(a_pState, \"Not enough arguments for function \\\"" + l_Name + "\\\". " + str(l_MinArgs - 1) + " argument")
          
          if int(l_MinArgs) != 2:
            a_Source.write("s")
          
          a_Source.write(" required.\"); return 0; }\n\n")
          
          for l_Arg in l_Method["arguments"]:
            a_Source.write("      ")
            
            l_IsClass = False
            
            for l_ArgClass in a_Json["classes"]:
              if l_Arg["type"].startswith(l_ArgClass):
                a_Source.write("C")
                l_IsClass = True
                
            a_Source.write(l_Arg["type"] + " ")
            
            if l_IsClass:
              a_Source.write("*") 
            
            a_Source.write("l_" + l_Arg["name"] + ";\n")
            
            if l_MaxArgs > l_MinArgs:
              a_Source.write("      if (lua_gettop(a_pState) >= " + str(l_MaxArgs) + ") {\n")
              l_Check = CreateTypeCheck(l_Arg["name"], l_Arg["type"], a_Json, True, False, "a_", "")
              
              if l_Check != "":
                a_Source.write("    " + l_Check)
                
              if l_Arg["type"].startswith("std::vector"):
                a_Source.write("        ")
              else:
                a_Source.write("        l_")
              a_Source.write(CreatePull(l_Arg["name"], l_Arg["type"], a_Json, "", "a_"))
              a_Source.write("      }\n  else {\n")
              a_Source.write("        l_" + l_Arg["name"] + " = " + l_Arg["default"] + ";\n")
              a_Source.write("      }\n")
            else:
              l_Check = CreateTypeCheck(l_Arg["name"], l_Arg["type"], a_Json, True, False, "a_", "")
              
              if l_Check != "":
                a_Source.write("      " + l_Check)
                
              if l_Arg["type"].startswith("std::vector"):
                a_Source.write("      ")
              else:
                a_Source.write("      l_")
              a_Source.write(CreatePull(l_Arg["name"], l_Arg["type"], a_Json, "", "a_"))
            
            l_MaxArgs = l_MaxArgs - 1
            
          l_Method["arguments"].reverse()
        
        l_Return = ""
        a_Source.write("\n      ")
        if "return" in l_Method:
          l_Return = l_Method["return"]["type"]
          a_Source.write(l_Method["return"]["type"] + " l_Ret = ")
          
        a_Source.write("l_pObject->" + l_Name + "(")
        
        if "arguments" in l_Method:
          for l_Argument in l_Method["arguments"]:
            a_Source.write("l_" + l_Argument["name"])
            if l_Argument != l_Method["arguments"][-1]:
              a_Source.write(", ")
        
        a_Source.write(");\n")
        
        if "return" in l_Method:
          a_Source.write("    " + CreatePush("l_Ret", l_Method["return"]["type"], a_Json, "a_"))
        
        
        if "postfix" in l_Method:
          print("Postfix found in method " + l_Name + " of object " + a_Name)
          a_Source.write("\n  // Code defined in \"Postfix\" node\n")
          for l_Postfix in l_Method["postfix"]:
            a_Source.write("  " + l_Postfix + "\n")
          a_Source.write("  // Code defined in \"Postfix\" node (end)\n\n")
            
        
        if l_Return != "":
          CreatePush("l_Ret", l_Method["return"]["type"], a_Json, "a_")
          a_Source.write("      return 1;\n")
        else:
          a_Source.write("      return 0;\n")
        
        a_Source.write("    }\n\n")
        
      a_Source.write("    static luaL_Reg S" + a_Name + "Methods[] = {\n")
      
      for l_Name in a_Class["methods"]["lua_from"]:
        a_Source.write("      { \"" + l_Name + "\", " + a_Name + "_" + l_Name + " },\n")
        
      if "inherits" in a_Class:
        for l_Class in a_Json["classes"]:
          if l_Class == a_Class["inherits"]:
            a_Source.write("  // Methods inherited from \"" + a_Class["inherits"] + "\"\n")
            l_Parent = a_Json["classes"][l_Class]
            if "lua_from" in l_Parent["methods"]:
              for l_Method in l_Parent["methods"]["lua_from"]:
                a_Source.write("  { \"" + l_Method + "\", C" + l_Class + "_" + l_Method + "},\n")
      
      a_Source.write("      { NULL, NULL }\n")
      a_Source.write("    };\n\n")
      
      if a_Type == "class":
        a_Source.write("    void " + a_Name + "::registerMethods(lua_State *a_pState) {\n")
        a_Source.write("      LuaWrap::register_class<" + a_Name + ">(a_pState, \"" + a_Name[1:] + "\", S" + a_Name + "Methods, NULL, LuaWrap::InternalLua::default_new<" + a_Name + ">, LuaWrap::InternalLua::default_gc<" + a_Name + ">);\n")
        a_Source.write("    }\n\n")
      
    if "lua_to" in a_Class["methods"]:
      for l_Name in a_Class["methods"]["lua_to"]:
        l_Method = a_Class["methods"]["lua_to"][l_Name]
        if "comment" in l_Method:
          a_Source.write("    // " + l_Method["comment"] + "\n")
          
          if "return" in l_Method and l_Method["return"] != "":
            a_Source.write(l_Method["return"]["type"])
          else:
            a_Source.write("    void")
            
          a_Source.write(" " + a_Name + "::" + l_Name + "(")
          
          if "arguments" in l_Method:
            CreateArguments(l_Method["arguments"], a_Json, a_Source)
          a_Source.write(") {\n")
          
          if "return" in l_Method:
            a_Source.write("      " + l_Method["return"]["type"] + " l_Ret")
            
            l_Ret = ""
            if "default" in l_Method["return"]:
              a_Source.write(" = " + l_Method["return"]["default"])
              l_Ret = l_Method["return"]["default"]
              
            a_Source.write(";\n\n")
              
          if "prefix" in l_Method:
            print("Prefix found in method " + l_Name + " of object " + a_Name)
            a_Source.write("\n      // Code defined in \"Prefix\" node\n")
            for l_Prefix  in l_Method["prefix"]:
              a_Source.write("  " + l_Prefix + "\n")
            a_Source.write("      // Code defined in \"Prefix\" node (end)\n\n")
          
          a_Source.write("      lua_getglobal(m_pState, \"" + l_Name + "\");\n")
          a_Source.write("      if (!lua_isnil(m_pState, -1)) {\n")
          
          l_Args = 0
          if "arguments" in l_Method:
            for l_Argument in l_Method["arguments"]:
              l_Args = l_Args + 1
              a_Source.write("    " + CreatePush("a_" + l_Argument["name"], l_Argument["type"], a_Json, "m_"))
          
          l_Return = 0
          if "return" in l_Method:
            l_Return = 1

          a_Source.write("        if (lua_pcall(m_pState, " + str(l_Args) + ", " + str(l_Return) + ", 0) != 0) {\n")
          a_Source.write("          std::string l_sMessage = std::string(\"ERROR: function \\\"" + l_Name + "\\\" failed: \\\"\") + std::string(lua_tostring(m_pState, -1)) + std::string(\"\\\"\");\n")
          a_Source.write("          printf(\"%s\\n\", l_sMessage.c_str());\n")
          a_Source.write("        }\n")
          
          if "return" in l_Method:
            a_Source.write("        " + CreateTypeCheck("l_Ret", l_Method["return"]["type"], a_Json, False, "default" in l_Method["return"], "m_", "false"))
            a_Source.write("        " + CreatePull("l_Ret", l_Method["return"]["type"], a_Json, l_Ret, "m_"))
            
          a_Source.write("      }\n")
          a_Source.write("      else lua_pop(m_pState, 1);\n\n")
                          
          if "return" in l_Method:
            a_Source.write("      return l_Ret;\n")
          
          if "postfix" in l_Method:
            print("Postfix found in method " + l_Name + " of object " + a_Name)
            a_Source.write("\n      // Code defined in \"Postfix\" node\n")
            for l_Postfix in l_Method["postfix"]:
              a_Source.write("      " + l_Postfix + "\n")
            a_Source.write("      // Code defined in \"Postfix\" node (end)\n\n")
          a_Source.write("    }\n\n")
    
    a_Source.write("    void *" + a_Name + "::getObjectFromLuaStack(const std::string &a_sIdentifier) {\n")
    a_Source.write("      lua_getglobal(m_pState, a_sIdentifier.c_str());\n")
    a_Source.write("      if (lua_islightuserdata(m_pState, lua_gettop(m_pState))) {\n")
    a_Source.write("        void *l_pObject = reinterpret_cast<" + a_Name + " *>(lua_touserdata(m_pState, lua_gettop(m_pState)));\n")
    a_Source.write("        lua_pop(m_pState, 1);\n")
    a_Source.write("        return l_pObject;\n")
    a_Source.write("      }\n")
    a_Source.write("      else return nullptr;\n")
    a_Source.write("    }\n\n")
    
    if a_Type == "singleton":
      a_Source.write("    " + a_Name + "::" + a_Name + "(lua_State *a_pState) {\n")
      WriteConstructorPrefix(a_Class, a_Source)
      a_Source.write("      m_pState = a_pState;\n")
      a_Source.write("      LuaWrap::register_singleton(m_pState, \"" + a_Name[14:] + "\", S" + a_Name + "Methods);\n")
      a_Source.write("\n")
      
      if "globalidentifier" in a_Class:
        a_Source.write("      lua_pushlightuserdata(m_pState, (void *)this);\n")
        a_Source.write("      lua_setglobal(m_pState, \"" + a_Class["globalidentifier"] + "\");\n")
        
      if "members" in a_Class:
        a_Source.write("\n")
        for l_Member in a_Class["members"]:
          if "default" in l_Member:
            if "arraysize" in l_Member:
              a_Source.write("      for (int i = 0; i < " + l_Member["arraysize"] + "; i++) m_" + l_Member["name"] + "[i] = " + l_Member["default"] + ";\n")
            else:
              a_Source.write("      m_" + l_Member["name"] + " = " + l_Member["default"] + ";\n")
        
      a_Source.write("    }\n\n")
      
      a_Source.write("    " + a_Name + "::~" + a_Name + "() {\n    }\n\n")
    elif a_Type == "script":
      a_Source.write("    " + a_Name + "::" + a_Name + "(const std::string &a_sScript) {\n")
      WriteConstructorPrefix(a_Class, a_Source)
      
      if "members" in a_Class:
        a_Source.write("\n")
        for l_Member in a_Class["members"]:
          if "default" in l_Member and l_Member["default"] != "":
            if "arraysize" in l_Member:
              a_Source.write("      for (int i = 0; i < " + l_Member["arraysize"] + "; i++) m_" + l_Member["name"] + "[i] = " + l_Member["default"] + ";\n")
            else:
              a_Source.write("      m_" + l_Member["name"] + " = " + l_Member["default"] + ";\n")
        
      a_Source.write("      // Create LUA State\n")
      a_Source.write("      m_pState = luaL_newstate();\n")
      a_Source.write("      luaL_openlibs(m_pState);\n\n")
      a_Source.write("      // Store a reference of this script on the LUA stack\n")
      a_Source.write("      lua_pushlightuserdata(m_pState, (void *)this);\n")
      a_Source.write("      lua_setglobal(m_pState, \"script_cinstance\");\n")
      
      if "globalidentifier" in a_Class:
        a_Source.write("\n")
        a_Source.write("      // Store the instance again with the identifier defined in the JSON\n")
        a_Source.write("      lua_pushlightuserdata(m_pState, (void *)this);\n")
        a_Source.write("      lua_setglobal(m_pState, \"" + a_Class["globalidentifier"] + "\");\n")
        
      if "exports" in a_Class:
        a_Source.write("\n      // Export LUA classes\n")
        for a_Export in a_Class["exports"]:
          a_Source.write("      C" + a_Export + "::registerMethods(m_pState);\n")
        
      if "enums" in a_Class:
        a_Source.write("\n      // Export LUA enumerations\n")
        for a_Enum in a_Class["enums"]:
          a_Source.write("      enumMakeGlobal_" + a_Enum + "(m_pState);\n")
      
      if "singletons" in a_Class:
        a_Source.write("\n      // Create and register singletons\n")
        for l_Singleton in a_Class["singletons"]:
          a_Source.write("      m_LuaSgt_" + l_Singleton + " = new CLuaSingleton_" + l_Singleton + "(m_pState);\n")
        
      a_Source.write("      int l_iResult = luaL_dostring(m_pState, a_sScript.c_str());\n")
      a_Source.write("    \n")
      a_Source.write("      if (l_iResult != 0) {\n")
      a_Source.write("        if (!lua_isnil(m_pState, -1)) {\n")
      a_Source.write("          std::string l_sMsg = \"LUA Error: \";\n")
      a_Source.write("          l_sMsg += lua_tostring(m_pState, -1);\n")
      a_Source.write("          m_sError = l_sMsg;\n")
      a_Source.write("        }\n")
      a_Source.write("        else {\n")
      a_Source.write("          lua_close(m_pState);\n")
      a_Source.write("          m_pState = nullptr;\n")
      a_Source.write("          std::string l_sMsg = \"Error \";\n")
      a_Source.write("          l_sMsg += std::to_string(l_iResult);\n")
      a_Source.write("          l_sMsg += \" on \\\"" + a_Name + "\\\" execution.\";\n")
      a_Source.write("          m_sError = l_sMsg;\n")
      a_Source.write("        }\n")
      a_Source.write("      }\n")
      a_Source.write("    }\n\n")
      
      a_Source.write("    " + a_Name + "::~" + a_Name + "() {\n")
      a_Source.write("      if (m_pState != nullptr)\n")
      a_Source.write("        lua_close(m_pState);\n")
      a_Source.write("    }\n")

def CreateBinding(a_Json):
  l_Name = g_SourceOutput + "/lua_binding.cpp"
  
  print("Writing " + l_Name + "...")
  
  l_Source = open(l_Name, "w")
  l_Source.write("/** This file was generated by the Dustbin::Games LUAWrapGenerator (w) 2020 by Christian Keimel **/\n\n")
  
  if "tables" in a_Json:
    l_Source.write("#include <" + g_IncludePath + "/lua_tables.h>\n")
    
  if "enumerations" in a_Json:
    l_Source.write("#include <" + g_IncludePath + "/lua_enums.h>\n")
    
  if "classes" in a_Json:
    for l_ClassName in a_Json["classes"]:
      l_Source.write("#include <" + g_IncludePath + "/C" + l_ClassName + ".h>\n")
      
      l_Includes = [ ]
      
      FillHeaders(a_Json["classes"][l_ClassName], a_Json, l_Includes)
      
  if "singletons" in a_Json:
    for l_Singleton in a_Json["singletons"]:
      l_Source.write("#include <" + g_IncludePath + "/CLuaSingleton_" + l_Singleton + ".h>\n")
      
  if "scripts" in a_Json:
    for l_Script in a_Json["scripts"]:
      l_Source.write("#include <" + g_IncludePath + "/CLuaScript_" + l_Script + ".h>\n")
  
  l_Source.write("\n")
  l_Source.write("namespace dustbin {\n")
  l_Source.write("  namespace lua {\n")
  
  if "classes" in a_Json:
    for l_Class in a_Json["classes"]:
      if "virtual" in a_Json["classes"][l_Class] and a_Json["classes"][l_Class]["virtual"] == 1:
        CreateClassBinding(l_Source, "C" + l_Class, a_Json["classes"][l_Class], "class", a_Json)
  
    for l_Class in a_Json["classes"]:
      if "virtual" not in a_Json["classes"][l_Class] or a_Json["classes"][l_Class]["virtual"] != 1:
        CreateClassBinding(l_Source, "C" + l_Class, a_Json["classes"][l_Class], "class", a_Json)
  
  if "singletons" in a_Json:
    for l_Singleton in a_Json["singletons"]:
      if "virtual" in a_Json["singletons"][l_Singleton] and a_Json["singletons"][l_Singleton]["virtual"] == 1:
        CreateClassBinding(l_Source, "CLuaSingleton_" + l_Singleton, a_Json["singletons"][l_Singleton], "singleton", a_Json)
        
      if "virtual" not in a_Json["singletons"][l_Singleton] or a_Json["singletons"][l_Singleton]["virtual"] != 1:
        CreateClassBinding(l_Source, "CLuaSingleton_" + l_Singleton, a_Json["singletons"][l_Singleton], "singleton", a_Json)
        
  if "scripts" in a_Json:
    for l_Script in a_Json["scripts"]:
      CreateClassBinding(l_Source, "CLuaScript_" + l_Script, a_Json["scripts"][l_Script], "script", a_Json)
  
  
  l_Source.write("  }\n")
  l_Source.write("}\n")
  
  l_Source.write("\n")
  l_Source.close()
  
if not os.path.exists(g_IncludeOutput):
  print("Creating folder \"" + g_IncludeOutput + "\"...")
  os.makedirs(g_IncludeOutput)
  
if not os.path.exists(g_SourceOutput):
  print("Creating folder \"" + g_SourceOutput + "\"...")
  os.makedirs(g_SourceOutput)
  
print("Loading LUA wrapper definitions...")

g_Definitions = json.load(open("luabind.json"))

CreateTables          (g_Definitions)
CreateEnums           (g_Definitions)
CreateClassHeaders    (g_Definitions)
CreateSingletonHeaders(g_Definitions)
CreateScriptHeaders   (g_Definitions)
CreateBinding         (g_Definitions)
