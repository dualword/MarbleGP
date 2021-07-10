// (w) 2021 by Dustbin::Games / Christian Keimel

#include <controller/CControllerBase.h>

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

    CControllerBase::SCtrlInput::SCtrlInput(irr::io::IXMLReaderUTF8* a_pXml) {
      while (a_pXml->read()) {
        std::string l_sName = a_pXml->getNodeName();

        if (l_sName == "attribute") {
          std::string l_sKey = a_pXml->getAttributeValueSafe("key");

          if (l_sKey == "type")
            m_eType = (enInputType)a_pXml->getAttributeValueAsInt("value");
          else if (l_sKey == "name")
            m_sName = a_pXml->getAttributeValueSafe("value");
          else if (l_sKey == "joy_name")
            m_sJoystick = a_pXml->getAttributeValueSafe("value");
          else if (l_sKey == "key")
            m_eKey = (irr::EKEY_CODE)a_pXml->getAttributeValueAsInt("value");
          else if (l_sKey == "joy_index")
            m_iJoystick = a_pXml->getAttributeValueAsInt("value");
          else if (l_sKey == "joy_button")
            m_iButton = a_pXml->getAttributeValueAsInt("value");
          else if (l_sKey == "joy_axis")
            m_iAxis = a_pXml->getAttributeValueAsInt("value");
          else if (l_sKey == "pov")
            m_iPov = a_pXml->getAttributeValueAsInt("value");
          else if (l_sKey == "direction")
            m_iDirection = a_pXml->getAttributeValueAsInt("value");
        }
        else if (l_sName == "control" && a_pXml->getNodeType() == irr::io::EXN_ELEMENT_END)
          break;
      }
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

                if (l_iPov >= m_iPov - 900 || l_iPov <= m_iPov + 900)
                  m_fValue = 1.0f;
                else
                  m_fValue = 0.0f;
              }
            }
          }
          break;
      }
    }

    void CControllerBase::SCtrlInput::serialize(irr::io::IXMLWriterUTF8* a_pXml) const {
      a_pXml->writeElement("control", false);
      a_pXml->writeLineBreak();

      a_pXml->writeElement("attribute", true, "key", "type", "value", m_eType == enInputType::JoyAxis ? "JoyAxis" : m_eType == enInputType::JoyButton ? "JoyButton" : m_eType == enInputType::JoyPov ? "JoyPov" : "Key"); a_pXml->writeLineBreak();
      a_pXml->writeElement("attribute", true, "key", "name"      , "value",                 m_sName     .c_str()                                                                                                       ); a_pXml->writeLineBreak();
      a_pXml->writeElement("attribute", true, "key", "joy_name"  , "value",                 m_sJoystick .c_str()                                                                                                       ); a_pXml->writeLineBreak();
      a_pXml->writeElement("attribute", true, "key", "key"       , "value", std::to_string(m_eKey      ).c_str()                                                                                                       ); a_pXml->writeLineBreak();
      a_pXml->writeElement("attribute", true, "key", "joy_index" , "value", std::to_string(m_iJoystick ).c_str()                                                                                                       ); a_pXml->writeLineBreak();
      a_pXml->writeElement("attribute", true, "key", "joy_button", "value", std::to_string(m_iButton   ).c_str()                                                                                                       ); a_pXml->writeLineBreak();
      a_pXml->writeElement("attribute", true, "key", "joy_axis"  , "value", std::to_string(m_iAxis     ).c_str()                                                                                                       ); a_pXml->writeLineBreak();
      a_pXml->writeElement("attribute", true, "key", "pov"       , "value", std::to_string(m_iPov      ).c_str()                                                                                                       ); a_pXml->writeLineBreak();
      a_pXml->writeElement("attribute", true, "key", "direction" , "value", std::to_string(m_iDirection).c_str()                                                                                                       ); a_pXml->writeLineBreak();
      
      a_pXml->writeClosingTag("control");
      a_pXml->writeLineBreak();
    }


    CControllerBase::CControllerBase() {

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
    * @param a_pXml an Irrlicht XML writer to save the data to
    */
    void CControllerBase::serialize(irr::io::IXMLWriterUTF8* a_pXml) {
      a_pXml->writeXMLHeader();

      a_pXml->writeElement("controller", false);
      a_pXml->writeLineBreak();

      for (std::vector<SCtrlInput>::iterator it = m_vControls.begin(); it != m_vControls.end(); it++)
        (*it).serialize(a_pXml);

      a_pXml->writeClosingTag("controller");
      a_pXml->writeLineBreak();
    }

    /**
    * Fill the controller from an XML file. If the vector of controllers is empty
    * it will be filled, otherwise the corresponding items will be updated
    * @param a_pXml the XML file
    */
    void CControllerBase::deserialize(irr::io::IXMLReaderUTF8* a_pXml) {
      bool l_bFillVector = m_vControls.size() == 0;

      while (a_pXml->read()) {
        std::string l_sName = a_pXml->getNodeName();

        if (l_sName == "control") {
          SCtrlInput l_cCtrl = SCtrlInput(a_pXml);

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