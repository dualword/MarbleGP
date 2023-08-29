// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <controller/CControllerGame.h>
#include <webserver/CWebServerBase.h>
#include <messages/CMessageHelpers.h>
#include <helpers/CTextureHelpers.h>
#include <helpers/CStringHelpers.h>
#include <threads/CMessageQueue.h>
#include <helpers/CDataHelpers.h>
#include <platform/CPlatform.h>
#include <json/CIrrJSON.h>
#include <CGlobal.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <ios>

#ifndef _WINDOWS
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <unistd.h>
#endif

namespace dustbin {
  namespace webserver {
    /**
    * The constructor. Takes the port number to listen to as argument
    * @param a_iPortNo the port number to listen to 
    */
    CWebServerBase::CWebServerBase(int a_iPortNo, threads::CInputQueue *a_pQueue) : m_bRunning(false), m_bStop(false), m_iPortNo(a_iPortNo), m_iError(0), m_pQueue(nullptr),
#ifdef _WINDOWS
      m_iSocket(INVALID_SOCKET)
#else
      m_iSocket(-1)
#endif
    {
      if (a_pQueue != nullptr) {
        m_pQueue = new threads::COutputQueue();
        a_pQueue->addSender(m_pQueue);
        m_pQueue->addListener(a_pQueue);
      }

#ifdef _WINDOWS
      WSADATA l_cWsa;

      m_iError = WSAStartup(MAKEWORD(2, 2), &l_cWsa);
      if (m_iError != 0) {
        std::string l_sError = "WSAStartup failed with code " + std::to_string(m_iError);
        printf((l_sError + "\n").c_str());
        
        return;
      }
#endif

#ifdef _WINDOWS
      struct addrinfo  l_cAddr;
      struct addrinfo *l_pData;

      ZeroMemory(&l_cAddr, sizeof(l_cAddr));

      l_cAddr.ai_family   = AF_INET;
      l_cAddr.ai_socktype = SOCK_STREAM;
      l_cAddr.ai_protocol = IPPROTO_TCP;
      l_cAddr.ai_flags    = AI_PASSIVE;

      m_iError = getaddrinfo(nullptr, std::to_string(a_iPortNo).c_str(), &l_cAddr, &l_pData);
      if (m_iError != 0) {
        std::string l_sError = "getaddrinfo failes with error " + std::to_string(WSAGetLastError());
        printf((l_sError + "\n").c_str());
        sendWeblogmessage((irr::s32)irr::ELL_ERROR, l_sError, m_pQueue);
        freeaddrinfo(l_pData);
        WSACleanup();
        return;
      }

      m_iSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

      if (m_iSocket == INVALID_SOCKET) {
        std::string l_sError = "Socket failed with error code " + std::to_string(WSAGetLastError());
        printf((l_sError + "\n").c_str());
        sendWeblogmessage((irr::s32)irr::ELL_ERROR, l_sError, m_pQueue);
        return;
      }

      int l_iBufLen = 0;
      m_iError = setsockopt(m_iSocket, SOL_SOCKET, SO_SNDBUF, (char *)&l_iBufLen, sizeof(int));
      if (m_iError != 0) {
        std::string l_sError = "Error while setting socket options: " + std::to_string(WSAGetLastError());
        printf((l_sError + "\n").c_str());
        sendWeblogmessage((irr::s32)irr::ELL_ERROR, l_sError, m_pQueue);
        return;
      }

      m_iError = bind(m_iSocket, l_pData->ai_addr, (int)l_pData->ai_addrlen);
      if (m_iError != 0) {
        std::string l_sError = "Bind failed with error " + std::to_string(WSAGetLastError());
        printf((l_sError + "\"").c_str());
        sendWeblogmessage((irr::s32)irr::ELL_ERROR, l_sError, m_pQueue);
        return;
      }

      char l_sHost[255];
      memset(l_sHost, 0, 255);

      if (gethostname(l_sHost, 255) == 0) {
        printf("HTTP server running on: \"%s\":4693\n", l_sHost);
        sendWeblogmessage((irr::s32)irr::ELL_INFORMATION, std::string("HTTP server running on: \"") + l_sHost + ":4693\"" , m_pQueue);
        m_sHostName = l_sHost;
      }

      freeaddrinfo(l_pData);
#else
      struct sockaddr_in l_cAddr;

      memset(&l_cAddr, 0, sizeof(l_cAddr));

      l_cAddr.sin_family = AF_INET; // sets to use IP
      l_cAddr.sin_addr.s_addr = htonl(INADDR_ANY); // sets our local IP address
      l_cAddr.sin_port = htons(a_iPortNo); // sets the server port number #endif

      m_iSocket = socket(AF_INET, SOCK_STREAM, 0);

      if (m_iSocket < 0) {
        std::string l_sError = "Socket has failed with error " + std::to_string(m_iSocket);
        sendWeblogmessage((irr::s32)irr::ELL_ERROR, l_sError, m_pQueue);
        return;
      }

      m_iError = bind(m_iSocket, (struct sockaddr *)&l_cAddr, sizeof(struct sockaddr));
      if (m_iError < 0) {
        std::string l_sError ="Error binding socket: " + std::to_string(m_iError);
        sendWeblogmessage((irr::s32)irr::ELL_ERROR, l_sError, m_pQueue);
        return;
      }

      struct ifaddrs *l_pAddr = nullptr;
      int l_iCount = getifaddrs(&l_pAddr);

      if (l_pAddr != nullptr) {
        for (struct  ifaddrs *l_pThis = l_pAddr; l_pThis != nullptr; l_pThis = l_pThis->ifa_next) {
          if (l_pThis->ifa_addr->sa_family == AF_INET) {
            if(l_pThis->ifa_addr != nullptr ){
              char l_sBuffer[INET_ADDRSTRLEN] = {0, };
              inet_ntop(
                AF_INET,
                &((struct sockaddr_in*)(l_pThis->ifa_addr))->sin_addr,
                l_sBuffer,
                INET_ADDRSTRLEN
              );

              std::string s = std::string(l_sBuffer);

              if (s != "127.0.0.1") {
                sendWeblogmessage((irr::s32) irr::ELL_INFORMATION, std::string("Host Address: \"") + s + "\"", m_pQueue);
                m_sHostName = s;
              }
            }
          }
        }
      }
#endif
    }

