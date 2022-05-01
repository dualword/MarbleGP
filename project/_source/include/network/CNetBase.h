// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <_generated/messages/CMessageFactory.h>
#include <threads/CMessageQueue.h>
#include <threads/IThread.h>
#include <irrlicht.h>
#include <enet.h>
#include <vector>

namespace dustbin {
  class CGlobal;  /**< Forward declaration of the global class */

  namespace network {
    /**
    * @class CNetBase
    * @author Christian Keimel
    * The base class for network client and server
    */
    class CNetBase : public threads::IThread {
      protected:
        CGlobal     *m_pGlobal;
        ENetHost    *m_pHost;
        ENetAddress  m_cAddress;

        messages::CMessageFactory l_cFactory;

        /**
        * Handle an event in a subclass
        * @return "true" if the event was handled
        */
        virtual bool OnEvent(ENetEvent *a_cEvent) = 0;

      public:
        CNetBase(CGlobal *a_pGlobal);
        ~CNetBase();

        /**
        * The implementing object must implement this method. It is called when the thread start working
        */
        virtual void execute();
    };
  }
}
