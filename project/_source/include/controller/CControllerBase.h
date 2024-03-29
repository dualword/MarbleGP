// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>
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
          std::string    m_sName;       /**< The name of this control */
          enInputType    m_eType;       /**< The type of this input */
          irr::EKEY_CODE m_eKey;        /**< The key to use (necessary for enCtrlType::Key input) */
          irr::u8        m_iJoystick;   /**< The joystick to use for any type of enCtrlType::Joy* input type */
          irr::u32       m_iButton;     /**< The button used for enCtrlType::JoyButton inputs */
          irr::s16       m_iAxis;       /**< The axis used for enCtrlType::JoyAxis inputs */
          irr::u16       m_iPov;        /**< The POV direction used */
          irr::s8        m_iDirection;  /**< The direction of the axis to use for enCtrlType::JoyAxis inputs */
          irr::f32       m_fValue;      /**< The value of the input (0 .. -1) */
          bool           m_bError;      /**< Error while deserialiation */

          SCtrlInput();
          SCtrlInput(enInputType a_eType, const std::string& a_sName);
          SCtrlInput(messages::CSerializer64 *a_pSerializer);
          SCtrlInput(const SCtrlInput& a_cOther);

          virtual ~SCtrlInput();

          /**
          * Event handler for this input
          * @param a_cEvent the event to handle
          */
          bool update(const irr::SEvent& a_cEvent);

          void serialize(messages::CSerializer64 *a_pSerializer) const;

          void copyFrom(const SCtrlInput& a_cOther);

          bool valueUpdate(irr::f32 a_fValue);

          std::string toString() const;

          std::wstring getControlString() const;

          /**
          * Compare a control input to another
          * @return true if the control data matches, false otherwise
          */
          bool equals(SCtrlInput *a_pOther);

          bool nextWizardStep(const irr::SEvent &a_cEvent);
        };

      protected:
        std::vector<SCtrlInput> m_vControls;
        bool m_bError;
        bool m_bIsTouchControl;   /**< Is the de-serialized data defining a touch controller (Android only) */

      public:
        CControllerBase();
        virtual ~CControllerBase();

        /**
        * The event will be passed on to all registered controls
        * @param a_cEvent the event to handle
        * @return true if the event was handled
        */
        virtual bool update(const irr::SEvent& a_cEvent);

        std::vector<SCtrlInput>& getInputs();

        /**
        * Serialize the settings
        * @return a serialized string with the controller settings
        */
        virtual std::string serialize();

        /**
        * Fill the controller from a serialized string. If the vector of controllers is empty
        * it will be filled, otherwise the corresponding items will be updated
        * @param a_sData the serialized string to load the data from
        */
        virtual void deserialize(const std::string a_sData);

        /**
        * Update a control item with the values of the parameter. The name of the
        * item is used to identify which of the items needs to be updated
        * @param a_cItem the new values for the item.
        */
        void setControlItem(const CControllerBase::SCtrlInput& a_cItem);

        /**
        * Is this a controller using a joystick?
        * return true if this is a controller using a joystick
        */
        bool usesJoystick();

        /**
        * Reset the joystick indices of the controls
        */
        void resetJoystick();

        /**
        * Set the joystick indices of the controls
        * @param a_iJoystick the new joystick index
        */
        void setJoystickIndices(irr::s32 a_iJoystick);

        /**
        * Check if the joystick is already assigned
        * @return true if the joystick is already assigned, false otherwise
        */
        virtual bool isJoystickAssigned();

        /**
        * Get the index of the joystick assigned to this controller
        * @return the index of the joystick assigned to this controller
        */
        irr::s32 getJoystickIndex();

        /**
        * Was there an error while deseriazlization?
        * @return true if the was an error, false otherwise
        */
        bool hasError();

        /**
        * Compare the items of this controller to another one
        * @param a_pController the other controller
        * @return true if the number of items and the names match, false otherwise
        */
        bool compareTo(CControllerBase *a_pController);

        /**
        * Clear the controls, necessary for the configuration GUI
        */
        void clearControlItems();
     };

  } // namespace controller 
} // namespace dustbin