import sys, json, os

def WriteFileHeader(a_File):
  a_File.write("/** This file was created by the Dustbin::Games Message Generator Python Script **/\n\n")
  
def StartNamespace(a_File):
  a_File.write("namespace dustbin {\n")
  a_File.write("  namespace messages {\n\n")
  
def EndNamepsaces(a_File):
  a_File.write("  }  // namespace messages\n")
  a_File.write("}    // namespace dustbin\n\n")

def CreateEnums(a_Data):
  global g_HeaderFolder
  
  l_File = open(g_HeaderFolder + "CMessageEnums.h", "w")
  WriteFileHeader(l_File)
  
  l_File.write("#pragma once\n\n")
  
  StartNamespace(l_File)
  
  l_File.write("    /**\n")
  l_File.write("     * This enumeration contains all the message IDs\n")
  l_File.write("     * @see dustbin::messages::IMessage::getID\n")
  l_File.write("     */\n")
  l_File.write("    enum class enMessageIDs {\n")
  
  a_FirstMessage = True
  
  for l_MessageId in a_Data:
    if a_FirstMessage:
      a_FirstMessage = False
    else:
      l_File.write(",\n")
      
    l_File.write("      " + l_MessageId[0] + " = " + l_MessageId[1])
  
  l_File.write("\n    }\n")
  
  EndNamepsaces(l_File)
  
  l_File.close()

def CreateParameter(a_Param, a_Static):  
  if a_Param["type"] in a_Static:
    return "const " + a_Param["type"] + " &"
  else:
    return a_Param["type"] + " "

def CreateMessages(a_Data, a_Static):
  global g_HeaderFolder
  global g_SourceFolder
  global g_Include
  
  l_Header = open(g_HeaderFolder + "/CMessages.h"  , "w")
  l_Source = open(g_SourceFolder + "/CMessages.cpp", "w")
  
  WriteFileHeader(l_Header)
  WriteFileHeader(l_Source)
  
  l_Header.write("#pragma once\n\n")
  l_Header.write("#include <" + g_Include  + "CMessageEnums.h>\n")
  l_Header.write("#include <messages/IMessage.h>\n\n")
  l_Header.write("#include <irrlicht.h>\n")
  l_Header.write("#include <string>\n\n")
  
  l_Source.write("#include <" + g_Include + "CMessages.h>\n\n")
  
  StartNamespace(l_Source)
  StartNamespace(l_Header)
  
  for l_MessageName in a_Data:
    l_Message = a_Data[l_MessageName]
    print "Generating interface for \"" + l_MessageName + "\"..."
    
    l_Header.write("    /**\n")
    l_Header.write("     * @class C" + l_MessageName + "\n")
    if "comment" in l_Message:
      l_Header.write("     * @brief " + l_Message["comment"] + "\n")
    
    l_Header.write("     */\n")
    l_Header.write("    class C" + l_MessageName + " : public IMessage {\n")
    l_Header.write("      private:\n")
    
    for l_Field in l_Message["fields"]:
      l_Header.write("        " + l_Field["type"] + " m_" + l_Field["name"] + ";  //*< " + l_Field["comment"] + "\n")
    
    l_Header.write("\n")
    l_Header.write("      public:\n")
    
    l_Header.write("        C" + l_MessageName + "(");
    
    l_First = True
    
    for l_Field in l_Message["fields"]:
      if l_First:
        l_First = False
      else:
        l_Header.write(", ")
      
      l_Header.write(CreateParameter(l_Field, a_Static) + "a_" + l_Field["name"])
    
    
    l_Header.write(");\n")
    l_Header.write("        C" + l_MessageName + "(ISerializer *a_pSerializer);\n")
    l_Header.write("        C" + l_MessageName + "(C" + l_MessageName + " *a_pOther);\n\n")
    l_Header.write("        virtual ~C" + l_MessageName + "();\n\n")
    
    l_Header.write("        // Getters\n")
    
    for l_Field in l_Message["fields"]:
      l_Header.write("        " + CreateParameter(l_Field, a_Static) + "get" + l_Field["name"] + "();\n")
    
    l_Header.write("\n")
    l_Header.write("        // Methods inherited from dustbin::messages::IMessage\n")
    l_Header.write("        virtual enMessageIDs getMessageId();\n")
    l_Header.write("        virtual IMessage *clone();\n")
    l_Header.write("        virtual void serialize(ISerializer *a_pSerializer);\n")
    
    l_Header.write("    };\n\n")
    
    l_Source.write("    // Implementation of \"C" + l_MessageName + "\"\n")
    l_Source.write("    C" + l_MessageName + "::C" + l_MessageName + "(")
    
    l_First = True
    
    for l_Field in l_Message["fields"]:
      if l_First:
        l_First = False
      else:
        l_Source.write(", ")
        
      l_Source.write(CreateParameter(l_Field, a_Static) + "a_" + l_Field["name"])
    
    l_Source.write(") {\n")
    
    for l_Field in l_Message["fields"]:
      l_Source.write("      m_" + l_Field["name"] + " = a_" + l_Field["name"] + ";\n")
      
    l_Source.write("    }\n\n")
    
    l_Source.write("    C" + l_MessageName + "::C" + l_MessageName + "(ISerializer *a_pSerializer) {\n")
    
    for l_Field in l_Message["fields"]:
      l_Source.write("      m_" + l_Field["name"] + " = a_pSerializer->");
      
      if l_Field["type"] == "irr::core::vector3df":
        l_Source.write("getVector3df")
      elif l_Field["type"] == "std::string":
        l_Source.write("getString")
      elif "irr::" in l_Field["type"]:
        l_Source.write("get" + l_Field["type"][5:].upper())
      
      l_Source.write("();\n")
    
    
    l_Source.write("    }\n\n")
    l_Source.write("    C" + l_MessageName + "::C" + l_MessageName + "(C" + l_MessageName + " *a_pOther) {\n")
    
    for l_Field in l_Message["fields"]:
      l_Source.write("      m_" + l_Field["name"] + " = a_pOther->get" + l_Field["name"] + "();\n")
    
    l_Source.write("    }\n\n")
    
    for l_Field in l_Message["fields"]:
      l_Source.write("    " + CreateParameter(l_Field, a_Static) + "C" + l_MessageName + "::get" + l_Field["name"] + "() {\n")
      l_Source.write("      return m_" + l_Field["name"] + ";\n")
      l_Source.write("    }\n\n")
      
    l_Source.write("    C" + l_MessageName + "::serialize(ISerializer *a_pSerializer) {\n")
    l_Source.write("      a_pSerializer->addU16((irr::u16)enMessageIDs::" + l_MessageName + ");\n\n")
      
    for l_Field in l_Message["fields"]:
      l_Source.write("      a_pSerializer->")
      if l_Field["type"] == "irr::core::vector3df":
        l_Source.write("addVector3df")
      elif l_Field["type"] == "std::string":
        l_Source.write("addString")
      elif "irr::" in l_Field["type"]:
        l_Source.write("add" + l_Field["type"][5:].upper())
          
      l_Source.write("(m_" + l_Field["name"] + ");\n")
      
    l_Source.write("    }\n\n")
    
    l_Source.write("    IMessage *C" + l_MessageName + "::clone() {\n")
    l_Source.write("      return new C" + l_MessageName + "(this);\n")
    l_Source.write("    }\n\n")
    
    l_Source.write("    enMessageIDs C" + l_MessageName + "::getMessageId() {\n")
    l_Source.write("      return enMessageIDs::" + l_MessageName + ";\n")
    l_Source.write("    }\n\n")
    
  
  EndNamepsaces(l_Header)
  EndNamepsaces(l_Source)
  
  l_Header.close()
  l_Source.close()


