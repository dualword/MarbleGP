#include <menu/datahandlers/CDataHandler_Controls.h>
#include <controller/CControllerGame.h>
#include <helpers/CStringHelpers.h>
#include <helpers/CMenuLoader.h>
#include <CGlobal.h>

namespace dustbin {
  namespace menu {
    CDataHandler_Controls::CDataHandler_Controls(std::vector<data::SPlayerData> &a_vProfiles, data::SChampionship *a_pChampionship) :
      IMenuDataHandler(),
      m_vProfiles     (a_vProfiles),
      m_pChampionship (a_pChampionship),
      m_iJoystick     (255),
      m_iBtnState     (0),
      m_pGui          (CGlobal::getInstance()->getGuiEnvironment())
    {
      for (auto l_cPlayer : a_pChampionship->m_vPlayers) {
        for (auto l_cProfile : a_vProfiles) {
          if (l_cProfile.m_iPlayerId == l_cPlayer.m_iPlayerId) {
            controller::CControllerGame l_cCtrl;
            l_cCtrl.deserialize(l_cProfile.m_sControls);

            if (l_cCtrl.usesJoystick()) {
              printf("Player %s uses a gamepad.\n", l_cProfile.m_sName.c_str());
              m_vUnassigned.push_back(l_cProfile.m_sName);
            }
          }
        }
      }

      updateLabel();
    }

    CDataHandler_Controls::~CDataHandler_Controls() {
    }

    void CDataHandler_Controls::updateLabel() {
      if (m_vUnassigned.size() > 0) {
        irr::gui::IGUIStaticText *l_pLabel = reinterpret_cast<irr::gui::IGUIStaticText *>(helpers::findElementByNameAndType("selectctrl_player", irr::gui::EGUIET_STATIC_TEXT, m_pGui->getRootGUIElement()));
        if (l_pLabel != nullptr) {
          std::wstring l_sText = L"Player \"" + helpers::s2ws(*m_vUnassigned.begin()) + L"\": Select your gamepad by pressing a button.";
          l_pLabel->setText(l_sText.c_str());
        }
      }
    }

    /**
    * This is the main method of this class. Irrlicht events
    * get forwarded and may be handled by the handler
    * @param a_cEvent the event to handle
    * @return true if the event was handled, false otherwise
    */
    bool CDataHandler_Controls::handleIrrlichtEvent(const irr::SEvent& a_cEvent) {
      if (a_cEvent.EventType == irr::EET_JOYSTICK_INPUT_EVENT) {
        if (m_iJoystick == 255) {
          if (m_mBtnStates.find(a_cEvent.JoystickEvent.Joystick) == m_mBtnStates.end()) {
            m_mBtnStates[a_cEvent.JoystickEvent.Joystick] = a_cEvent.JoystickEvent.ButtonStates;
          }

          if (m_mBtnStates[a_cEvent.JoystickEvent.Joystick] != a_cEvent.JoystickEvent.ButtonStates) {
            printf("Button state of joystick %i changed: %i\n", a_cEvent.JoystickEvent.Joystick, a_cEvent.JoystickEvent.ButtonStates);
            m_iJoystick = a_cEvent.JoystickEvent.Joystick;
            m_iBtnState = a_cEvent.JoystickEvent.ButtonStates;
          }
        }
        else {
          if (a_cEvent.JoystickEvent.Joystick == m_iJoystick && a_cEvent.JoystickEvent.ButtonStates != m_iBtnState) {
            for (auto& l_cPlayer : m_vProfiles) {
              if (l_cPlayer.m_sName == *m_vUnassigned.begin()) {
                controller::CControllerGame l_cCtrl;
                l_cCtrl.deserialize(l_cPlayer.m_sControls);
                if (l_cCtrl.usesJoystick()) {
                  l_cCtrl.setJoystickIndices(m_iJoystick);
                }
                l_cPlayer.m_sControls = l_cCtrl.serialize();
                m_vUnassigned.erase(m_vUnassigned.begin());
                break;
              }
            }

            printf("Proceed to next check.\n");
            m_iJoystick = 255;
            m_iBtnState = 0;

            if (m_vUnassigned.size() > 0)
              updateLabel();
          }
        }
      }

      return false;
    }

    /**
    * Check whether or not all controllers (gaempads) of the selected players are assigned
    * @return true if all controllers are assigned, false otherwise
    */
    bool CDataHandler_Controls::allControllersAssigned() {
      return m_vUnassigned.size() == 0;
    }
  }
}