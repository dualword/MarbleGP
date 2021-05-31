system:executeluascript("data/lua/helpers_main.lua")
system:executeluascript("data/lua/serializer.lua")

g_SpinBoxes = {
  laps = {
    key = "lapcount",
    plus = "button_laps_next",
    minus = "button_laps_prev",
    label = {
      name = "label_laps_ui",
      node = nil
    },
    options = {
      [1] = "1",
      [2] = "2",
      [3] = "3",
      [4] = "4",
      [5] = "5",
      [6] = "6",
      [7] = "7",
      [8] = "8",
      [9] = "9",
      [10] = "10"
    }
  }
}

g_Settings = {
  lapcount = 3,
  track = "jump_long"
}

g_TrackNo    = 1
g_TrackStart = 0

g_Tracks = {
  [1] = {
    folder = "rookie_oval",
    position = 0,
    name = "Noob: Oval",
    info = "A nice track to start your MarbleGP career with. An easy to master oval with fences that prevent you from falling down."
  },
  [2] = {
    folder = "rookie_eight",
    position = 1,
    name = "Noob: Eight",
    info = "This is an eight-shaped track for newbies. It has fences all over the place to make sure noone falls off the track. It's not an easy track to win though."
  },
  [3] = {
    folder = "rookie_loop",
    position = 2,
    name = "Noob: Loop",
    info = "This is another track for newbies, this time introducing loops. There are two loops on this track and fences to prevent falling off the track."
  },
  [4] = {
    folder = "rookie_complex",
    position = 3,
    name = "Noob: Complex",
    info = "On this track noobs can learn how to best master a complex track like the tracks you will enounter later, although fences around the track will prevent you from falling down on this one."
  },
  [5] = {
    folder = "oval",
    position = 4,
    name = "Flat: Oval",
    info = "A very simple track to get used to controlling the marble, yet it's a challange to get a good laptime on this track."
  },
  [6] = {
    folder = "counter_traffic",
    position = 5,
    name = "Flat: Counter Traffic",
    info = "On this small track you'll most likely encounter counter traffic so watch out - a collision may cost you a lot of time."
  },
  [6] = {
    folder = "eight_crossing",
    position = 6,
    name = "Flat: Eight Crossing",
    info = "An Eight style track with crossroads. Will we see a lot of crashes here?"
  },
  [7] = {
    folder = "steep_circle",
    position = 7,
    name = "Steep: Circle",
    info = "Tiny circle-shaped track with steep corners. Just turn right and you're done."
  },
  [8] = {
    folder = "steep_oval",
    position = 8,
    name = "Steep: Oval ",
    info = "Turn right all the way. Fast track with good option to use slipstreams."
  },
  [9] = {
    folder = "eight_steep",
    position = 9,
    name = "Steep: Eight",
    info = "A Very fast track if you know how to master it. Try not to fall off the track in the corners, and try to avoid crashing other marbles in the crossing."
  },
  [10] = {
    folder = "loop",
    position = 10,
    name = "Loop: Single",
    info = "Another track with the shape of an \"8\". The loop is a hard obstacle."
  },
  [11] = {
    folder = "doubleloop",
    position = 11,
    name = "Loop: Double",
    info = "On this track you'll encounter two entangled loops. But don't underestimate the turns. They hold some suprise for you."
  },
  [12] = {
    folder = "loop_double_half",
    position = 12,
    name = "Loop: Double-Half",
    info = "On this track you'll encounter two half loops connecting two levels. Each of the levels just has a U-turn."
  },
  [13] = {
    folder = "stepstones",
    position = 13,
    name = "Jump: Stepstones",
    info = "Lots of jumps on this track. How many attempts will you need to find out the correct speed to pass?"
  },
  [14] = {
    folder = "jump_long",
    position = 14,
    name = "Jump: Long Jump",
    info = "On this track you'll face a big ramp - find out the right speed so that you neither hit the obstacle nor jump off the track after the landing zone."
  },
  [15] = {
    folder = "jump_hi",
    position = 15,
    name = "Jump: Hi Jump",
    info = "This track has the shape of an eight (again), but this time with a hi-jump in the crossing. Not passing it will cost you a lot of time."
  },
  [16] = {
    folder = "eight_jump",
    position = 16,
    name = "Jump: Eight",
    info = "On this eight-shaped track you'll encounter two jumps over the same gap."
  },
  [17] = {
    folder = "small",
    position = 17,
    name = "Complex: Small",
    info = "This track is rather slow and comes with a lot of small corners and two tiny jumps. It still features a complex layout."
  },
  [18] = {
    folder = "eight_hi_lo",
    position = 18,
    name = "Complex: Eight Hi-Lo",
    info = "Two levels, two parts with the style of an 8. If you don't go carefully you'll most likely be stunned by the track."
  },
  [19] = {
    folder = "up_down",
    position = 19,
    name = "Complex: Up-Down",
    info = "Try to master this track without falling down - you'll soon realize that saying so is easier than doing so."
  },
  [20] = {
    folder = "corkscrew",
    position = 20,
    name = "Complex: Corkscrew",
    info = "On this track you find a corkscrew which is actually a laid-down double-loop, which is followed by some complex corners and a decline where you need to avoid crashing into the next checkpoint."
  },
  [21] = {
    folder = "tilted_turns",
    position = 21,
    name = "Complex: Tilted Turns",
    info = "A complex and long track that features 2 tilted U-turns."
  },
  [22] = {
    folder = "tower_ascend",
    position = 22,
    name = "Complex: Tower Ascend",
    info = "This track comes with a hollow tower. Use it's inner walls to get to the top."
  },
  [23] = {
    folder = "dustbin_raceway",
    position = 23,
    name = "Complex: Dustbin::Games circuit",
    info = "Longest and most complex track, this one has it all: steep turns, a loop, a crossing - but no jumps"
  },
  [24] = {
    folder = "rotor",
    position = 24,
    name = "Moving: Rotor",
    info = "On this track you have the choice - go left or go right. But be aware: each of the holes will be blocked from time to time."
  },
  [25] = {
    folder = "the_wall",
    position = 25,
    name = "Moving: the Wall",
    info = "On this track you'll find a wall which consists of parts that you need to swing open to pass. There is also a shortcut on top of the wall."
  },
  [26] = {
    folder = "blocker",
    position = 26,
    name = "Moving: Blocked Roads",
    info = "On this track you have the option to take the left or the right route - and both of them will be blocked from time to time."
  },
  [27] = {
    folder = "long_lap",
    position = 27,
    name = "Moving: Long Lap Penalty",
    info = "This track features a long and short way to go - the short way is blocked for the leading group though."
  },
  [28] = {
    folder = "four_obstacles",
    position = 28,
    name = "Moving: Four Obstacles",
    info = "This track comes with 4 obstacles that either block the road or try to kick you off the road - or both."
  }
}

