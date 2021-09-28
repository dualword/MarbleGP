function getDefaultTexture(a_PlayerIndex, a_Class)
  local l_Textures = {
    [ 1] = "numbercolor=000000&numberback=4b64f9&numberborder=4b64f9&ringcolor=3548b7&patterncolor=000000&patternback=4b64f9",
    [ 2] = "numbercolor=000000&numberback=fd5320&numberborder=fd5320&ringcolor=ba3b15&patterncolor=000000&patternback=fd5320",
    [ 3] = "numbercolor=000000&numberback=3aec1e&numberborder=3aec1e&ringcolor=28ae13&patterncolor=000000&patternback=3aec1e",
    [ 4] = "numbercolor=000000&numberback=c0c000&numberborder=c0c000&ringcolor=8d8d00&patterncolor=000000&patternback=c0c000",
    [ 5] = "numbercolor=000000&numberback=ffc0cb&numberborder=ffc0cb&ringcolor=bc8d95&patterncolor=000000&patternback=ffc0cb",
    [ 6] = "numbercolor=ffffff&numberback=6a0dad&numberborder=6a0dad&ringcolor=c1bbcc&patterncolor=ffffff&patternback=6a0dad",
    [ 7] = "numbercolor=000000&numberback=00ffff&numberborder=00ffff&ringcolor=00bcbc&patterncolor=000000&patternback=00ffff",
    [ 8] = "numbercolor=ffffff&numberback=000000&numberborder=000000&ringcolor=bbbbbb&patterncolor=ffffff&patternback=000000",
    [ 9] = "numbercolor=000000&numberback=ffdab9&numberborder=ffdab9&ringcolor=b9906b&patterncolor=000000&patternback=ffdab9",
    [10] = "numbercolor=000000&numberback=87cefa&numberborder=87cefa&ringcolor=8ebfdd&patterncolor=000000&patternback=87cefa",
    [11] = "numbercolor=000000&numberback=daa520&numberborder=daa520&ringcolor=b8860b&patterncolor=000000&patternback=daa520",
    [12] = "numbercolor=000000&numberback=9932cc&numberborder=9932cc&ringcolor=da70d6&patterncolor=000000&patternback=9932cc",
    [13] = "numbercolor=ffffff&numberback=00008b&numberborder=00008b&ringcolor=0000ff&patterncolor=ffffff&patternback=00008b",
    [14] = "numbercolor=000000&numberback=ffdead&numberborder=ffdead&ringcolor=ffefd5&patterncolor=000000&patternback=ffdead",
    [15] = "numbercolor=000000&numberback=20b2aa&numberborder=20b2aa&ringcolor=7fffd4&patterncolor=000000&patternback=20b2aa",
    [16] = "numbercolor=000000&numberback=ffffff&numberborder=ffffff&ringcolor=dddddd&patterncolor=000000&patternback=ffffff"
  }
  
  local l_ClassTexture = "texture_marblegp.png"
  
  if a_Class == 0 then
    l_ClassTexture = "texture_marbles3.png"
  elseif a_Class == 1 then
    l_ClassTexture = "texture_marbles2.png"
  end
  
  return "generate://pattern=" .. l_ClassTexture .. "&" .. l_Textures[a_PlayerIndex]
end
