#pragma once

namespace dustbin {
  #define c_iEventSettingsChanged 1
  #define c_iEventImagePosChanged 2
  #define c_iEventImageSelected 3
  #define c_iEventNewFrame 4
  #define c_iEventMoveMouse 5
  #define c_iEventKeyboardDestroyed 6
  #define c_iEventMouseClicked 7
  #define c_iEventHideCursor 8
  #define c_iEventOkClicked 9
  #define c_iEventCancelClicked 10
  #define c_iHighlightControlOn 11
  #define c_iHighlightControlOff 12

  // Some constants for profile serialization
  #define c_iProfileHead 42
  #define c_iProfileStart 43
  #define c_iProfileEnd 44
  #define c_iAllProfileEnd 45

  #define c_sProfileHead "PlayerProfiles"

  enum class enTouchCtrlType {
    Gamepad       = 0,
    Gyroscope     = 1,
    SteerCenter   = 2,
    SteerLeft     = 3,
    SteerRight    = 4,
    ControlCenter = 5,
    ControlLeft   = 6,
    ControlRight  = 7
  };

  enum class enControlHiLight {
    Steer   = 0,
    Brake   = 1,
    Respawn = 2,
    Pause   = 3,
    Gyro    = 4,

    Count   = 5
  };
}