g_Time = -1

g_TrackNames = { }

function initialize()
  g_Smgr = system:getscenemanager()
  g_Smgr:loadscene("data/menu3d/menu_trackselect.xml")
  g_Root = g_Smgr:getscenenodefromname("root_menutrackselect")
  
  g_Thumbnail = g_Smgr:getguiitemfromname("thumbnail_ui")
  
  g_Camera = g_Smgr:addcamera()
  g_Camera:setposition({ x = 0.0, y = 0.0, z = 30.0 })
  g_Camera:setupvector({ x = 0.0, y = 1.0, z =  0.0 })
  g_Camera:settarget  ({ x = 0.0, y = 0.0, z = 70.0 })
  g_Camera:activate()
  
  g_LabelName = g_Smgr:getguiitemfromname("trackname_ui")
  
  for i = 1, #g_Tracks do
    if g_Tracks[i]["folder"] == g_Settings["track"] then
      g_TrackNo = i
      g_TrackStart = g_TrackNo - 3
      if g_TrackStart < 0 then
        g_TrackStart = 0
      end
      selectTrack()
      break
    end
  end
  
  for i = 1, 5 do
    local l_Item = g_Smgr:getguiitemfromname("trackname_" .. tostring(i))
    table.insert(g_TrackNames, l_Item)
  end
  
  fillTrackList()
  initSpinBox(g_SpinBoxes, g_Settings)
  
  startFadeIn(g_Root)
end

function fillTrackList()
  io.write("g_TrackStart: " .. tostring(g_TrackStart) .. "\n")
  if g_TrackStart < 0 then
    g_TrackStart = 0
  end
  
  if #g_Tracks > 5 and g_TrackStart > #g_Tracks - 5 then
    g_TrackStart = #g_Tracks - 5
  end
  
  for i = 1, 5 do
    local l_Index = i + g_TrackStart
    if l_Index > #g_Tracks then
      g_TrackNames[i]:settext("---")
      g_TrackNames[i]:setbackgroundcolor({ a = 255, r = 192, g = 192, b = 192 })
    else
      g_TrackNames[i]:settext(g_Tracks[l_Index]["name"])
      if l_Index == g_TrackNo then
        g_TrackNames[i]:setbackgroundcolor({ a = 255, r = 192, g = 255, b = 192 })
      else
        g_TrackNames[i]:setbackgroundcolor({ a = 255, r = 192, g = 192, b = 192 })
      end
    end
  end
end

function selectTrack()
  if g_Tracks[g_TrackNo]["info"] ~= nil then
    g_Thumbnail:settext(g_Tracks[g_TrackNo]["info"])
  end
  
  if g_Tracks[g_TrackNo]["name"] ~= nil then
    g_LabelName:settext(g_Tracks[g_TrackNo]["name"])
  else
    g_LabelName:settext("----")
  end
end

function step(a_Time)
  if g_Time == -1 then
    g_Time = a_Time
  end
  
  local l_Time = a_Time - g_Time
  g_Time = a_Time
  processanimation()
end

function cleanup()
  g_Smgr:clear()
end

function uibuttonclicked(a_Id, a_Name)
  if a_Name == "button_cancel" then
    system:pushscript("data/lua/menu_setupgame.lua")
    startFadeOut(g_Root, g_Time, 1)
  elseif a_Name == "button_up" then
    g_TrackStart = g_TrackStart - 1
    fillTrackList()
  elseif a_Name == "button_down" then
    g_TrackStart = g_TrackStart + 1
    fillTrackList()
  else
    if processSpinBoxes(a_Name, g_SpinBoxes, g_Settings) then
      return
    end
    
    for i = 1, 5 do
      if a_Name == "track_" .. tostring(i) then
        local l_Index = g_TrackStart + i
        if l_Index <= #g_Tracks then
          g_TrackNo = l_Index
          fillTrackList()
          selectTrack()
        end
        return
      end
    end
    
    io.write("Button clicked: " .. tostring(a_Name) .. "\n")
  end
end

function uielementhovered(a_Id, a_Name)
  if a_Name == "thumbnail" and g_Thumbnail ~= nil then
    io.write("uielementhovered\n")
    g_Thumbnail:showtext(true)
  end
end

function uielementleft(a_Id, a_Name)
  if a_Name == "thumbnail" and g_Thumbnail ~= nil then
    io.write("uielementleft\n")
    g_Thumbnail:showtext(false)
  end
end
