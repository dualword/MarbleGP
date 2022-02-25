#pragma once

namespace dustbin {
  #define c_iEventSettingsChanged 1
  #define c_iEventChangeZLayer 2
  #define c_iEventImagePosChanged 3
  #define c_iEventImageSelected 4

  // Some constants for profile serialization
  #define c_iProfileHead 42
  #define c_iProfileStart 43
  #define c_iProfileEnd 44
  #define c_iAllProfileEnd 45

  #define c_sProfileHead "PlayerProfiles"
}
