#pragma once

#ifdef _WINDOWS
#include <WinSock2.h>
#include <WS2tcpip.h>
#endif

#include <vector>
#include <string>
#include <map>

namespace dustbin {
  namespace webserver {
    class IWebServerRequest;

    /**
    * @class IWebServer
    * @author Christian Keimel
    * The interface of the web server
    */
    class IWebServer {
      protected:
        /**
        * Create a webserver request object
        * @param a_iSocket the socket to use
        * @return an instance of IWebServerRequest handling the call
        */
        virtual IWebServerRequest *createRequest(
#ifdef _WINDOWS
          SOCKET a_iSocket
#else
          int a_iSocket
#endif
        ) = 0;

      public:
        /**
        * The constructor. Takes the port number to listen to as argument
        * @param a_iPortNo the port number to listen to 
        */
        IWebServer() { }

        virtual ~IWebServer() { }

        /**
        * Get the error code
        * @return the error code (0 == no error)
        */
        virtual int getError() = 0;

        /**
        * Start the server thread. Will return immediately
        */
        virtual void startServer() = 0;

        /**
        * Stop the server. This method will tell the server
        * to stop but return at once
        */
        virtual void stopServer() = 0;

        /**
        * Is the thread running?
        * @return "true" if the server is running, false otherwise
        */
        virtual bool isRunning() = 0;

        /**
        * Join the threat, i.e. wait until it has finished
        */
        virtual void join() = 0;
    };

    /**
    * @class IWebServerRequest
    * @author Christian Keimel
    * This is the interface that handles HTTP requests
    */
    class IWebServerRequest {
      public:
        IWebServerRequest() { }
        virtual ~IWebServerRequest() { }

        /**
        * Execute the request in a thread of it's own
        */
        virtual void execute() = 0;

        /**
        * Has the request finished?
        * @return true if the request has finished, false otherwise
        */
        virtual bool hasFinished() = 0;

        /**
        * Detach from the thread
        */
        virtual void detach() = 0;
    };
  }
}
