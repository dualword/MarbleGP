// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <controller/CControllerBase.h>
#include <messages/CSerializer64.h>
#include <helpers/CStringHelpers.h>
#include <CGlobal.h>

namespace dustbin {
  namespace controller {
    const int c_iHead =  5;
    const int c_iType =  7;
    const int c_iName = 11;
    const int c_iKey  = 13;
    const int c_iJoy  = 17;
    const int c_iBtn  = 19;
    const int c_iAxis = 23;
    const int c_iDir  = 29;
    const int c_iPov  = 31;

    /** Implementation of SCtrlInput */
    CControllerBase::SCtrlInput::SCtrlInput() : m_sName(""), m_eType(enInputType::Key), m_eKey(irr::KEY_SPACE), m_iJoystick(0), m_iButton(0), m_iAxis(0), m_iPov(0xFFFF), m_iDirection(1), m_fValue(0.0f), m_bError(false) {
      m_sName      = "";
      m_eType      = enInputType::Key;
      m_eKey       = irr::KEY_SPACE;
      m_iJoystick  = 0;
      m_iButton    = 0;
      m_iAxis      = 0;
      m_iDirection = 1;
      m_fValue     = 0.0f;
    }

    CControllerBase::SCtrlInput::SCtrlInput(enInputType a_eType, const std::string& a_sName) : m_bError(false) {
      m_eType = a_eType;
      m_sName = a_sName;
    }

    CControllerBase::SCtrlInput::SCtrlInput(messages::CSerializer64* a_pSerializer) : m_fValue(0.0f), m_bError(false) {
      int l_iHead = a_pSerializer->getS32();

      if (l_iHead == c_iHead) {
        while (a_pSerializer->hasMoreMessages() && !m_bError) {
          int l_iIdentifier = a_pSerializer->getS32();

          switch (l_iIdentifier) {
            case c_iName: m_sName      =                 a_pSerializer->getString(); break;
            case c_iType: m_eType      = (enInputType   )a_pSerializer->getS32   (); break;
            case c_iKey : m_eKey       = (irr::EKEY_CODE)a_pSerializer->getS32   (); break;
            case c_iJoy : m_iJoystick  =                 a_pSerializer->getS32   (); break;
            case c_iBtn : m_iButton    =                 a_pSerializer->getS32   (); break;
            case c_iAxis: m_iAxis      =                 a_pSerializer->getS32   (); break;
            case c_iDir : m_iDirection =                 a_pSerializer->getS32   (); break;
            case c_iPov : m_iPov       =                 a_pSerializer->getU16   (); break;
            default:
              m_bError = true;
              break;
          }
        }
      }

      printf("constructor: %s\n", toString().c_str());
    }

    CControllerBase::SCtrlInput::SCtrlInput(const SCtrlInput& a_cOther) : m_fValue(0.0f) {
      copyFrom(a_cOther);
    }

    CControllerBase::SCtrlInput::~SCtrlInput() {
    }

    void CControllerBase::SCtrlInput::serialize(messages::CSerializer64* a_pSerializer) const {
      a_pSerializer->addS32(c_iHead);
      a_pSerializer->addS32(c_iName); a_pSerializer->addString(          m_sName     );
      a_pSerializer->addS32(c_iType); a_pSerializer->addS32   ((irr::s32)m_eType     );
      a_pSerializer->addS32(c_iKey ); a_pSerializer->addS32   ((irr::s32)m_eKey      );
      a_pSerializer->addS32(c_iJoy ); a_pSerializer->addS32   (          m_iJoystick );
      a_pSerializer->addS32(c_iBtn ); a_pSerializer->addS32   (          m_iButton   );
      a_pSerializer->addS32(c_iAxis); a_pSerializer->addS32   (          m_iAxis     );
      a_pSerializer->addS32(c_iDir ); a_pSerializer->addS32   (          m_iDirection);
      a_pSerializer->addS32(c_iPov ); a_pSerializer->addU16   (          m_iPov      );

      printf("serialize: % s\n", toString().c_str());
    }