def CreateFactory(a_Data):
  global g_HeaderFolder
  global g_SourceFolder
  
  l_Header = open(g_HeaderFolder + "/CMessageFactory.h"  , "w")
  l_Source = open(g_SourceFolder + "/CMessageFactory.cpp", "w")
  
  WriteFileHeader(l_Header)
  WriteFileHeader(l_Source)
  
  l_Header.write("#pragma once\n\n")
  l_Header.write("#include <" + g_Include + "CMessages.h>\n")
  l_Header.write("\n")
  
  l_Source.write("#include <" + g_Include + "CMessageFactory.h>\n\n")
  
  StartNamespace(l_Header)
  StartNamespace(l_Source)
  
  l_Header.write("    class ISerializer;\n")
  l_Header.write("    class IMessage;\n\n")
  
  l_Header.write("    class CMessageFactory {\n")
  l_Header.write("      public:\n")
  l_Header.write("        static IMessage *createMessage(ISerializer *a_pSerializer);\n")
  l_Header.write("    };\n")
  
  l_Source.write("    IMessage *CMessageFactory::createMessage(ISerializer *a_pSerializer) {\n")
  l_Source.write("      switch (a_pSerializer->getMessageType()) {\n")
  
  for l_MessageName in a_Data:
    l_Source.write("        case (irr::u16)enMessageIDs::" + l_MessageName + ": return new C" + l_MessageName + "(a_pSerializer); break;\n")
  
  l_Source.write("      }\n")
  l_Source.write("      return nullptr;\n")
  l_Source.write("    }\n")
  
  EndNamepsaces(l_Header)
  EndNamepsaces(l_Source)


print ""
print "Generating messages ..."
print ""

print "Load message definitions ..."
l_Json = json.load(open("messages.json"))

g_HeaderFolder = l_Json["output"]["header"]
g_SourceFolder = l_Json["output"]["source"]
g_Include      = l_Json["output"]["include"]

if not os.path.exists(g_HeaderFolder):
  print "Creating folder \"" + g_HeaderFolder + "\""
  os.makedirs(g_HeaderFolder)
else:
  print "Path \"" + g_HeaderFolder + "\" exists"

if not os.path.exists(g_SourceFolder):
  print "Creating folder \"" + g_SourceFolder + "\""
  os.makedirs(g_SourceFolder)
else:
  print "Path \"" + g_SourceFolder + "\" exists"
  
CreateEnums(l_Json["messageids"])
CreateMessages(l_Json["messages"], l_Json["static_types"])
CreateFactory(l_Json["messages"])
#CreateInterfaces(l_Json["interfaces"], l_Json["messages"])

print ""
print "Ready."