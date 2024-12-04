// (w) 2024 by Dustbin::Games / Christian Keimel
// This file is licensed under the terms of the ZLib license
// https://www.zlib.net/zlib_license.html
#pragma once

#include <irrlicht.h>

#include <commands/ICommand.h>

#include <vector>

namespace dustbin {
  namespace commands {
    /**
    * @class CCommandHandler
    * @author Christian Keimel
    * This class manages commands
    */
    class CCommandHandler : public irr::IEventReceiver {
      protected:
        std::vector<ICommand *> m_vCommands;  /**< The active commands */

      public:
        CCommandHandler();
        virtual ~CCommandHandler();

        /**
        * Add a command to the vector of active commands
        * Please note that if a command with the new id
        * is already active it will be replaced
        * @param a_pCommand the command to be registered.
        */
        void addCommand(dustbin::commands::ICommand *a_pCommand);

        /**
        * Get a command by it's id
        * @param a_sCmdId the ID of the command
        * @return the command with the id, nullptr no command was found
        */
        ICommand *findCommand(const std::string& a_sCmdId);

        /**
        * Call on every frame. It will call the "run" method
        * of all active commands. Finished commands will
        * be removed from the list and deleted
        * @see ICommand::run
        */
        void run();

        /**
        * The Irrlicht event callback. The event will be passed
        * to the active commands top down (i.e. the command which
        * was last registered gets the callback first). Processing
        * the command is stopped with the first command handling the
        * event (i.e. it returns "true")
        * @param a_cEvent the event
        * @return true if the command was handled
        */
        virtual bool OnEvent(const irr::SEvent& a_cEvent) override;
    };
  }
}