    std::string CControllerBase::SCtrlInput::toString() const {
      std::string l_sRet = "CtrlInput \"" + m_sName + "\": ";

      switch (m_eType) {
        case enInputType::JoyAxis  : l_sRet += "Joy " + std::to_string(m_iJoystick) + " Axis " + std::to_string(m_iAxis) + " Direction " + std::to_string(m_iDirection); break;
        case enInputType::JoyButton: l_sRet += "Joy " + std::to_string(m_iJoystick) + " Button " + std::to_string(m_iButton)                                                ; break;
        case enInputType::JoyPov   : l_sRet += "Joy " + std::to_string(m_iJoystick) + " POV " + std::to_string(m_iPov)                                                      ; break;
        case enInputType::Key      : l_sRet += "Key " + helpers::ws2s(helpers::keyCodeToString(m_eKey))                                                                               ; break;
      }

      return l_sRet;
    }

    std::wstring CControllerBase::SCtrlInput::getControlString() const {
      std::wstring l_sRet = L"";

      switch (m_eType) {
        case enInputType::JoyAxis  : l_sRet += L"Gamepad Axis "   + std::to_wstring(m_iAxis  ) + L" " + (m_iDirection > 0 ? L"+" : L"-"); break;
        case enInputType::JoyButton: l_sRet += L"Gamepad Button " + std::to_wstring(m_iButton)                                          ; break;
        case enInputType::JoyPov   : l_sRet += L"Gamepad POV "    + std::to_wstring(m_iPov   )                                          ; break;
        case enInputType::Key      : l_sRet += L"Key " + helpers::keyCodeToString(m_eKey)                                                                                        ; break;
      }

      return l_sRet;
    }

    void CControllerBase::SCtrlInput::copyFrom(const SCtrlInput& a_cOther) {
      m_sName      = a_cOther.m_sName;
      m_eType      = a_cOther.m_eType;
      m_eKey       = a_cOther.m_eKey;
      m_iJoystick  = a_cOther.m_iJoystick;
      m_iButton    = a_cOther.m_iButton;
      m_iAxis      = a_cOther.m_iAxis;
      m_iDirection = a_cOther.m_iDirection;
      m_iPov       = a_cOther.m_iPov;
    }

    bool CControllerBase::SCtrlInput::valueUpdate(irr::f32 a_fValue) {
      bool b = a_fValue != m_fValue;
      m_fValue = a_fValue;
      return b;
    }

    /**
    * Event handler for this input
    * @param a_cEvent the event to handle
    */
    bool CControllerBase::SCtrlInput::update(const irr::SEvent& a_cEvent) {
      bool l_bRet = false;

      switch (m_eType) {
        case enInputType::Key: {
          if (a_cEvent.EventType == irr::EET_KEY_INPUT_EVENT) {
            if (a_cEvent.KeyInput.Key == m_eKey) {
              l_bRet = valueUpdate(a_cEvent.KeyInput.PressedDown ? 1.0f : 0.0f);
            }
          }
          break;
        }

        case enInputType::JoyAxis:
          if (a_cEvent.EventType == irr::EET_JOYSTICK_INPUT_EVENT) {
            if (a_cEvent.JoystickEvent.Joystick == m_iJoystick) {
              irr::s16 l_iValue = a_cEvent.JoystickEvent.Axis[m_iAxis];
              if (m_iDirection > 0) {
                if (l_iValue < 0) l_iValue = 0;
                irr::f32 l_fValue = ((irr::f32)l_iValue) / 32000.0f;
                if (l_fValue > 1.0f) l_fValue = 1.0f;
                l_bRet = valueUpdate(l_fValue);
              }
              else {
                if (l_iValue > 0) l_iValue = 0;
                irr::f32 l_fValue = -((irr::f32)l_iValue) / 32000.0f;
                if (l_fValue > 1.0f) l_fValue = 1.0f;
                l_bRet = valueUpdate(l_fValue);
              }
            }
          }
          break;        

        case enInputType::JoyButton:
          if (a_cEvent.EventType == irr::EET_JOYSTICK_INPUT_EVENT) {
            if (a_cEvent.JoystickEvent.Joystick == m_iJoystick) {
              l_bRet = valueUpdate(a_cEvent.JoystickEvent.IsButtonPressed(m_iButton) ? 1.0f : 0.0f);
            }
          }
          break;

        case enInputType::JoyPov:
          if (a_cEvent.EventType == irr::EET_JOYSTICK_INPUT_EVENT) {
            if (a_cEvent.JoystickEvent.Joystick == m_iJoystick) {
              if (a_cEvent.JoystickEvent.POV == 65535)
                l_bRet = valueUpdate(0.0f);
              else {
                irr::u16 l_iPov = a_cEvent.JoystickEvent.POV;

                if (l_iPov == m_iPov)
                  l_bRet = valueUpdate(1.0f);
                else
                  l_bRet = valueUpdate(0.0f);
              }
            }
          }
          break;
      }

      return l_bRet;
    }


