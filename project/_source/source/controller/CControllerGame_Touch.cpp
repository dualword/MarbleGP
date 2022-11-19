// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <controller/CControllerGame_Touch.h>
#include <CGlobal.h>

namespace dustbin {
  namespace controller {
    CControllerGame_Touch::CControllerGame_Touch(IControllerGame::enType a_eType) : irr::gui::IGUIElement(
        (irr::gui::EGUI_ELEMENT_TYPE)g_TouchControlId,
        CGlobal::getInstance()->getGuiEnvironment(), 
        CGlobal::getInstance()->getGuiEnvironment()->getRootGUIElement(), 
        -1, 
        irr::core::recti(irr::core::position2di(0, 0), CGlobal::getInstance()->getVideoDriver()->getScreenSize())
      ),
      IControllerGame   (a_eType),
      m_fSteer          (0.0f),
      m_fThrottle       (0.0f),
      m_bBrake          (false),
      m_bRear           (false),
      m_bRespawn        (false),
      m_bWithdraw       (false),
      m_bPause          (false),
      m_bWithdrawPressed(false),
      m_bPausePressed   (false)
    {
    }

    CControllerGame_Touch::~CControllerGame_Touch() {
    }

    /**
    * Returns the throttle control (-1 .. 1): forward - backward
    * @return the throttle state
    */
    irr::f32 CControllerGame_Touch::getThrottle() {
      return m_fThrottle;
    }

    /**
    * Returns the steer (-1 .. 1): right - left
    * @return the steer state
    */
    irr::f32 CControllerGame_Touch::getSteer() {
      return m_fSteer;
    }

    /**
    * Returns the state of the brake control
    * @return true if control >= 0.5, false otherwise
    */
    bool CControllerGame_Touch::getBrake() {
      return m_bBrake;
    }

    /**
    * Return the rearview control
    * @return true if control >= 0.5, false otherwise
    */
    bool CControllerGame_Touch::getRearView() {
      return m_bRear;
    }

    /**
    * Get the respawn control state
    * @return true if control >= 0.5, false otherwise
    */
    bool CControllerGame_Touch::getRespawn() {
      return m_bRespawn;
    }

    /**
    * Get the "withdraw from race" button state
    * @return true if the button was pressed since the last query, false otherwise
    */
    bool CControllerGame_Touch::withdrawFromRace() {
      bool b = m_bWithdraw;
      m_bWithdraw = false;
      return b;
    }

    /**
    * Get the pause button state
    * @return true if the button was pressed since the last query, false otherwise
    */
    bool CControllerGame_Touch::pause() {
      bool b = m_bPause;
      m_bPause = false;
      return b;
    }

    /**
    * Process Irrlicht evnts to update the controls
    * @param a_cEvent event to process
    */
    void CControllerGame_Touch::updateControls(const irr::SEvent &a_cEvent) {
    }    


    /**
    * Implementation of the serialization method which does nothing in this case
    */
    void CControllerGame_Touch::serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const  {
    }

    /**
    * Implementation of the deserialization method which does nothing in this case
    */
    void CControllerGame_Touch::deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) {
    }

    /**
    * handle Irrlicht events
    * @param a_cEvent the Irrlicht event to handle
    */
    bool CControllerGame_Touch::OnEvent(const irr::SEvent &a_cEvent)  {
      bool l_bRet = false;

      return l_bRet;
    }

    void CControllerGame_Touch::draw()  {
    }
  }
}