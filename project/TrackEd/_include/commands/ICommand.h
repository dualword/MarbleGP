// (w) 2024 by Dustbin::Games / Christian Keimel
// This file is licensed under the terms of the ZLib license
// https://www.zlib.net/zlib_license.html
#pragma once

#include <irrlicht.h>
#include <string>

namespace dustbin {
  namespace commands {
    /**
    * @class ICommand
    * @author Christian Keimel
    * This is the interface which must be implemented by all commands
    */
    class ICommand : public irr::IEventReceiver {
      private:
        std::string m_sCmdId;   /**< ID of the command */

      public:
        /**
        * The constructor
        * @param a_sCmdId if of this command
        */
        ICommand(const std::string &a_sCmdId) : m_sCmdId(a_sCmdId) {
        }

        /**
        * The destructor
        */
        virtual ~ICommand() {
        }

        /**
        * Get the id of this command
        * @return the id of this command
        */
        const std::string &getId() {
          return m_sCmdId;
        }

        /**
        * If this method returns "true" the command is finished and
        * will be removed from the vector of active commands
        * @return true if the command is finished and can be removed
        */
        virtual bool isFinished() = 0;

        /**
        * This method is called on every frame.
        */
        virtual void run() {
        }

        /**
        * Callback method for changes in the UI like modifying
        * font size or resizin window
        */
        virtual void onUiSetupModified() {
        }

        /**
        * The Irrlicht event handler method. Implement it if the command
        * needs to react to user input
        */
        virtual bool OnEvent(const irr::SEvent &a_cEvent) override {
          return false;
        }

        /**
        * Let this command draw something (if necessary)
        * @param a_pDrv the video driver
        */
        virtual void draw(irr::video::IVideoDriver *a_pDrv) {
        }
    };
  }
}
