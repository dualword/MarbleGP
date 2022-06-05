#include <irrlicht.h>
#include <fstream>


#include <controller/ICustomEventReceiver.h>
// #include <paddleboat/paddleboat.h>
#include <sys/system_properties.h>
#include <state/IState.h>
#include <Defines.h>
#include <CGlobal.h>

#include "android_native_app_glue.h"

#include <CMainClass.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "MarbleGP.NativeActivity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "MarbleGP.NativeActivity", __VA_ARGS__))


class CAndroidVirtualKeyboard {

};

class CAndroidMenuEventHandler : public dustbin::controller::ICustomEventReceiver {
  private:
    irr::s32 m_iTouchID;

  public:
    CAndroidMenuEventHandler(irr::IrrlichtDevice* a_pDevice, dustbin::CMainClass *a_pMainClass) : 
      ICustomEventReceiver(a_pDevice), 
      m_iTouchID(-1)
    {
    }

    virtual ~CAndroidMenuEventHandler() {
    }

    virtual bool handleEvent(const irr::SEvent& a_cEvent) {
      if (a_cEvent.EventType == irr::EET_TOUCH_INPUT_EVENT) {
        /*
        For now we fake mouse-events. Touch-events will be handled inside Irrlicht in the future, but until
        that is implemented you can use this workaround to get a GUI which works at least for simple elements like
        buttons. That workaround does ignore multi-touch events - if you need several buttons pressed at the same
        time you have to handle that yourself.
        */
        irr::SEvent a_cFakeMouseEvent;
        a_cFakeMouseEvent.EventType = irr::EET_MOUSE_INPUT_EVENT;
        a_cFakeMouseEvent.MouseInput.X = a_cEvent.TouchInput.X;
        a_cFakeMouseEvent.MouseInput.Y = a_cEvent.TouchInput.Y;
        a_cFakeMouseEvent.MouseInput.Shift = false;
        a_cFakeMouseEvent.MouseInput.Control = false;
        a_cFakeMouseEvent.MouseInput.ButtonStates = 0;
        a_cFakeMouseEvent.MouseInput.Event = irr::EMIE_COUNT;

        switch (a_cEvent.TouchInput.Event) {
          case irr::ETIE_PRESSED_DOWN: {
            // We only work with the first for now.force opengl error
            if (m_iTouchID == -1) {
              a_cFakeMouseEvent.MouseInput.Event = irr::EMIE_LMOUSE_PRESSED_DOWN;
              m_iTouchID = a_cEvent.TouchInput.ID;

              if (m_pDevice) {
                irr::core::position2d<irr::s32> touchPoint(a_cEvent.TouchInput.X, a_cEvent.TouchInput.Y);
              }
            }
            break;
          }
          case irr::ETIE_MOVED:
            if (m_iTouchID == a_cEvent.TouchInput.ID) {
              a_cFakeMouseEvent.MouseInput.Event = irr::EMIE_MOUSE_MOVED;
              a_cFakeMouseEvent.MouseInput.ButtonStates = irr::EMBSM_LEFT;
            }
            break;
          case irr::ETIE_LEFT_UP:
            if (m_iTouchID == a_cEvent.TouchInput.ID) {
              a_cFakeMouseEvent.MouseInput.Event = irr::EMIE_LMOUSE_LEFT_UP;
              m_iTouchID = -1;
            }
            break;
          default:
            break;
        }

        if (a_cFakeMouseEvent.MouseInput.Event != irr::EMIE_COUNT && m_pDevice) {
          m_pDevice->postEventFromUser(a_cFakeMouseEvent);
        }
      }

      return false;
    }
};

int32_t (*g_IrrlichtInputHandler)(struct android_app* app, AInputEvent* event) = nullptr;

struct SJoystickInput {
  irr::SEvent m_cJoypadEvent;

  int32_t m_aAxes[7];

  irr::IrrlichtDevice *m_pDevice;

  SJoystickInput() {
    m_cJoypadEvent.EventType = irr::EET_JOYSTICK_INPUT_EVENT;

    for (int i = 0; i < 18; i++)
      m_cJoypadEvent.JoystickEvent.Axis[i] = 0;

    m_cJoypadEvent.JoystickEvent.ButtonStates = 0;
    m_cJoypadEvent.JoystickEvent.Joystick     = 0;
    m_cJoypadEvent.JoystickEvent.POV          = 65535;

    m_aAxes[0] = AMOTION_EVENT_AXIS_X;
    m_aAxes[1] = AMOTION_EVENT_AXIS_Y;
    m_aAxes[2] = AMOTION_EVENT_AXIS_Z;
    m_aAxes[3] = AMOTION_EVENT_AXIS_RZ;
    m_aAxes[4] = AMOTION_EVENT_AXIS_BRAKE;
    m_aAxes[5] = AMOTION_EVENT_AXIS_GAS;
    m_aAxes[6] = -1;
  }