    CControllerBase::CControllerBase() : m_bError(false), m_bIsTouchControl(false) {
    }

    CControllerBase::~CControllerBase() {
      m_vControls.clear();
    }

    /**
    * The event will be passed on to all registered controls
    * @param a_cEvent the event to handle
    */
    bool CControllerBase::update(const irr::SEvent& a_cEvent) {
      bool l_bRet = false;

      for (std::vector<SCtrlInput>::iterator it = m_vControls.begin(); it != m_vControls.end(); it++) {
        l_bRet |= (*it).update(a_cEvent);
      }

      return l_bRet;
    }

    /**
    * Serialize the settings
    * @return a serialized string with the controller settings
    */
    std::string CControllerBase::serialize() {
      std::string l_sRet = "";
      messages::CSerializer64 l_cSerializer;
      l_cSerializer.addString("DustbinController");

      for (std::vector<SCtrlInput>::iterator it = m_vControls.begin(); it != m_vControls.end(); it++) {
        messages::CSerializer64 l_cCtrl;
        (*it).serialize(&l_cCtrl);
        l_cSerializer.addString("control");
        l_cSerializer.addString(l_cCtrl.getMessageAsString());
      }
      
      l_sRet = std::string((char*)l_cSerializer.getBuffer());
      return l_sRet;
    }

    /**
    * Fill the controller from a serialized string. If the vector of controllers is empty
    * it will be filled, otherwise the corresponding items will be updated
    * @param a_sData the serialized string to load the data from
    */
    void CControllerBase::deserialize(const std::string a_sData) {
      if (a_sData != "") {
        std::string l_sCtrl[] = {
          "DustbinTouchSteerRight",
          "DustbinTouchSteerLeft" ,
          "DustbinTouchSteerOnly" ,
          "DustbinGyroscope"      ,
          ""
        };

        m_bIsTouchControl = false;

        for (int i = 0; l_sCtrl[i] != "" && !m_bIsTouchControl; i++) {
          if (a_sData == l_sCtrl[i]) {
            m_bIsTouchControl = true;
          }
        }

        bool l_bFillVector = m_vControls.size() == 0;

        messages::CSerializer64 l_cSerializer = messages::CSerializer64(a_sData.c_str());

        std::string l_sHead = l_cSerializer.getString();

        if (l_sHead == "DustbinController") {
          while (l_cSerializer.getString() == "control") {
            messages::CSerializer64 l_cCtrlData = messages::CSerializer64(l_cSerializer.getString().c_str());
            SCtrlInput l_cCtrl(&l_cCtrlData);

            m_bError |= l_cCtrl.m_bError;

            if (l_bFillVector) {
              m_vControls.push_back(l_cCtrl);
            }
            else {
              for (std::vector<SCtrlInput>::iterator it = m_vControls.begin(); it != m_vControls.end(); it++) {
                if ((*it).m_sName == l_cCtrl.m_sName) {
                  printf("%s <==> %s\n", (*it).m_sName.c_str(), l_cCtrl.m_sName.c_str());
                  (*it) = l_cCtrl;
                }
              }
            }
          }
        }
        else {
          std::string s = std::string("Invalid controller id \"") + l_sHead + "\"";
          CGlobal::getInstance()->setGlobal("ERROR_MESSAGE", s);
          CGlobal::getInstance()->setGlobal("ERROR_HEAD", "Error while loading controller");
          // throw std::exception();
        }
      }
    }

