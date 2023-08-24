#pragma once

#include <_generated/messages/IWebLogSender.h>
#include <webserver/IWebServer.h>
#include <irrlicht.h>

#ifdef _WINDOWS
#define WIN32_LEAN_AND_MEAN

#include <WinSock2.h>
#include <WS2tcpip.h>
#endif

#include <vector>
#include <string>
#include <thread>
#include <map>

namespace dustbin {
  namespace threads {
    class CInputQueue;
    class COutputQueue;
  }

  namespace webserver {
    /**
    * @class CWebServerBase
    * @author Christian Keimel
    * This is the base class for the web server. The actual implementation must
    * be derived from this class and override the methods for the HTTP methods
    */
    class CWebServerBase : public IWebServer, public messages::IWebLogSender {
      private:
        std::thread m_cThread;      /**< The main thread of the server */
        bool        m_bRunning;     /**< The "running" flag of the server */
        bool        m_bStop;        /**< "Stop" flag */
        int         m_iPortNo;      /**< The port to listen to */
        int         m_iError;       /**< Error code (0 == no error) */
        std::string m_sHostName;    /**< Name of the host */

        threads::COutputQueue *m_pQueue;

#ifdef _WINDOWS
        SOCKET      m_iSocket;  /**< Server ID */
#else
        int         m_iSocket;  /**< Server ID */
#endif

        void execute();

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
        ) override;

      public:
        /**
        * The constructor. Takes the port number to listen to as argument
        * @param a_pQueue the input queue of the remote menu, we add an output queue to this queue
        * @param a_iPortNo the port number to listen to 
        */
        CWebServerBase(int a_iPortNo, threads::CInputQueue *a_pQueue);

        virtual ~CWebServerBase();

        /**
        * Get the error code
        * @return the error code (0 == no error)
        */
        virtual int getError() override;

        /**
        * Start the server thread. Will return immediately
        */
        virtual void startServer() override;

        /**
        * Stop the server. This method will tell the server
        * to stop but return at once
        */
        virtual void stopServer() override;

        /**
        * Is the thread running?
        * @return "true" if the server is running, false otherwise
        */
        virtual bool isRunning() override;

        /**
        * Join the threat, i.e. wait until it has finished
        */
        virtual void join() override;

        /**
        * Get the name of the host
        * @return the name of the host
        */
        std::string hostName();
    };


    /**
    * @class CWebServerRequestBase
    * @author Christian Keimel
    * A base class sending 404s to all requests
    */
    class CWebServerRequestBase : public IWebServerRequest, public messages::IWebLogSender {
      protected:
        std::thread m_cThread;      /**< The main thread of the server */
        bool        m_bFinished;    /**< Is processing this request finished? */
#ifdef _WINDOWS
        SOCKET m_iSocket;           /**< The socket to use*/
#else
        int m_iSocket;
#endif

        threads::COutputQueue *m_pQueue;

        irr::io::IFileSystem *m_pFs;  /**< The Irrlicht file system */

        /**
        * Send data to the client
        * @param a_pData the data to send
        * @param a_iLen the number of bytes to send
        * @return the number of bytes sent
        */
        virtual int sendData(const char *a_pData, int a_iLen);

        /**
        * Send header data
        * @param a_iResult the HTTP result code to send
        * @param a_mHeader string-string map with the header data. The first line will be added by the method
        */
        void sendHeader(int a_iResult, const std::map<std::string, std::string> &a_mHeader);

        /**
        * Get the content type defined by the file extension
        * @param a_sExtension the file extension
        * @return the content type
        */
        std::string getContentType(const std::string &a_sExtension);

        /**
        * Get the string representation of the HTTP status core
        * @param a_iStatus the HTTP status code
        * @return the string representation of the HTTP status core
        */
        std::string getHttpStatusCode(int a_iStatus);

        /**
        * Get the name of the host
        * @return the name of the host
        */
        std::string hostName();

        /**
        * Send a 404
        * @param a_sPath the path of the not-found file
        * @return 404
        */
        int send404(const std::string &a_sPath);

        /**
        * Create the track name JSON
        * @return a string with a JSON representation of the track name map
        */
        std::string createTrackNameJSON();

        /**
        * Get the XML with the results of the last championship
        * @return the XML with the results of the last championship
        */
        std::string getChampionshipData();

        /**
        * Return a string with a JSON representation of the profiles
        * @return a string with a JSON representation of the profiles
        */
        std::string getProfileData();

        /**
        * Get the Base64 representation of an image
        * @param a_sImage path to the image
        * @return the Base64 representation of the image
        */
        std::string getBase64Image(const std::string &a_sImage);

        /**
        * Get a JavaScript snippet filling the texture pattern dictionary
        * @return a JavaScript snippet filling the texture pattern dictionary
        */
        std::string getTexturePatternJS();

      public:
        /**
        * The constructor
        * @param a_pQueue the message queue for the log messages
        * @param a_iSocket socket identifier to use
        */
        CWebServerRequestBase(threads::COutputQueue *a_pQueue,
#ifdef _WINDOWS
          SOCKET a_iSocket
#else
          int a_iSocket
#endif
        );
        virtual ~CWebServerRequestBase();

        /**
        * Execute the request in a thread of it's own
        */
        virtual void execute() override;

        /**
        * Has the request finished?
        * @return true if the request has finished, false otherwise
        */
        virtual bool hasFinished() override;

        /**
        * Handle a GET request
        * @param a_sUrl the requested URL
        * @param a_mHeader a map with the header data
        * @return the HTTP result code 
        */
        virtual int handleGet(const std::string a_sUrl, const std::map<std::string, std::string> a_mHeader);

        /**
        * Handle a POST request
        * @param a_sUrl the requested URL
        * @param a_vBody the body of the request
        * @param a_mHeader a map with the header data
        * @return the HTTP result code 
        */
        virtual int handlePost(const std::string a_sUrl, const std::vector<unsigned char> a_vBody, const std::map<std::string, std::string> a_mHeader);

        /**
        * Handle a PUT request
        * @param a_sUrl the requested URL
        * @param a_vBody the body of the request
        * @param a_mHeader a map with the header data
        * @return the HTTP result code 
        */
        virtual int handlePut(const std::string a_sUrl, const std::vector<unsigned char> a_vBody, const std::map<std::string, std::string> a_mHeader);

        /**
        * Handle a DELETE request
        * @param a_sUrl the requested URL
        * @param a_mHeader a map with the header data
        * @return the HTTP result code 
        */
        virtual int handleDelete(const std::string a_sUrl, const std::map<std::string, std::string> a_mHeader);

        /**
        * Detach from the thread
        */
        virtual void detach() override;
    };
  }
}