  bool parseEvent(AInputEvent* a_pEvent) {
    int32_t l_iSource = AInputEvent_getSource(a_pEvent);

    bool l_bRet = false;

    if ((l_iSource & AINPUT_SOURCE_JOYSTICK) == AINPUT_SOURCE_JOYSTICK) {  
      int32_t l_iEvent = AInputEvent_getType(a_pEvent);

      switch (l_iEvent) {
        case AINPUT_EVENT_TYPE_MOTION: {
          float l_fHatX = AMotionEvent_getAxisValue(a_pEvent, AMOTION_EVENT_AXIS_HAT_X, 0);
          float l_fHatY = AMotionEvent_getAxisValue(a_pEvent, AMOTION_EVENT_AXIS_HAT_Y, 0);

          for (int i = 0; m_aAxes[i] != -1; i++) {
            float f = AMotionEvent_getAxisValue(a_pEvent, m_aAxes[i], 0);
            if (abs(f) > 0.5)
              printf("Blub\n");
            m_cJoypadEvent.JoystickEvent.Axis[i] = (irr::s16)(32767.0 * f);
          }

          if (l_fHatY == -1)
            m_cJoypadEvent.JoystickEvent.POV = 0;
          else if (l_fHatY == 1)
            m_cJoypadEvent.JoystickEvent.POV = 18000;
          else if (l_fHatX == -1)
            m_cJoypadEvent.JoystickEvent.POV = 27000;
          else if (l_fHatX == 1)
            m_cJoypadEvent.JoystickEvent.POV = 9000;
          else
            m_cJoypadEvent.JoystickEvent.POV = 65535;

          l_bRet = true;
          break;
        }

        default:
          break;
      }
    }
    else if ((l_iSource & AINPUT_SOURCE_KEYBOARD) == AINPUT_SOURCE_KEYBOARD) {
      int32_t l_iKeyCode = AKeyEvent_getKeyCode(a_pEvent);
      int32_t l_iAction  = AKeyEvent_getAction(a_pEvent);

      int l_iIndex = l_iKeyCode - 96;
      if (l_iIndex >= 0 && l_iIndex < 32) {
        int l_iFlag = 1 << l_iIndex;

        if (l_iAction == 0)
          m_cJoypadEvent.JoystickEvent.ButtonStates |= l_iFlag;
        else
          m_cJoypadEvent.JoystickEvent.ButtonStates &= ~(l_iFlag);

        l_bRet = true;
      }

      // Hacky, but the Irrlicht Android
      // event handler does never return
      // 1 on key events
      if (l_iKeyCode == 4) {
        irr::SEvent l_cEvent;
        l_cEvent.EventType = irr::EET_KEY_INPUT_EVENT;
        l_cEvent.KeyInput.Key = irr::KEY_BACK;
        l_cEvent.KeyInput.Char = l_iKeyCode;
        l_cEvent.KeyInput.Shift = false;
        l_cEvent.KeyInput.PressedDown = l_iAction != 0;

        m_pDevice->postEventFromUser(l_cEvent);

        l_bRet = true;
      }
    }

    if (l_bRet && m_pDevice)
      m_pDevice->postEventFromUser(m_cJoypadEvent);

    return l_bRet;
  }
};

SJoystickInput g_cJoystickInput;

irr::s32 overrideInputReceiever(android_app* a_pApp, AInputEvent* a_pAndroidEvent) {
  if (g_cJoystickInput.parseEvent(a_pAndroidEvent))
    return 1;

  return (*g_IrrlichtInputHandler)(a_pApp, a_pAndroidEvent);
}


