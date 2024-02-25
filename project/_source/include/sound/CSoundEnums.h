// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#pragma once

// An enumeration for the soundtracks
enum class enSoundTrack {
  enStMenu   = 0,
  enStRace   = 1,
  enStFinish = 2,
  enStNone   = 3
};

// An enumeration for the continuus marble sounds
enum class enMarbleSounds {
  Rolling    = 0,
  Wind       = 1,
  Skid       = 2,
  Stunned    = 3,
  Count      = 4
};

// An enumeration for the one-shot sounds
enum class enOneShots {
  RespawnStart = 0,
  RespawnDone  = 1,
  Hit          = 2,
  Checkpoint   = 3,
  Lap          = 4,
  GameOver     = 5,
  Count        = 6
};

// An enumeration for the 2d sounds
enum class en2dSounds {
  ButtonHover = 0,
  ButtonPress = 1,
  Countdown   = 2,
  CountdownGo = 3,
  Lap         = 4,
  Count       = 5
};
