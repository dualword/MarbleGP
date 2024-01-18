#include <menu/datahandlers/CDataHandler_Controls.h>
#include <controller/CControllerGame.h>
#include <helpers/CStringHelpers.h>
#include <helpers/CMenuLoader.h>
#include <CGlobal.h>

namespace dustbin {
  namespace menu {
    CDataHandler_Controls::CDataHandler_Controls(std::vector<data::SPlayerData> *a_vProfiles, const std::vector<std::string> &a_vSelected) :
      IMenuDataHandler(),
      m_vProfiles     (a_vProfiles),
      m_vSelected     (a_vSelected),
      m_iJoystick     (255),
      m_iBtnState     (0)
    {
      gameclasses::STournament *l_pTournament = CGlobal::getInstance()->getTournament();
      for (auto l_sPlayer : m_vSelected) {
        for (auto &l_cProfile : *a_vProfiles) {
          if (l_cProfile.m_sName == l_sPlayer) {
            controller::CControllerGame l_cCtrl;
            l_cCtrl.deserialize(l_cProfile.m_sControls);

            if (l_cCtrl.usesJoystick()) {
              printf("Player %s uses a gamepad.\n", l_cProfile.m_sName.c_str());
              m_vUnassigned.push_back(l_cProfile.m_sName);
            }
          }
        }
      }

      updateLabel(false);
    }

    CDataHandler_Controls::~CDataHandler_Controls() {
    }

    void CDataHandler_Controls::updateLabel(bool a_bOther) {
      if (m_vUnassigned.size() > 0) {
        irr::gui::IGUIStaticText *l_pLabel = reinterpret_cast<irr::gui::IGUIStaticText *>(helpers::findElementByNameAndType("selectctrl_player", irr::gui::EGUIET_STATIC_TEXT, m_pGui->getRootGUIElement()));
        if (l_pLabel != nullptr) {
          std::wstring l_sText = L"Player \"" + helpers::s2ws(*m_vUnassigned.begin()) + L"\": Select your gamepad by pressing a button.";

          if (a_bOther)
            l_sText += L"\nThis controller is already in use, please choose other.";

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
      bool l_bOther = false;
      bool l_bRet   = false;

      if (a_cEvent.EventType == irr::EET_JOYSTICK_INPUT_EVENT) {
        l_bRet = true;

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
            printf("Button state of joystick %i has changed again.\n", m_iJoystick);
            for (auto& l_cPlayer : *m_vProfiles) {
              if (l_cPlayer.m_sName == *m_vUnassigned.begin()) {
                printf("Player %s found.\n", l_cPlayer.m_sName.c_str());
                if (std::find(m_vAssigned.begin(), m_vAssigned.end(), m_iJoystick) == m_vAssigned.end()) {
                  controller::CControllerGame l_cCtrl;
                  l_cCtrl.deserialize(l_cPlayer.m_sControls);
                  if (l_cCtrl.usesJoystick()) {
                    printf("Assign joystick %i to player \"%s\"\n", m_iJoystick, (*m_vUnassigned.begin()).c_str());
                    l_cCtrl.setJoystickIndices(m_iJoystick);
                  }
                  l_cPlayer.m_sControls = l_cCtrl.serialize();
                  m_vUnassigned.erase(m_vUnassigned.begin());
                  m_vAssigned.push_back(m_iJoystick);
                  printf("%i profiles not assigned.\n", (int)m_vUnassigned.size());
                }
                else {
                  l_bOther = true;
                }
                break;
              }
            }

            printf("Proceed to next check.\n");
            m_iJoystick = 255;
            m_iBtnState = 0;

            if (m_vUnassigned.size() > 0)
              updateLabel(l_bOther);
          }
        }
      }

      return l_bRet;
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