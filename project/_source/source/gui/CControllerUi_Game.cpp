// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel

#include <controller/CControllerGame.h>
#include <gui/CControllerUi_Game.h>
#include <helpers/CDataHelpers.h>

namespace dustbin {
  namespace gui {
    CControllerUi_Game::CControllerUi_Game(irr::gui::IGUIElement* a_pParent) : CControllerUi(a_pParent, (irr::gui::EGUI_ELEMENT_TYPE)g_ControllerUiGameId) {
    }

    CControllerUi_Game::~CControllerUi_Game() {
    }


    /**
    * Set the controller which is to be configured
    * @param a_sCtrl the controller string the controller is constructed from
    */
    void CControllerUi_Game::setController(const std::string& a_sCtrl) {
      CControllerUi::setController(a_sCtrl);

      controller::CControllerBase l_pCtrl = controller::CControllerGame();

      if (!m_pController->compareTo(&l_pCtrl)) {
        m_pController->getInputs().clear();
        m_pController->deserialize(l_pCtrl.serialize());
      }

      if (m_pController->getInputs().size() > 0) {
        if (m_pController->getInputs()[0].m_eType == controller::CControllerBase::enInputType::Key)
          m_eCtrl = enControl::Keyboard;
        else
          m_eCtrl = enControl::Joystick;
      }

      calculateGui();
    }

    /**
    * Fill the controller from a serialized string. If the vector of controllers is empty
    * it will be filled, otherwise the corresponding items will be updated
    * @param a_sData the serialized string to load the data from
    */
    void CControllerUi_Game::deserialize(const std::string a_sData) {
      std::string l_sData = a_sData;
      if (l_sData == "" || l_sData.substr(0, std::string("DustbinController").size()) != "DustbinController") {
#ifdef _ANDROID
        l_sData = "DustbinController;control;f%3bl%3bForward%3bh%3bb%3bn%3bM%3br%3ba%3bt%3ba%3bx%3bc%3bD%3b-b;control;f%3bl%3bBackward%3bh%3bb%3bn%3bO%3br%3ba%3bt%3ba%3bx%3bc%3bD%3bb;control;f%3bl%3bLeft%3bh%3bb%3bn%3bL%3br%3ba%3bt%3ba%3bx%3ba%3bD%3b-b;control;f%3bl%3bRight%3bh%3bb%3bn%3bN%3br%3ba%3bt%3ba%3bx%3ba%3bD%3bb;control;f%3bl%3bBrake%3bh%3bc%3bn%3bG%3br%3ba%3bt%3ba%3bx%3ba%3bD%3bb;control;f%3bl%3bRearview%3bh%3bc%3bn%3bj%3br%3ba%3bt%3be%3bx%3ba%3bD%3bb;control;f%3bl%3bRespawn%3bh%3bc%3bn%3bn%3br%3ba%3bt%3bf%3bx%3ba%3bD%3bb;control;f%3bl%3bPause%3bh%3bc%3bn%3bt%3br%3ba%3bt%3bg%3bx%3ba%3bD%3bb;control;f%3bl%3bCancel%2520Race%3bh%3bc%3bn%3bB%3br%3ba%3bt%3bh%3bx%3ba%3bD%3bb";
#else
        // Default controls for new player
        l_sData = "DustbinController;control;f%3bl%3bForward%3bh%3bb%3bn%3bM%3br%3ba%3bt%3ba%3bx%3bc%3bD%3b-b;control;f%3bl%3bBackward%3bh%3bb%3bn%3bO%3br%3ba%3bt%3ba%3bx%3bc%3bD%3bb;control;f%3bl%3bLeft%3bh%3bb%3bn%3bL%3br%3ba%3bt%3ba%3bx%3ba%3bD%3b-b;control;f%3bl%3bRight%3bh%3bb%3bn%3bN%3br%3ba%3bt%3ba%3bx%3ba%3bD%3bb;control;f%3bl%3bBrake%3bh%3bc%3bn%3bG%3br%3ba%3bt%3ba%3bx%3ba%3bD%3bb;control;f%3bl%3bRearview%3bh%3bc%3bn%3bj%3br%3ba%3bt%3be%3bx%3ba%3bD%3bb;control;f%3bl%3bRespawn%3bh%3bc%3bn%3bn%3br%3ba%3bt%3bf%3bx%3ba%3bD%3bb;control;f%3bl%3bPause%3bh%3bc%3bn%3bt%3br%3ba%3bt%3bh%3bx%3ba%3bD%3bb;control;f%3bl%3bCancel%2520Race%3bh%3bc%3bn%3bB%3br%3ba%3bt%3bg%3bx%3ba%3bD%3bb";
#endif
      }

      if (m_pController != nullptr)
        m_pController->deserialize(l_sData);
    }

    /**
    * The child classes nees to implement this method for
    * creating default controls if the controller type
    * is changed
    * @param a_eCtrl the new controller type
    */
    void CControllerUi_Game::resetToDefaultForMode(enControl a_eCtrl) {
      if (m_pController == nullptr)
        m_pController = new controller::CControllerGame();

      if (a_eCtrl == enControl::Keyboard) {
        m_pController->deserialize(helpers::getDefaultGameCtrl_Keyboard());
      }
      else if (a_eCtrl == enControl::Joystick) {
        m_pController->deserialize(helpers::getDefaultGameCtrl_Gamepad());
      }
    }
  }
}