    /**
    * Update a control item with the values of the parameter. The name of the
    * item is used to identify which of the items needs to be updated
    * @param a_pItem the new values for the item.
    */
    void CControllerBase::setControlItem(const CControllerBase::SCtrlInput& a_cItem) {
      for (std::vector<SCtrlInput>::iterator it = m_vControls.begin(); it != m_vControls.end(); it++) {
        if ((*it).m_sName == a_cItem.m_sName) {
          (*it).copyFrom(a_cItem);
          break;
        }
      }
    }

    /**
    * Is this a controller using a joystick?
    * return true if this is a controller using a joystick
    */
    bool CControllerBase::usesJoystick() {
      if (m_bIsTouchControl)
        return false;

      for (std::vector<SCtrlInput>::iterator it = m_vControls.begin(); it != m_vControls.end(); it++) {
        if ((*it).m_eType == enInputType::JoyAxis || (*it).m_eType == enInputType::JoyButton || (*it).m_eType == enInputType::JoyPov)
          return true;
      }

      return false;
    }

    /**
    * Reset the joystick indices of the controls
    */
    void CControllerBase::resetJoystick() {
      if (usesJoystick()) {
        for (std::vector<SCtrlInput>::iterator it = m_vControls.begin(); it != m_vControls.end(); it++) {
          (*it).m_iJoystick = 255;
        }
      }
    }

    /**
    * Set the joystick indices of the controls
    * @param a_iJoystick the new joystick index
    */
    void CControllerBase::setJoystickIndices(irr::s32 a_iJoystick) {
      if (usesJoystick()) {
        for (std::vector<SCtrlInput>::iterator it = m_vControls.begin(); it != m_vControls.end(); it++) {
          (*it).m_iJoystick = (irr::u8)a_iJoystick;
        }
      }
    }

    /**
    * Check if the joystick is already assigned
    * @return true if the joystick is already assigned, false otherwise
    */
    bool CControllerBase::isJoystickAssigned() {
      if (usesJoystick()) {
        bool l_bRet = true;

        for (std::vector<SCtrlInput>::iterator it = m_vControls.begin(); it != m_vControls.end(); it++) {
          if ((*it).m_iJoystick == 255) {
            l_bRet = false;
            break;
          }
        }

        return l_bRet;
      }
      else return true;
    }

    /**
    * Get the index of the joystick assigned to this controller
    * @return the index of the joystick assigned to this controller
    */
    irr::s32 CControllerBase::getJoystickIndex() {
      if (usesJoystick()) 
        return (*m_vControls.begin()).m_iJoystick;
      else 
        return -1;
    }

    /**
    * Was there an error while deseriazlization?
    * @return true if the was an error, false otherwise
    */
    bool CControllerBase::hasError() {
      return m_bError;
    }

    /**
    * Compare the items of this controller to another one
    * @param a_pController the other controller
    * @return true if the number of items and the names match, false otherwise
    */
    bool CControllerBase::compareTo(CControllerBase* a_pController) {
      if (a_pController->getInputs().size() != m_vControls.size())
        return false;

      for (int i = 0; i < m_vControls.size(); i++)
        if (a_pController->getInputs()[i].m_sName != m_vControls[i].m_sName)
          return false;

      return true;
    }

    std::vector<CControllerBase::SCtrlInput>& CControllerBase::getInputs() {
      return m_vControls;
    }
  } // namespace controller 
} // namespace dustbin