void android_main(struct android_app* a_pApp) {
  JNIEnv *l_pJni = nullptr;

  if (0 != a_pApp->activity->vm->AttachCurrentThread(&l_pJni, nullptr)) {
    printf("Oops");
  }

  /*Paddleboat_ErrorCode l_iError = Paddleboat_init(l_pJni, a_pApp->activity->clazz);

  std::string l_sError   = "PADDLEBOAT_NO_ERROR";
  std::string l_sMessage = "Everything fine.";

  if (l_iError != PADDLEBOAT_NO_ERROR) {
    printf("Oops, could not initialize Paddleboat.\n");

    switch (l_iError) {
      case PADDLEBOAT_ERROR_ALREADY_INITIALIZED     : l_sError = "PADDLEBOAT_ERROR_ALREADY_INITIALIZED"     ; l_sMessage = "Paddleboat_init was called a second time without a call to Paddleboat_destroy in between..\n"; break;
      case PADDLEBOAT_ERROR_FEATURE_NOT_SUPPORTED   : l_sError = "PADDLEBOAT_ERROR_FEATURE_NOT_SUPPORTED"   ; l_sMessage = "The feature is not supported by the specified controller.\nExample: Calling Paddleboat_setControllerVibrationData on a controller that does not have the PADDLEBOAT_CONTROLLER_FLAG_VIBRATION bit set in Paddleboat_Controller_Info.controllerFlags..\n"; break;
      case PADDLEBOAT_ERROR_INIT_GCM_FAILURE        : l_sError = "PADDLEBOAT_ERROR_INIT_GCM_FAILURE"        ; l_sMessage = "Paddleboat could not be successfully initialized.\nInstantiation of the GameControllerManager class failed..\n"; break;
      case PADDLEBOAT_ERROR_INVALID_CONTROLLER_INDEX: l_sError = "PADDLEBOAT_ERROR_INVALID_CONTROLLER_INDEX"; l_sMessage = "Invalid controller index specified.\nValid index range is from 0 to PADDLEBOAT_MAX_CONTROLLERS - 1.\n"; break;
      case PADDLEBOAT_ERROR_INVALID_PARAMETER       : l_sError = "PADDLEBOAT_ERROR_INVALID_PARAMETER"       ; l_sMessage = "An invalid parameter was specified.\nThis usually means NULL or nullptr was passed in a parameter that requires a valid pointer..\n"; break;
      case PADDLEBOAT_ERROR_NOT_INITIALIZED         : l_sError = "PADDLEBOAT_ERROR_NOT_INITIALIZED"         ; l_sMessage = "Paddleboat was not successfully initialized.\nEither Paddleboat_init was not called or returned an error..\n"; break;
      case PADDLEBOAT_ERROR_NO_CONTROLLER           : l_sError = "PADDLEBOAT_ERROR_NO_CONTROLLER"           ; l_sMessage = "No controller is connected at the specified controller index..\n"; break;
      case PADDLEBOAT_ERROR_NO_MOUSE                : l_sError = "PADDLEBOAT_ERROR_NO_MOUSE"                ; l_sMessage = "No virtual or physical mouse device is connected.\n"; break;
    }
  }*/

  dustbin::CMainClass *l_pMainClass = nullptr;
  dustbin::state::enState l_eState;

  do {
    std::string l_sSettings = "";

    std::string l_sPath = a_pApp->activity->internalDataPath;
    l_sPath += "MarbleGP_Setup.xml";

    std::ifstream l_cFile(l_sPath.c_str());
    if (l_cFile.is_open()) {
      l_sSettings = std::string(std::istreambuf_iterator<char>(l_cFile), std::istreambuf_iterator<char>());
      l_cFile.close();
    }

    l_pMainClass = new dustbin::CMainClass(l_sSettings, a_pApp);

    char l_sDeviceName[PROP_VALUE_MAX+1];
    __system_property_get("ro.product.model", l_sDeviceName);
    l_pMainClass->setDeviceName(l_sDeviceName);

    // l_pMainClass->setGlobal("message_headline", l_sError);
    // l_pMainClass->setGlobal("label_message"   , l_sMessage);

    irr::SIrrlichtCreationParameters l_cParams;
    l_cParams.DriverType       = irr::video::EDT_OGLES2;
    l_cParams.Fullscreen       = true;
    l_cParams.WindowSize       = irr::core::dimension2du(0, 0);
    l_cParams.Bits             = 24;
    l_cParams.ZBufferBits      = 32;
    l_cParams.PrivateData      = a_pApp;
    l_cParams.OGLES2ShaderPath = "shaders_es/";
    l_cParams.EventReceiver    = l_pMainClass;

    irr::IrrlichtDevice *l_pDevice = irr::createDeviceEx(l_cParams);

    dustbin::controller::ICustomEventReceiver *l_pMenuHandler = new CAndroidMenuEventHandler(l_pDevice, l_pMainClass);

    l_pMainClass->setIrrlichtDevice(l_pDevice);
    l_pMainClass->setCustomEventReceivers(l_pMenuHandler, nullptr);

    g_IrrlichtInputHandler = a_pApp->onInputEvent;
    g_cJoystickInput.m_pDevice = l_pDevice;
    a_pApp->onInputEvent = &overrideInputReceiever;


    do {
      l_eState = l_pMainClass->run();

      irr::SEvent l_cEvent;
      l_cEvent.EventType = irr::EET_USER_EVENT;
      l_cEvent.UserEvent.UserData1 = c_iEventNewFrame;
      l_cEvent.UserEvent.UserData2 = c_iEventNewFrame;
      l_pDevice->postEventFromUser(l_cEvent);
    }
    while (l_eState != dustbin::state::enState::Restart && l_eState != dustbin::state::enState::Quit);

    l_pDevice->closeDevice();
    l_pDevice->drop();

    {
      std::ofstream l_cFile(l_sPath);
      l_cFile << l_pMainClass->getSettings();
      l_cFile.close();
    }

    delete l_pMainClass;
    l_pMainClass = nullptr;
  }
  while (l_eState != dustbin::state::enState::Quit);

  // Paddleboat_destroy(l_pJni);

  std::exit(0);
}
