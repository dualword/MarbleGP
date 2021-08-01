// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

#include <string>
#include <vector>

namespace dustbin {
  namespace messages {
    class CSerializer64;
  }

  namespace controller {

    /**
    * @class CControllerBase
    * @author Christian Keimel
    * This is the base class for the controllers. It holds all of the functionality, the
    * subclasses just need to add control items
    */
    class CControllerBase {
      public:
        /**
        * An enumeration for the type of controllers, in this case
        * only "key" and "joystick" are defined
        */
        enum class enInputType {
          Key,
          JoyAxis,
          JoyButton,
          JoyPov
        };

        /**
        * This struct holds all data necessary to handle input
        */
        struct SCtrlInput {
          std::string    m_sName,       /**< The name of this control */
                         m_sJoystick;   /**< Name of the joystick (if this is a joystick control) */
          enInputType    m_eType;       /**< The type of this input */
          irr::EKEY_CODE m_eKey;        /**< The key to use (necessary for enCtrlType::Key input) */
          irr::u8        m_iJoystick;   /**< The joystick to use for any type of enCtrlType::Joy* input type */
          irr::u32       m_iButton;     /**< The button used for enCtrlType::JoyButton inputs */
          irr::s16       m_iAxis,       /**< The axis used for enCtrlType::JoyAxis inputs */
                         m_iPov;        /**< The POV direction used */
          irr::s8        m_iDirection;  /**< The direction of the axis to use for enCtrlType::JoyAxis inputs */
          irr::f32       m_fValue;      /**< The value of the input (0 .. -1) */

          SCtrlInput();
          SCtrlInput(enInputType a_eType, const std::string& a_sName);
          SCtrlInput(messages::CSerializer64 *a_pSerializer);
          SCtrlInput(const SCtrlInput& a_cOther);

          ~SCtrlInput();

          /**
          * Event handler for this input
          * @param a_cEvent the event to handle
          */
          virtual void update(const irr::SEvent& a_cEvent);

          void serialize(messages::CSerializer64 *a_pSerializer) const;

          void copyFrom(const SCtrlInput& a_cOther);
        };

      protected:
        std::vector<SCtrlInput> m_vControls;
        irr::core::array<irr::SJoystickInfo> m_aJoysticks;

      public:
        CControllerBase();
        virtual ~CControllerBase();

        /**
        * The event will be passed on to all registered controls
        * @param a_cEvent the event to handle
        */
        void update(const irr::SEvent& a_cEvent);

        std::vector<SCtrlInput>& getInputs();

        /**
        * Serialize the settings
        * @return a serialized string with the controller settings
        */
        std::string serialize();

        /**
        * Fill the controller from a serialized string. If the vector of controllers is empty
        * it will be filled, otherwise the corresponding items will be updated
        * @param a_sData the serialized string to load the data from
        */
        void deserialize(const std::string a_sData);

        /**
        * Update a control item with the values of the parameter. The name of the
        * item is used to identify which of the items needs to be updated
        * @param a_cItem the new values for the item.
        */
        void setControlItem(const CControllerBase::SCtrlInput& a_cItem);
     };

  } // namespace controller 
} // namespace dustbin