    CWebServerBase::~CWebServerBase() {
      if (m_pQueue != nullptr) {
        delete m_pQueue;
        m_pQueue = nullptr;
      }
    }

    /**
    * Get the name of the host
    * @return the name of the host
    */
    std::string CWebServerBase::hostName() {
      return m_sHostName;
    }

    /**
    * Create a webserver request object
    * @param a_iSocket the socket to use
    * @return an instance of IWebServerRequest handling the call
    */
    IWebServerRequest* CWebServerBase::createRequest(
#ifdef _WINDOWS
      SOCKET a_iSocket
#else
      int a_iSocket
#endif
    )
    {
      return new CWebServerRequestBase(m_pQueue, a_iSocket);
    }

    /**
    * Get the error code
    * @return the error code (0 == no error)
    */
    int CWebServerBase::getError() {
      return m_iError;
    }

    /**
    * Start the server thread. Will return immediately
    */
    void CWebServerBase::startServer() {
      m_bRunning = true;
      m_cThread = std::thread([this] { printf("HTTP Server running.\n"); execute(); m_bRunning = false; printf("HTTP Server stopped.\n"); });
    }

    /**
    * Stop the server. This method will tell the server
    * to stop but return at once
    */
    void CWebServerBase::stopServer() {
      m_bStop = true;
#ifdef _WINDOWS
      closesocket(m_iSocket);
#else
      shutdown(m_iSocket, SHUT_RDWR);
#endif
    }

    /**
    * Is the thread running?
    * @return "true" if the server is running, false otherwise
    */
    bool CWebServerBase::isRunning() {
      return m_bRunning;
    }

    /**
    * Join the threat, i.e. wait until it has finished
    */
    void CWebServerBase::join() {
      m_cThread.join();
    }

