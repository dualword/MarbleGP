-- (w) 2021 by Dustbin::Games / Christian Keimel / This file is licensed under the terms of the zlib license: https://opensource.org/licenses/Zlib

function doSerializeTable(a_Table, a_Indent)
  if a_Indent == nil then
    a_Indent = 0
  end
  
  function doIndent(a_Spaces)
    local l_Indent = ""
    for i = 0, a_Spaces - 1 do
      l_Indent = l_Indent .. " "
    end
    return l_Indent
  end
  
  local l_Ret  = ""
  local l_Type = type(a_Table)
  
  if l_Type == "table" then
    l_Ret = l_Ret .. doIndent(a_Indent) .. "{\n"
    for k,v in pairs(a_Table) do
      l_Ret = l_Ret .. doIndent(a_Indent)
      
      if type(k) == "string" then
        l_Ret = l_Ret .. tostring(k) .. "="
      elseif type(k) == "number" then
        l_Ret = l_Ret .. "[" .. tostring(k) .. "]="
      end
      
      l_Ret = l_Ret .. doSerializeTable(v, tonumber(a_Indent) + 2)
    end
    l_Ret = l_Ret .. doIndent(a_Indent) .. "}"
    
    if a_Indent > 0 then
      l_Ret = l_Ret .. ",\n"
    else
      l_Ret = l_Ret .. "\n"
    end
  elseif l_Type == "string" then
    l_Ret = l_Ret .. "\"" .. a_Table .. "\",\n"
  else
    l_Ret = l_Ret .. tostring(a_Table) .. ",\n"
  end
  
  return l_Ret
end

function serializeTable(a_Table, a_Indent)
  local l_Result = doSerializeTable(a_Table, a_Indent)
  
  while string.sub(l_Result, -1) == " " or string.sub(l_Result, -1) == "\n" or string.sub(l_Result, -1) == "," do
    l_Result = l_Result:sub(1, -2)
  end
  
  return l_Result
end