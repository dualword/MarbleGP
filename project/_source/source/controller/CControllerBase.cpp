// (w) 2021 by Dustbin::Games / Christian Keimel

#include <controller/CControllerBase.h>
#include <messages/CSerializer64.h>
#include <CGlobal.h>

namespace dustbin {
  namespace controller {
    /** Implementation of SCtrlInput */
    CControllerBase::SCtrlInput::SCtrlInput() {
      m_sName      = "";
      m_eType      = enInputType::Key;
      m_eKey       = irr::KEY_SPACE;
      m_iJoystick  = 0;
      m_iButton    = 0;
      m_iAxis      = 0;
      m_iDirection = 1;
      m_fValue     = 0.0f;
    }

    CControllerBase::SCtrlInput::SCtrlInput(enInputType a_eType, const std::string& a_sName) {
      m_eType = a_eType;
      m_sName = a_sName;
    }

    CControllerBase::SCtrlInput::SCtrlInput(messages::CSerializer64* a_pSerializer) {
      std::string l_sType = a_pSerializer->getString();

      m_eType = l_sType == "JoyAxis" ? enInputType::JoyAxis : l_sType == "JoyButton" ? enInputType::JoyButton : l_sType == "JoyPov" ? enInputType::JoyPov : enInputType::Key;

      m_sName      =                 a_pSerializer->getString();
      m_sJoystick  =                 a_pSerializer->getString();
      m_eKey       = (irr::EKEY_CODE)a_pSerializer->getS32();
      m_iJoystick  =                 a_pSerializer->getS32();
      m_iButton    =                 a_pSerializer->getS32();
      m_iAxis      =                 a_pSerializer->getS32();
      m_iPov       =                 a_pSerializer->getS32();
      m_iDirection =                 a_pSerializer->getS32();
    }

    CControllerBase::SCtrlInput::SCtrlInput(const SCtrlInput& a_cOther) {
      copyFrom(a_cOther);
    }

    CControllerBase::SCtrlInput::~SCtrlInput() {
    }

    void CControllerBase::SCtrlInput::copyFrom(const SCtrlInput& a_cOther) {
      m_sName      = a_cOther.m_sName;
      m_eType      = a_cOther.m_eType;
      m_eKey       = a_cOther.m_eKey;
      m_iJoystick  = a_cOther.m_iJoystick;
      m_iButton    = a_cOther.m_iButton;
      m_iAxis      = a_cOther.m_iAxis;
      m_iDirection = a_cOther.m_iDirection;
      m_fValue     = a_cOther.m_fValue;
      m_iPov       = a_cOther.m_iPov;
      m_sJoystick  = a_cOther.m_sJoystick;
    }

    /**
    * Event handler for this input
    * @param a_cEvent the event to handle
    */
    void CControllerBase::SCtrlInput::update(const irr::SEvent& a_cEvent) {
      switch (m_eType) {
        case enInputType::Key: {
          if (a_cEvent.EventType == irr::EET_KEY_INPUT_EVENT) {
            if (a_cEvent.KeyInput.Key == m_eKey) {
              m_fValue = a_cEvent.KeyInput.PressedDown ? 1.0f : 0.0f;
            }
          }
          break;
        }

        case enInputType::JoyAxis:
          if (a_cEvent.EventType == irr::EET_JOYSTICK_INPUT_EVENT) {
            if (a_cEvent.JoystickEvent.Joystick) {
              irr::s16 l_iValue = a_cEvent.JoystickEvent.Axis[m_iAxis];
              if (m_iDirection > 0) {
                if (l_iValue < 0) l_iValue = 0;
                m_fValue = ((irr::f32)l_iValue) / 32000.0f;
                if (m_fValue > 1.0f) m_fValue = 1.0f;
              }
              else {
                if (l_iValue > 0) l_iValue = 0;
                m_fValue = -((irr::f32)l_iValue) / 32000.0f;
                if (m_fValue > 1.0f) m_fValue = 1.0f;
              }
            }
          }
          break;        

        case enInputType::JoyButton:
          if (a_cEvent.EventType == irr::EET_JOYSTICK_INPUT_EVENT) {
            if (a_cEvent.JoystickEvent.Joystick == m_iJoystick) {
              m_fValue = a_cEvent.JoystickEvent.IsButtonPressed(m_iButton) ? 1.0f : 0.0f;
            }
          }
          break;

        case enInputType::JoyPov:
          if (a_cEvent.EventType == irr::EET_JOYSTICK_INPUT_EVENT) {
            if (a_cEvent.JoystickEvent.Joystick == m_iJoystick) {
              if (a_cEvent.JoystickEvent.POV == 65535)
                m_fValue = 0;
              else {
                irr::u16 l_iPov = a_cEvent.JoystickEvent.POV;

                if (l_iPov == m_iPov)
                  m_fValue = 1.0f;
                else
                  m_fValue = 0.0f;
              }
            }
          }
          break;
      }
    }

    void CControllerBase::SCtrlInput::serialize(messages::CSerializer64* a_pSerializer) const {
      a_pSerializer->addString("control");
      a_pSerializer->addString(m_eType == enInputType::JoyAxis ? "JoyAxis" : m_eType == enInputType::JoyButton ? "JoyButton" : m_eType == enInputType::JoyPov ? "JoyPov" : "Key");
      a_pSerializer->addString(m_sName);
      a_pSerializer->addString(m_sJoystick);
      a_pSerializer->addS32((irr::s32)m_eKey);
      a_pSerializer->addS32(m_iJoystick);
      a_pSerializer->addS32(m_iButton);
      a_pSerializer->addS32(m_iAxis);
      a_pSerializer->addS32(m_iPov);
      a_pSerializer->addS32(m_iDirection);
    }


    CControllerBase::CControllerBase() {
      CGlobal::getInstance()->getIrrlichtDevice()->activateJoysticks(m_aJoysticks);
    }

    CControllerBase::~CControllerBase() {
      m_vControls.clear();
    }

    /**
    * The event will be passed on to all registered controls
    * @param a_cEvent the event to handle
    */
    void CControllerBase::update(const irr::SEvent& a_cEvent) {
      for (std::vector<SCtrlInput>::iterator it = m_vControls.begin(); it != m_vControls.end(); it++) {
        (*it).update(a_cEvent);
      }
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
        (*it).serialize(&l_cSerializer);
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
        bool l_bFillVector = m_vControls.size() == 0;

        messages::CSerializer64 l_cSerializer = messages::CSerializer64(a_sData.c_str());

        std::string l_sHead = l_cSerializer.getString();

        if (l_sHead == "DustbinController") {
          while (l_cSerializer.getString() == "control") {
            SCtrlInput l_cCtrl(&l_cSerializer);

            if (l_bFillVector) {
              m_vControls.push_back(l_cCtrl);
            }
            else {
              for (std::vector<SCtrlInput>::iterator it = m_vControls.begin(); it != m_vControls.end(); it++) {
                if ((*it).m_sName == l_cCtrl.m_sName)
                  (*it) = l_cCtrl;
              }
            }
          }
        }
        else {
          std::string s = std::string("Invalid controller id \"") + l_sHead + "\"";
          CGlobal::getInstance()->setGlobal("ERROR_MESSAGE", s);
          CGlobal::getInstance()->setGlobal("ERROR_HEAD", "Error while loading controller");
          throw std::exception();
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

    std::vector<CControllerBase::SCtrlInput>& CControllerBase::getInputs() {
      return m_vControls;
    }
  } // namespace controller 
} // namespace dustbin