    void CWebServerBase::execute() {
      printf("Waiting for incoming connections.\n");

      while (!m_bStop) {
        printf("Listening...\n");
        int l_iResult = listen(m_iSocket, SOMAXCONN);
#ifdef _WINDOWS
        if (l_iResult == SOCKET_ERROR) {
          printf("Listen error %i\n", WSAGetLastError());
#else
        if (l_iResult < 0) {
          printf("Socket error %i\n", l_iResult);
#endif
          break;
        }

        printf("Done.\nAccepting...\n");
#ifdef _WINDOWS
        struct sockaddr_in l_cClient;
        int l_iClientSize = sizeof(l_cClient);

        SOCKET l_iRequest = WSAAccept(m_iSocket, (SOCKADDR *)&l_cClient, &l_iClientSize, nullptr, NULL);

        if (l_iRequest == INVALID_SOCKET) {
          printf("Accept error %i\n", WSAGetLastError());
          break;
        }
#else
        int l_iRequest = accept(m_iSocket, nullptr, nullptr);
        if (l_iRequest < 0) {
          printf("Accept error %i\n", l_iRequest);
          break;
        }
#endif

        printf("Ready.\n");
        IWebServerRequest *l_pRequest = createRequest(l_iRequest);
        l_pRequest->execute();
        delete l_pRequest;
      }
    }

    /**
    * Send data to the client
    * @param a_pData the data to send
    * @param a_iLen the number of bytes to send
    * @return the number of bytes sent
    */
    int CWebServerRequestBase::sendData(const char* a_pData, int a_iLen) {
      int l_iSent = send(m_iSocket, a_pData, a_iLen, 0);
      return l_iSent;
    }

    /**
    * Send header data
    * @param a_iResult the HTTP result code to send
    * @param a_sPath the path of the file on the server
    * @param a_mHeader string-string map with the header data. The first line will be added by the method
    */
    void CWebServerRequestBase::sendHeader(int a_iResult, const std::map<std::string, std::string>& a_mHeader) {
      std::string l_sResponse = "HTTP/1.1 " + std::to_string(a_iResult) + " " + getHttpStatusCode(a_iResult) + "\r\n";
      int l_iSent = sendData(l_sResponse.c_str(), (int)l_sResponse.size());
      // printf("\t%i of %i Bytes sent.\n", l_iSent, l_sResponse.size());

      for (std::map<std::string, std::string>::const_iterator l_itHeader = a_mHeader.begin(); l_itHeader != a_mHeader.end(); l_itHeader++) {
        std::string l_sHeader = l_itHeader->first + ": " + l_itHeader->second + "\r\n";
        l_iSent = sendData(l_sHeader.c_str(), (int)l_sHeader.size());
        // printf("\t%i of %i Bytes sent.\n", l_iSent, l_sResponse.size());
      }

      l_iSent = sendData("\r\n", 2);
      // printf("\t%i of %i Bytes sent.\n", l_iSent, l_sResponse.size());
    }


    /**
    * Get the content type defined by the file extension
    * @param a_sExtension the file extension
    * @return the content type
    */
    std::string CWebServerRequestBase::getContentType(const std::string& a_sExtension) {
      if (a_sExtension == ".css")
        return "text/css";
      else if (a_sExtension == ".htm" || a_sExtension == ".html")
        return "text/html";
      else if (a_sExtension == ".xml")
        return "text/xml";
      else if (a_sExtension == ".jpg" || a_sExtension == ".jpeg")
        return "image/jpeg";
      else if (a_sExtension == ".png")
        return "image/png";
      else if (a_sExtension == ".ico")
        return "image/vnd.microsoft.icon";
      else if (a_sExtension == ".js")
        return "text/javascript";
      else if (a_sExtension == ".json")
        return "application/json";
      else if (a_sExtension == ".txt")
        return "text/plain";
      else
        return "application/octet-stream";
    }

    /**
    * Get the string representation of the HTTP status core
    * @param a_iStatus the HTTP status code
    * @return the string representation of the HTTP status core
    */
    std::string CWebServerRequestBase::getHttpStatusCode(int a_iStatus) {
      switch (a_iStatus) {
        case 100: return "Continue";
        case 101: return "Switching Protocols";
        case 200: return "OK";
        case 201: return "Created";
        case 400: return "Bad Request";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 501: return "Not implemented";
        case 507: return "Insufficient Storage";
        default:
          return "Internal Server Error";
      }
    }

  /**
  * The constructor
  * @param a_iSocket socket identifier to use
  */
    CWebServerRequestBase::CWebServerRequestBase(threads::COutputQueue *a_pQueue,
#ifdef _WINDOWS
      SOCKET a_iSocket
#else
      int a_iSocket
#endif
    ) : m_iSocket(a_iSocket), m_bFinished(false), m_pFs(nullptr), m_pQueue(a_pQueue) {
      m_pFs = CGlobal::getInstance()->getFileSystem();

      // m_cThread = std::thread([this] { execute(); m_bFinished = true; detach(); delete(this); });
    }

    CWebServerRequestBase::~CWebServerRequestBase() {
      if (m_iSocket != INVALID_SOCKET) {
        closesocket(m_iSocket);
        printf("Socket closed.\n");
      }
    }

    /**
    * The execution method
    */
    void CWebServerRequestBase::execute() {
#ifdef _WINDOWS
      if (m_iSocket != INVALID_SOCKET) {
#else
      if (m_iSocket >= 0) {
#endif
        while (true) {
          enum class enHTTPState {
            HttpMethod,
            UrlPath,
            HttpVersion,
            HeaderKey,
            HeaderValue,
            LineBreak,
            HeaderEnd,
            Body
          };

          enHTTPState l_eState = enHTTPState::HttpMethod;

          std::string l_sMethod  = "";
          std::string l_sPath    = "";
          std::string l_sLine    = "";
          std::string l_sVersion = "";
          std::string l_sKey     = "";
          std::string l_sValue   = "";

          std::map<std::string, std::string> l_mHeaders;

          int l_iRead = 0;

          while (l_eState != enHTTPState::HeaderEnd) {
            char c;

            int l_iThisRead = recv(m_iSocket, &c, 1, 0);
            if (l_iThisRead != 0) {
              l_iRead += l_iThisRead;
              l_sLine += c;

              switch (l_eState) {
                case enHTTPState::HttpMethod : if (c != ' ' ) l_sMethod  += c; else l_eState = enHTTPState::UrlPath    ; break;
                case enHTTPState::UrlPath    : if (c != ' ' ) l_sPath    += c; else l_eState = enHTTPState::HttpVersion; break;
                case enHTTPState::HttpVersion: if (c != '\r') l_sVersion += c; else l_eState = enHTTPState::LineBreak  ; break;

                case enHTTPState::HeaderKey: 
                  if (c == '\r' && l_sKey == "")
                    l_eState = enHTTPState::HeaderEnd;
                  else if (c != ':') 
                    l_sKey += c; 
                  else
                    l_eState = enHTTPState::HeaderValue; 
                  break;

                case enHTTPState::HeaderValue:
                  if (c == '\r') {
                    if (l_sKey == "" && l_sValue == "") {
                      l_eState = enHTTPState::Body;
                    }
                    else {
                      l_mHeaders[l_sKey] = l_sValue;

                      l_sKey   = "";
                      l_sValue = "";

                      l_eState = enHTTPState::LineBreak;
                    }
                  }
                  else 
                    if (c != ' ' || l_sValue != "")
                      l_sValue += c;
                  break;

                case enHTTPState::LineBreak:
                  if (c == '\n') l_eState = enHTTPState::HeaderKey;
                  l_sLine = "";
                  break;

                case enHTTPState::Body:
                  break;
              }
            }
            else break;
          }

          printf("Method: \"%s\", Path: \"%s\", Version: \"%s\"\n", l_sMethod.c_str(), l_sPath.c_str(), l_sVersion.c_str());

          if (l_iRead > 0 && l_sMethod != "" && l_sVersion != "") {
            std::time_t l_cTime = std::time(nullptr);

            char s[0xFF];
            std::strftime(s, 255, "%Y/%M/%D_%H:%M:%S", std::localtime(& l_cTime));

            std::string l_sLog = std::string(s) + ": " + l_sMethod + " \"" + l_sPath + "\"";

            int l_iResult = -1;

            if (l_sMethod == "GET") {
              l_iResult = handleGet(l_sPath, l_mHeaders);
            }

            l_sLog += " (" + std::to_string(l_iResult) + ")";
            sendWeblogmessage((irr::s32)(l_iResult < 400 ? irr::ELL_INFORMATION : irr::ELL_WARNING), l_sLog, m_pQueue);
            return;
          }
          else {
            printf("Method: %s, Read: %i, Version: %s\n", l_sMethod.c_str(), l_iRead, l_sVersion.c_str());
            return;
          }
        }
      }
      else printf("Invalid socket!\n");

      printf("End of function.\n");
    }

    /**
    * Has the request finished?
    * @return true if the request has finished, false otherwise
    */
    bool CWebServerRequestBase::hasFinished() {
      return m_bFinished;
    }

    /**
    * Detach from the thread
    */
    void CWebServerRequestBase::detach() {
      m_cThread.detach();
    }


    /**
    * Create the track name JSON
    * @return a string with a JSON representation of the track name map
    */
    std::string CWebServerRequestBase::createTrackNameJSON() {
      std::map<std::string, std::string> l_mTracks = helpers::getTrackNameMap();

      std::string l_sReturn = "{ ";

      for (std::map<std::string, std::string>::iterator l_itName = l_mTracks.begin(); l_itName != l_mTracks.end(); l_itName++) {
        if (l_itName != l_mTracks.begin())
          l_sReturn += ", ";

        l_sReturn += "\"" + l_itName->first + "\": \"" + l_itName->second + "\"";
      }

      l_sReturn += "}";
      return l_sReturn;
    }

    /**
    * Create a JSON with the possible AI Help options
    * @return a JSON with the possible AI Help options
    */
    std::string CWebServerRequestBase::createAiHelpOptionJSON() {
      std::string l_sReturn = "[";

      std::vector<std::string> l_vOpts = helpers::getAiHelpOptions();

      for (std::vector<std::string>::iterator l_itAi = l_vOpts.begin(); l_itAi != l_vOpts.end(); l_itAi++) {
        if (l_itAi != l_vOpts.begin())
          l_sReturn += ",";

        l_sReturn += "\"" + *l_itAi + "\"";
      }

      return l_sReturn + "]";
    }

    /**
    * Return a string with a JSON representation of the profiles
    * @return a string with a JSON representation of the profiles
    */
    std::string CWebServerRequestBase::getProfileData() {
      std::vector<data::SPlayerData> l_vPlayers = data::SPlayerData::createPlayerVector(CGlobal::getInstance()->getSetting("profiles"));

      std::string l_sReturn = "[";

      for (std::vector<data::SPlayerData>::iterator l_itPlr = l_vPlayers.begin(); l_itPlr != l_vPlayers.end(); l_itPlr++) {
        if (l_itPlr != l_vPlayers.begin())
          l_sReturn += ",";

        l_sReturn += (*l_itPlr).to_json();
      }

      return l_sReturn + "]";
    }

    /**
    * Get the Base64 representation of an image
    * @param a_sImage path to the image
    * @return the Base64 representation of the image
    */
    std::string CWebServerRequestBase::getBase64Image(const std::string& a_sImage) {
      std::string l_sReturn = "";

      irr::io::IReadFile* l_pFile = m_pFs->createAndOpenFile(a_sImage.c_str());

      if (l_pFile != nullptr) {
        unsigned char *l_pBuffer = new unsigned char[l_pFile->getSize()];
        l_pFile->read((void *)l_pBuffer, l_pFile->getSize());

        l_sReturn = messages::base64Encode(l_pBuffer, l_pFile->getSize(), false);

        l_pFile->drop();
        delete []l_pBuffer;
      }

      return l_sReturn;
    }


    /**
    * Get a JavaScript snippet filling the texture pattern dictionary
    * @return a JavaScript snippet filling the texture pattern dictionary
    */
    std::string CWebServerRequestBase::getTexturePatternJS() {
      std::string l_sReturn = "";

      std::vector<std::string> l_vPatterns = helpers::getTexturePatterns();

      for (std::vector<std::string>::iterator l_itTexture = l_vPatterns.begin(); l_itTexture != l_vPatterns.end(); l_itTexture++) {
        l_sReturn += "      g_Patterns[\"" + *l_itTexture + "\"] = new Image();\n";
        l_sReturn += "      g_Patterns[\"" + *l_itTexture + "\"].src = \"data:image/png;base64," + getBase64Image("data/patterns/" + *l_itTexture) + "\"\n";
      }

      return l_sReturn;
    }


    /**
    * Get the JSON with the results of the last championship
    * @return the JSON with the results of the last championship
    */
    std::string CWebServerRequestBase::getChampionshipData() {
      std::string l_sXmlPath = helpers::ws2s(platform::portableGetDataPath()) + "championship_result.json";
      std::string l_sReturn  = "";

      irr::io::IReadFile *l_pFile = m_pFs->createAndOpenFile(l_sXmlPath.c_str());

      if (l_pFile != nullptr) {
        int l_iBufLen = l_pFile->getSize();
        char *l_aBuffer = new char[l_iBufLen + 1];
        memset(l_aBuffer, 0, l_iBufLen + 1);
        l_pFile->read(l_aBuffer, l_iBufLen);

        l_pFile->drop();

        l_sReturn = l_aBuffer;
      }

      return l_sReturn;
    }

    /**
    * Save the profiles transmitted by the browser
    * @param a_sData JSON encoded profile data
    */
    bool CWebServerRequestBase::saveProfileData(const std::string& a_sData) {
      return true;
    }

    /**
    * Handle a GET request
    * @param a_sUrl the requested URL
    * @param a_mHeader a map with the header data
    * @return the HTTP result code 
    */
    int CWebServerRequestBase::handleGet(const std::string a_sUrl, const std::map<std::string, std::string> a_mHeader) {
      const std::string l_sPathThumbnails = "/thumbnails/";
      const std::string l_sReplaceInclude = "include:";
      const std::string l_sPathSave       = "/saveprofiles?";

      std::string l_sPath = a_sUrl;

      std::map<std::string, std::string> l_mHeader;

      std::string l_sExtension = "";

      char *l_aBuffer = nullptr;
      int   l_iBufLen = 0;
      int   l_iResult = 0;

      if (l_sPath.substr(0, l_sPathSave.size()) == l_sPathSave) {
        std::string l_sJSON = messages::urlDecode(l_sPath.substr(l_sPathSave.length()));
        std::string l_sResponse = "Profiles Saved.";
        printf("\n\n%s\n\n", l_sJSON.c_str());

        json::CIrrJSON l_cJson = json::CIrrJSON(l_sJSON);

        std::vector<data::SPlayerData> l_vPlayers;

        int l_eState = 0;
        std::string l_sKey;
        std::string l_sCtrl = "";
        std::string l_sCtrlData = "";

        while (l_cJson.read()) {
          switch (l_eState) {
            case 0:
              if (l_cJson.getType() == json::CIrrJSON::enToken::ObjectStart) {
                l_vPlayers.push_back(data::SPlayerData());
                l_eState = 1;
              }
              else if (l_cJson.getType() == json::CIrrJSON::enToken::ArrayEnd) {
                printf("End Reached.\n");
              }
              break;

            case 1:
              if (l_cJson.getType() == json::CIrrJSON::enToken::ValueString) {
                l_sKey = l_cJson.asString();
                printf("Key: \"%s\"\n", l_sKey.c_str());
                l_eState = 2;
              }
              break;

            case 2:
              if (l_cJson.getType() == json::CIrrJSON::enToken::Colon)
                l_eState = 3;
              break;

            case 3:
              if (l_cJson.isValueType()) {
                if (l_sKey == "name")
                  l_vPlayers.back().m_sName = l_cJson.asString();
                else if (l_sKey == "short")
                  l_vPlayers.back().m_sShortName = l_cJson.asString();
                else if (l_sKey == "ai_help")
                  l_vPlayers.back().m_eAiHelp = (dustbin::data::SPlayerData::enAiHelp)l_cJson.asInt();
                else if (l_sKey == "auto_throttle")
                  l_vPlayers.back().m_bAutoThrottle = l_cJson.asBool();
                else if (l_sKey == "texture")
                  l_vPlayers.back().m_sTexture = l_cJson.asString();
                else if (l_sKey == "controller")
                  l_sCtrl = l_cJson.asString();
                else if (l_sKey == "ctrl_data")
                  l_sCtrlData = l_cJson.asString();
                else
                  printf("Unknown key \"%s\"\n", l_sKey.c_str());
              }
              l_eState = 4;
              break;

            case 4:
              if (l_cJson.getType() == json::CIrrJSON::enToken::Separator)
                l_eState = 1;
              else if (l_cJson.getType() == json::CIrrJSON::enToken::ObjectEnd) {
                if (l_sCtrl != "" && l_sCtrlData != "") {
                  if (l_sCtrl == "TouchControl")
                    l_vPlayers.back().m_sControls = "DustbinTouchControl";
                  else if (l_sCtrl == "Gyroscope")
                    l_vPlayers.back().m_sControls = "DustbinGyroscope";
                  else {
                    controller::CControllerGame l_cCtrl;
                    l_cCtrl.deserialize(l_sCtrlData);

                    switch ((*l_cCtrl.getInputs().begin()).m_eType) {
                      case controller::CControllerBase::enInputType::JoyAxis:
                      case controller::CControllerBase::enInputType::JoyButton:
                      case controller::CControllerBase::enInputType::JoyPov:
                        if (l_sCtrl == "Gamepad") {
                          l_vPlayers.back().m_sControls = l_sCtrlData;
                        }
                        else {
                          l_vPlayers.back().m_sControls = helpers::getDefaultGameCtrl_Keyboard();
                        }
                        break;

                      case controller::CControllerBase::enInputType::Key:
                        if (l_sCtrl == "Keyboard") {
                          l_vPlayers.back().m_sControls = l_sCtrlData;
                        }
                        else {
                          l_vPlayers.back().m_sControls = helpers::getDefaultGameCtrl_Gamepad();
                        }
                        break;
                    }
                  }
                }
                l_eState = 0;
              }
              break;
          }
        }


        helpers::saveProfiles(l_vPlayers);

        l_aBuffer = new char[l_sResponse.size() + 1];
        memset(l_aBuffer,0, l_sResponse.size() + 1);
        memcpy(l_aBuffer, l_sResponse.c_str(), l_sResponse.size());
        l_iBufLen = (int)l_sResponse.size();
        l_sExtension = ".txt";
      }
      else if (l_sPath.substr(0, l_sPathThumbnails.size()) == l_sPathThumbnails) {
        std::string l_sImage = "data/levels/" + l_sPath.substr(l_sPathThumbnails.size()) + "/thumbnail.png";

        printf("Thumbnail: \"%s\"\n", l_sImage.c_str());
        if (m_pFs->existFile(l_sImage.c_str())) {
          irr::io::IReadFile *l_pFile = m_pFs->createAndOpenFile(l_sImage.c_str());

          if (l_pFile != nullptr) {
            l_iBufLen = l_pFile->getSize();
            l_aBuffer = new char[l_iBufLen];
            memset(l_aBuffer, 0, l_iBufLen);
            l_pFile->read(l_aBuffer, l_iBufLen);
            l_pFile->drop();

            l_sExtension = ".png";
          }
        }
      }

      if (l_aBuffer == nullptr) {
        if (l_sPath == "/")
          l_sPath = "/index.html";

        l_sExtension = l_sPath.find_last_of('.') != std::string::npos ? l_sPath.substr(l_sPath.find_last_of('.')) : "";

        std::string l_sRelativePath = "data/html" + l_sPath;

        irr::io::IReadFile *l_pFile = m_pFs->createAndOpenFile(l_sRelativePath.c_str());

        if (l_pFile != nullptr) {
          l_aBuffer = new char[l_pFile->getSize() + 1];
          memset(l_aBuffer, 0, l_pFile->getSize() + 1);
          l_iBufLen = l_pFile->getSize();

          l_pFile->read(l_aBuffer, l_iBufLen);
          l_pFile->drop();

          l_iResult = 200;

          if (l_sExtension == ".html") {
            std::string l_sFile = l_aBuffer;

            bool l_bReplace = false;

            while (l_sFile.find("{!") != std::string::npos) {
              std::string l_sSub = l_sFile.substr(l_sFile.find("{!") + 2);
              if (l_sSub.find("}") != std::string::npos) {
                l_bReplace = true;

                std::string l_sReplace = l_sSub.substr(0, l_sSub.find("}"));
                
                if (l_sReplace == "tracknames") {
                  l_sReplace = createTrackNameJSON();
                }
                else if (l_sReplace == "championship") {
                  l_sReplace = getChampionshipData();
                }
                else if (l_sReplace == "profiles") {
                  l_sReplace = getProfileData();
                }
                else if (l_sReplace == "profilecount") {
#ifdef _WINDOWS
                  l_sReplace = "8";
#else
                  l_sReplace = "3";
#endif
                }
                else if (l_sReplace == "controloptions") {
#ifdef _WINDOWS
                  l_sReplace = "[ \"Gamepad\", \"Keyboard\" ]";
#else
                  l_sReplace = "[ \"Gamepad\", \"TouchControl\", \"Gyroscope\" ]";
#endif
                }
                else if (l_sReplace == "defaultcontrol") {
#ifdef _WINDOWS
                  l_sReplace = "\"Keyboard\"";
#else
                  l_sReplace = "\"TouchControl\""
#endif
                }
                else if (l_sReplace == "textureframe") {
                  l_sReplace = "data:image/png;base64," + getBase64Image("data/textures/texture_top.png");
                }
                else if (l_sReplace == "texturenumber") {
                  l_sReplace = "data:image/png;base64," + getBase64Image("data/textures/one.png");
                }
                else if (l_sReplace == "texturenumberglow") {
                  l_sReplace = "data:image/png;base64," + getBase64Image("data/textures/one_glow.png");
                }
                else if (l_sReplace == "texturepatterns") {
                  l_sReplace = getTexturePatternJS();
                }
                else if (l_sReplace == "aihelpoptions") {
                  l_sReplace = createAiHelpOptionJSON();
                }
                else if (l_sReplace.substr(0, l_sReplaceInclude.size()) == l_sReplaceInclude) {
                  printf("Include: \"%s\"\n", l_sReplace.substr(l_sReplaceInclude.size()).c_str());
                  irr::io::IReadFile *l_pFile = m_pFs->createAndOpenFile((std::string("data/html/") + l_sReplace.substr(l_sReplaceInclude.size())).c_str());
                  if (l_pFile != nullptr) {
                    char *p = new char[l_pFile->getSize() + 1];
                    memset(p, 0, l_pFile->getSize() + 1);
                    l_pFile->read(p, l_pFile->getSize());
                    l_sReplace = p;
                    delete []p;
                  }
                  else l_sReplace = std::string("<!-- File not found: ") + l_sReplace.substr(l_sReplaceInclude.size()).c_str() + " -->";
                }
                else {
                  l_sReplace = "<!-- Unkonw replacement " + l_sReplace + " -->";
                }

                std::string l_sNewFile = l_sFile.substr(0, l_sFile.find("{!")) + l_sReplace + l_sSub.substr(l_sSub.find("}") + 1);
                l_sFile = l_sNewFile;
              }
            }

            if (l_bReplace) {
              delete[] l_aBuffer;
              l_iBufLen = (int)l_sFile.size();
              l_aBuffer = new char[l_iBufLen];
              memset(l_aBuffer, 0, l_iBufLen);
              memcpy(l_aBuffer, l_sFile.c_str(), l_iBufLen);
            }
          }
        }
      }

      if (l_aBuffer != nullptr && l_iBufLen > 0) {
        l_mHeader["Content-Type"  ] = getContentType(l_sExtension);
        l_mHeader["Content-Length"] = std::to_string(l_iBufLen);
        l_mHeader["Connection"    ] = "close";

        char *l_aToDelete = l_aBuffer;

        int l_iTotal = 0;
        int l_iSize  = l_iBufLen;

        sendHeader(l_iResult, l_mHeader);
        while (l_iBufLen > 0) {
          int l_iSent = sendData(l_aBuffer, l_iBufLen > 1024 ? 1024 : l_iBufLen);
          if (l_iBufLen > l_iSent) {
            l_iBufLen -= l_iSent;
            l_aBuffer += l_iSent;
          }
          else l_iBufLen = 0;
          l_iTotal  += l_iSent;

          // printf("\t%i of %i Bytes sent (Total: %i).\n", l_iSent, l_iSize, l_iTotal);
        }
        send(m_iSocket, "", 0, 0);

        delete[] l_aToDelete;

        return 200;
      }
      else {
        return send404(l_sPath);
      }
    }

    /**
    * Send a 404
    * @param a_sPath the path of the not-found file
    * @return 404
    */
    int CWebServerRequestBase::send404(const std::string &a_sPath) {
      std::map<std::string, std::string> l_mHeader;

      std::string l_s404 = "<!DOCTYPE html>\n<html><head><title>404 - not found</title></head><body>Error 404 - Page &quot;" + a_sPath + "&quot; not found.</body></html>";

      l_mHeader["Content-Type"] = getContentType(".html");
      l_mHeader["Content-Length"] = std::to_string(l_s404.size());

      sendHeader(404, l_mHeader);

      sendData(l_s404.c_str(), (int)l_s404.size());

      return 404;
    }

    /**
    * Handle a POST request
    * @param a_sUrl the requested URL
    * @param a_vBody the body of the request
    * @param a_mHeader a map with the header data
    * @return the HTTP result code 
    */
    int CWebServerRequestBase::handlePost(const std::string a_sUrl, const std::vector<unsigned char> a_vBody, const std::map<std::string, std::string> a_mHeader) {
      send404(a_sUrl);
      return 404;
    }

    /**
    * Handle a PUT request
    * @param a_sUrl the requested URL
    * @param a_vBody the body of the request
    * @param a_mHeader a map with the header data
    * @return the HTTP result code 
    */
    int CWebServerRequestBase::handlePut(const std::string a_sUrl, const std::vector<unsigned char> a_vBody, const std::map<std::string, std::string> a_mHeader) {
      send404(a_sUrl);
      return 404;
    }

    /**
    * Handle a DELETE request
    * @param a_sUrl the requested URL
    * @param a_mHeader a map with the header data
    * @return the HTTP result code 
    */
    int CWebServerRequestBase::handleDelete(const std::string a_sUrl, const std::map<std::string, std::string> a_mHeader) {
      send404(a_sUrl);
      return 404;
    }

  }
}