system:executeluascript("data/lua/serializer.lua")
system:executeluascript("data/lua/spairs.lua")

g_GameSetup = { }
g_Players   = { }
g_Tracks    = { } -- All tracks
g_Filtered  = { } -- The filtered list of tracks
g_Filter    = { }
g_Items     = { }
g_Settings  = { }
g_AddPlayer = -1

function initialize()
  io.write("Setup Race script started.\n")
  
  g_Smgr = system:getscenemanager()
  g_Smgr:loadscene("data/scenes/skybox.xml")
  
  g_Camera = g_Smgr:addcamera()

  g_Camera:setposition({ x = 0.0, y = 0.0, z =  0.0 })
  g_Camera:setupvector({ x = 0.0, y = 1.0, z =  0.0 })
  g_Camera:settarget  ({ x = 0.0, y = 0.0, z = 35.0 })
  g_Camera:activate()
  
  dialog:loaddialog("data/menu/menu_selecttrack.xml")
  dialog:loaddialog("data/menu/button_cancel.xml"   )  -- The "Cancel" button
  
  local l_Settings = system:getsetting("selecttrack")
  
  g_Settings["nolaps"] = 2
  g_Settings["page"  ] = 0
  g_Settings["track" ] = ""
  g_Settings["filter"] = ""
  
  if l_Settings ~= "" then
    local s = "g_Settings = " .. l_Settings
    system:executeluastring(s)
  end
  
  dialog:createui();
  audio:startsoundtrack(0)
  
  local l_Tracks = system:gettracklist()
  g_Tracks = { }
  
  for k,v in spairs(l_Tracks, function(t, a, b)
    return t[a]["position"] < t[b]["position"]
  end) do
    table.insert(g_Tracks, v)
    
    if string.find(v["name"], ":") ~= nil then
      local l_Filter = string.sub(v["name"], 1, string.find(v["name"], ":") - 1)
      
      local l_Add = true
      
      for i = 1, #g_Filter do
        if g_Filter[i] == l_Filter then
          l_Add = false
          break
        end
      end
      
      if l_Add then
        table.insert(g_Filter, l_Filter)
      end
    end
  end
  
  fillItems()
end

function cleanup()
  g_Smgr:clear()
  dialog:clear()
end

function uibuttonclicked(a_Id, a_Name)
  if a_Name == "cancel" then
    system:clearscriptstack()
    system:statechange(1)
  elseif a_Name == "btn_right" then
    g_Settings["page"] = g_Settings["page"] + 8
    updateThumbnails()
    system:setsetting("selecttrack", serializeTable(g_Settings, 2))
  elseif a_Name == "btn_left" then
    g_Settings["page"] = g_Settings["page"] - 8
    updateThumbnails()
    system:setsetting("selecttrack", serializeTable(g_Settings, 2))
  elseif a_Name == "SelectTrack" then
    local l_Item = g_Items["tracks"][a_Id]
    
    if l_Item ~= nil then
      local l_Track = l_Item:getproperty("Track")
      io.write("Track selected: \"" .. l_Track .. "\"\n")
    end
  else
    io.write("Button clicked: \"" .. a_Name .. "\" (" .. tostring(a_Id) .. ")\n")
  end
end

function updateThumbnails()
  local l_Filter = ""
  
  if g_Items["filter"] ~= nil then
    io.write("Filter: " .. tostring(g_Items["filter"]:getselected()) .. " (" .. tostring(g_Items["filter"]:getselecteditem()) .. ")\n")
    if g_Items["filter"]:getselected() ~= 0 then
      l_Filter = g_Items["filter"]:getselecteditem() .. ":"
    end
  end
  
  g_Filtered = { }
  
  for i = 1, #g_Tracks do
    if l_Filter == "" or string.sub(g_Tracks[i]["name"], 1, #l_Filter) == l_Filter then
      table.insert(g_Filtered, g_Tracks[i])
    end
  end
  
  for i = 1, 8 do
    local l_Index = g_Settings["page"] + i
    
    if g_Items["tracks"][i] ~= nil then
      if g_Filtered[l_Index] ~= nil then
        g_Items["tracks"][i]:setproperty("Track", g_Filtered[l_Index]["folder"])
        
        local l_Thumbnail = "data/levels/" .. g_Filtered[l_Index]["folder"] .. "/thumbnail.png"
        
        if system:fileexists(l_Thumbnail) then
          g_Items["tracks"][i]:setproperty("Texture", "file://" .. l_Thumbnail)
        else
          g_Items["tracks"][i]:setproperty("Texture", "data/images/no_image.png")
        end
        
        g_Items["tracks"][i]:settext(g_Filtered[l_Index]["name"])
        g_Items["tracks"][i]:setvisible(true)
      else
        g_Items["tracks"][i]:setvisible(false)
      end
    end
  end
  
  if g_Settings["page"] < 8 then
    g_Items["btn_left"]:setvisible(false)
  else
    g_Items["btn_left"]:setvisible(true)
  end
  
  if g_Settings["page"] >= #g_Filtered - 8 then
    g_Items["btn_right"]:setvisible(false)
  else
    g_Items["btn_right"]:setvisible(true)
  end
end

function fillItems()
  g_Items = { }
  g_Items["tracks"] = { }
  for i = 1, 8 do
    local l_Item = dialog:getitemfromnameandid("SelectTrack", i)
    g_Items["tracks"][i] = l_Item
  end
  
  g_Items["btn_left" ] = dialog:getitemfromname("btn_left"    )
  g_Items["btn_right"] = dialog:getitemfromname("btn_right"   )
  g_Items["nolaps"   ] = dialog:getitemfromname("nolaps"      )
  g_Items["filter"   ] = dialog:getitemfromname("track_filter")
  
  if g_Settings ~= nil then
    if g_Settings["nolaps"] ~= nil then
      g_Items["nolaps"]:setselecteditem(g_Settings["nolaps"])
    end
    
    if g_Settings["filter"] ~= nil then
      g_Items["filter"]:clearitems()
      
      g_Items["filter"]:additem("** All Tracks")
      for i = 1, #g_Filter do
        g_Items["filter"]:additem(g_Filter[i])
      end
      
      g_Items["filter"]:setselecteditem(g_Settings["filter"])
    end
  end
  
  updateThumbnails()
end

function windowresized()
  fillItems()
end

function uivaluechanged(a_Id, a_Name, a_Value)
  if a_Name == "track_filter" then
    g_Settings["filter"] = g_Items["filter"]:getselecteditem()
    g_Settings["page"  ] = 0
    updateThumbnails()
  elseif a_Name == "nolaps" then
    g_Settings["nolaps"] = g_Items["nolaps"]:getselecteditem()
  else
    io.write("Value Changed: " .. a_Name .. " (" .. tostring(a_Id) .. "): " .. tostring(a_Value) .. "\n")
  end
end
