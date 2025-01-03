// (w) 2024 by Dustbin::Games / Christian Keimel
// This file is licensed under the terms of the ZLib license
// https://www.zlib.net/zlib_license.html
#include <commands/CCommandHandler.h>

namespace dustbin {
  namespace commands {
        CCommandHandler::CCommandHandler() {
        }

        CCommandHandler::~CCommandHandler() {
          while (m_vCommands.size() > 0) {
            auto l_itCmd = m_vCommands.begin();
            ICommand* l_pCmd = *l_itCmd;
            m_vCommands.erase(l_itCmd);
            delete l_pCmd;
          }
        }

        /**
        * Add a command to the vector of active commands
        * Please note that if a command with the new id
        * is already active it will be replaced
        * @param a_pCommand the command to be registered.
        */
        void CCommandHandler::addCommand(dustbin::commands::ICommand *a_pCommand) {
          m_vNewCmds.push_back(a_pCommand);
        }

        /**
        * Get a command by it's id
        * @param a_sCmdId the ID of the command
        * @return the command with the id, nullptr no command was found
        */
        ICommand *CCommandHandler::findCommand(const std::string& a_sCmdId) {
          for (auto l_pCmd: m_vCommands) {
            if (l_pCmd->getId() == a_sCmdId) {
              return l_pCmd;
            }
          }

          return nullptr;
        }

        /**
        * Call on every frame. It will call the "run" method
        * of all active commands. Finished commands will
        * be removed from the list and deleted
        * @see ICommand::run
        */
        void CCommandHandler::run() {
          for (auto l_pCmd: m_vNewCmds) {
            bool l_bAdded = false;

            for (size_t i = 0; i < m_vCommands.size(); i++) {
              if (m_vCommands[i]->getId() == l_pCmd->getId()) {
                ICommand* l_pOld = m_vCommands[i];
                m_vCommands[i] = l_pCmd;
                delete l_pOld;
                l_bAdded = true;
              }
            }

            if (!l_bAdded)
              m_vCommands.push_back(l_pCmd);
          }

          m_vNewCmds.clear();

          if (m_vCommands.size() > 0) {
            std::vector<ICommand*> l_vFinished;

            std::vector<ICommand*>::iterator l_itCmd = m_vCommands.end();

            do {
              l_itCmd--;

              (*l_itCmd)->run();

              if ((*l_itCmd)->isFinished())
                l_vFinished.push_back(*l_itCmd);
            } 
            while (l_itCmd != m_vCommands.begin());


            for (auto l_pFinished: l_vFinished) {
              auto l_itCmd = std::find(m_vCommands.begin(), m_vCommands.end(), l_pFinished);

              if (l_itCmd != m_vCommands.end()) {
                ICommand *p = *l_itCmd;
                m_vCommands.erase(l_itCmd);
                delete p;
              }
            }
          }
        }

        /**
        * The Irrlicht event callback. The event will be passed
        * to the active commands top down (i.e. the command which
        * was last registered gets the callback first). Processing
        * the command is stopped with the first command handling the
        * event (i.e. it returns "true")
        * @param a_cEvent the event
        * @return true if the command was handled
        */
        bool CCommandHandler::OnEvent(const irr::SEvent& a_cEvent) {
          if (m_vCommands.size() > 0) {
            std::vector<ICommand*> l_vFinished;

            if (a_cEvent.EventType == irr::EET_GUI_EVENT && a_cEvent.GUIEvent.EventType == irr::gui::EGET_COMBO_BOX_CHANGED)
              printf(".");

            std::vector<ICommand*>::iterator l_itCmd = m_vCommands.end();
            do {
              l_itCmd--;

              if ((*l_itCmd)->OnEvent(a_cEvent)) {
                return true;
              }
            } 
            while (l_itCmd != m_vCommands.begin());
          }

          return false;
        }
  }
}