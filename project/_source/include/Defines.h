#pragma once

namespace dustbin {
  #define c_iEventSettingsChanged 1
  #define c_iEventChangeZLayer 2
  #define c_iEventImagePosChanged 3
  #define c_iEventImageSelected 4
  #define c_iEventNewFrame 5
  #define c_iEventMoveMouse 6
  #define c_iEventKeyboardDestroyed 7
  #define c_iEventMouseClicked 8

  // Some constants for profile serialization
  #define c_iProfileHead 42
  #define c_iProfileStart 43
  #define c_iProfileEnd 44
  #define c_iAllProfileEnd 45

  #define c_sProfileHead "PlayerProfiles"
}
