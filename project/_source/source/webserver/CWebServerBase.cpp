// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
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
    typedef struct SWebFileBuffer {
      char *m_pBuffer;
      int   m_iSize;

      SWebFileBuffer() : m_pBuffer(nullptr), m_iSize(0) {
      }

      ~SWebFileBuffer() {
        if (m_pBuffer != nullptr)
          delete []m_pBuffer;
      }
    }
    SWebFileBuffer;

    std::map<std::string, SWebFileBuffer *> g_mFiles;
    std::map<std::string, SWebFileBuffer *> g_mTracks;
    std::map<std::string, std::string     > g_mPatterns;

    std::string g_sChampionshipData;
    std::string g_sTrackNames;
    std::string g_sProfileData;
    std::string g_sAiProfileData;

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
      irr::io::IFileSystem *l_pFs = CGlobal::getInstance()->getFileSystem();

      fillHtmlFileMap (l_pFs);
      loadChampionship(l_pFs);
      fillThumbnailMap(l_pFs);

      fillTexturePatterns();
      loadTrackMapping   ();
      loadProfileData    ();
      loadAiProfileData  ();

      printf("Setup done.\n");
    }

    CWebServerBase::~CWebServerBase() {
      if (m_pQueue != nullptr) {
        delete m_pQueue;
        m_pQueue = nullptr;
      }

      for (auto l_cFile : g_mFiles) {
        delete l_cFile.second;
        l_cFile.second = nullptr;
      }

      for (auto l_cTrack: g_mTracks) {
        delete l_cTrack.second;
        l_cTrack.second = nullptr;
      }

      g_mFiles   .clear();
      g_mTracks  .clear();
      g_mPatterns.clear();
    }

    /**
    * Load AI profile data into a JavaScript string
    * @param a_pFs the file system to use
    */
    void CWebServerBase::loadAiProfileData() {
      g_sAiProfileData = "[";

      std::vector<std::tuple<std::string, std::string, std::string, int, int, float>> l_vAiPlayers;

      helpers::loadAiProfiles(l_vAiPlayers);

      for (std::vector<std::tuple<std::string, std::string, std::string, int, int, float>>::iterator l_itAi = l_vAiPlayers.begin(); l_itAi != l_vAiPlayers.end(); l_itAi++) {
        if (l_itAi != l_vAiPlayers.begin())
          g_sAiProfileData += ",";

        g_sAiProfileData += "{\"name\":\"" + std::get<0>(*l_itAi) + "\",\"short\":\"" + std::get<1>(*l_itAi) + "\",\"texture\":\"" + std::get<2>(*l_itAi) + "\"}";
      }

      g_sAiProfileData += "]";
    }

    /**
    * Load profile data into a JavaScript string
    * @param a_pFs the file system to use
    */
    void CWebServerBase::loadProfileData() {
      std::vector<data::SPlayerData> l_vPlayers = data::SPlayerData::createPlayerVector(CGlobal::getInstance()->getSetting("profiles"));

      g_sProfileData = "[";

      for (std::vector<data::SPlayerData>::iterator l_itPlr = l_vPlayers.begin(); l_itPlr != l_vPlayers.end(); l_itPlr++) {
        if (l_itPlr != l_vPlayers.begin())
          g_sProfileData += ",";

        g_sProfileData += (*l_itPlr).to_json();
      }

      g_sProfileData += "]";
    }

    /**
    * Load the mapping from track identifier to name into a global string
    * @param a_pFs the file system to use
    */
    void CWebServerBase::loadTrackMapping() {
      std::map<std::string, std::string> l_mTracks = helpers::getTrackNameMap();

      g_sTrackNames = "{ ";

      for (std::map<std::string, std::string>::iterator l_itName = l_mTracks.begin(); l_itName != l_mTracks.end(); l_itName++) {
        if (l_itName != l_mTracks.begin())
          g_sTrackNames += ", ";

        g_sTrackNames += "\"" + l_itName->first + "\": \"" + l_itName->second + "\"";
      }

      g_sTrackNames += "}";
    }

    /**
    * fill the global map of track thumbnails
    * @param a_pFs the file system to use
    */
    void CWebServerBase::fillThumbnailMap(irr::io::IFileSystem* a_pFs) {
      irr::io::IReadFile *l_pFile = a_pFs->createAndOpenFile("data/levels/tracks.dat");

      if (l_pFile != nullptr) {
        long l_iSize = l_pFile->getSize();
        char *s = new char[l_iSize + 1];
        memset(s, 0, static_cast<size_t>(l_iSize) + 1);
        l_pFile->read(s, l_iSize);
        std::vector<std::string> l_vTracks = helpers::splitString(s, '\n');
        delete []s;
        l_pFile->drop();

        for (auto s: l_vTracks) {
          std::string l_sTrack = s;
          if (l_sTrack.substr(l_sTrack.size() - 1) == "\r")
            l_sTrack = l_sTrack.substr(0, l_sTrack.size() - 1);

          std::string l_sPath = "data/levels/" + l_sTrack + "/thumbnail.png";

          if (a_pFs->existFile(l_sPath.c_str())) {
            l_pFile = a_pFs->createAndOpenFile(l_sPath.c_str());

            if (l_pFile != nullptr) {
              SWebFileBuffer *p = new SWebFileBuffer();
              p->m_iSize = l_pFile->getSize();
              p->m_pBuffer = new char[p->m_iSize];
              l_pFile->read(p->m_pBuffer, p->m_iSize);
              l_pFile->drop();
              g_mTracks[l_sTrack] = p;
            }
          }
        }
      }
    }

    /**
    * Load the championship data into a global string
    * @param a_pFs the file system to use
    */
    void CWebServerBase::loadChampionship(irr::io::IFileSystem* a_pFs) {
      std::string l_sJsonPath = helpers::ws2s(platform::portableGetDataPath()) + "tournament.json";

      irr::io::IReadFile *l_pFile = a_pFs->createAndOpenFile(l_sJsonPath.c_str());

      if (l_pFile != nullptr) {
        int l_iBufLen = l_pFile->getSize();
        char *l_aBuffer = new char[l_iBufLen + 1];
        memset(l_aBuffer, 0, l_iBufLen + 1);
        l_pFile->read(l_aBuffer, l_iBufLen);
        l_pFile->drop();
        g_sChampionshipData = l_aBuffer;
      }
    }

    /**
    * Fill the global map of base64 encoded texture patterns
    */
    void CWebServerBase::fillTexturePatterns() {
      std::vector<std::string> l_vPatterns = helpers::getTexturePatterns();

      for (auto l_cPattern: l_vPatterns) {
        g_mPatterns[l_cPattern] = getBase64Image("data/patterns/" + l_cPattern);
      }

      g_mPatterns["__frame"     ] = getBase64Image("data/textures/texture_top.png");
      g_mPatterns["__number"    ] = getBase64Image("data/textures/one.png");
      g_mPatterns["__numberglow"] = getBase64Image("data/textures/one_glow.png");
    }

    /**
    * Fill the global map of HTML files
    * @param a_pFs the file system to use
    */
    void CWebServerBase::fillHtmlFileMap(irr::io::IFileSystem* a_pFs) {
      irr::io::IXMLReaderUTF8 *l_pList = a_pFs->createXMLReaderUTF8("data/htmlfiles.xml");

      if (l_pList != nullptr) {
        while (l_pList->read()) {
          if (l_pList->getNodeType() == irr::io::EXN_ELEMENT && std::string(l_pList->getNodeName()) == "file") {
            std::string l_sFile = std::string("data/html/") + l_pList->getAttributeValue("name");

            if (a_pFs->existFile(l_sFile.c_str())) {
              irr::io::IReadFile *l_pFile = a_pFs->createAndOpenFile(l_sFile.c_str());

              if (l_pFile != nullptr) {
                std::string l_sExtension = l_sFile.find_last_of('.') != std::string::npos ? l_sFile.substr(l_sFile.find_last_of('.')) : "";

                bool l_bIsTextFile = l_sExtension == ".html" || l_sExtension == ".css" || l_sExtension == ".js";

                SWebFileBuffer *p = new SWebFileBuffer();

                p->m_iSize   = (int)l_pFile->getSize();

                if (l_bIsTextFile) {
                  p->m_pBuffer = new char[p->m_iSize + 1];
                  memset(p->m_pBuffer, 0, p->m_iSize + 1);
                }
                else {  
                  p->m_pBuffer = new char[p->m_iSize];
                }

                l_pFile->read(p->m_pBuffer, p->m_iSize);
                g_mFiles[l_pList->getAttributeValue("name")] = p;

                l_pFile->drop();
              }
            }
          }
        }

        l_pList->drop();
      }
    }

    /**
    * Get the Base64 representation of an image
    * @param a_sImage path to the image
    * @return the Base64 representation of the image
    */
    std::string CWebServerBase::getBase64Image(const std::string& a_sImage) {
      std::string l_sReturn = "";

      irr::io::IReadFile* l_pFile = CGlobal::getInstance()->getFileSystem()->createAndOpenFile(a_sImage.c_str());

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

        DWORD l_iTimeout = 2500;
        setsockopt(l_iRequest, SOL_SOCKET, SO_RCVTIMEO, (const char *)&l_iTimeout, sizeof(DWORD));

#else
        int l_iRequest = accept(m_iSocket, nullptr, nullptr);
        if (l_iRequest < 0) {
          printf("Accept error %i\n", l_iRequest);
          break;
        }

        struct timeval l_cTimeout;
        l_cTimeout.tv_sec  = 3;
        l_cTimeout.tv_usec = 0;

        setsockopt (l_iRequest, SOL_SOCKET, SO_RCVTIMEO, &l_cTimeout, sizeof l_cTimeout);
#endif

        printf("Ready.\n");
        IWebServerRequest *l_pRequest = createRequest(l_iRequest);
        l_pRequest->startThread();
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

      for (std::map<std::string, std::string>::const_iterator l_itHeader = a_mHeader.begin(); l_itHeader != a_mHeader.end(); l_itHeader++) {
        std::string l_sHeader = l_itHeader->first + ": " + l_itHeader->second + "\r\n";
        l_iSent = sendData(l_sHeader.c_str(), (int)l_sHeader.size());
      }

      l_iSent = sendData("\r\n", 2);
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
    ) : m_iSocket(a_iSocket), m_bFinished(false), m_pQueue(a_pQueue) {
    }

    CWebServerRequestBase::~CWebServerRequestBase() {
#ifdef _WINDOWS
      if (m_iSocket != INVALID_SOCKET) {
        closesocket(m_iSocket);
        printf("Socket closed.\n");
      }
#else
      if (m_iSocket >= 0) {
        close(m_iSocket);
        printf("Socket closed.\n");
        sendWeblogmessage((irr::s32) irr::ELL_INFORMATION, "Web request handler stopped.", m_pQueue);
      }
#endif
    }

    void CWebServerRequestBase::startThread() {
      m_cThread = std::thread([this] { printf("HTTP Request running.\n"); execute(); printf("HTTP Request stopped.\n"); detach(); delete this; });
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
        bool l_bStop = false;

        while (!l_bStop) {
          int l_iData = 0;
          int l_iRead = 0;

          std::vector<std::string> l_vHeaders;

          enum class enState {
            HeaderLine,
            LineBreak,
            EndOfHeader
          };

          enState l_eState = enState::HeaderLine;

          std::string l_sLine = "";

          do {
            char l_cData = '\0';
            l_iData = recv(m_iSocket, &l_cData, 1, 0);
            l_iRead += l_iData;

            if (l_iRead > 0) {
              switch (l_eState) {
                case enState::HeaderLine:
                  if (l_cData == '\r')
                    l_eState = enState::LineBreak;
                  else
                    l_sLine += l_cData;
                  break;

                case enState::LineBreak:
                  if (l_cData == '\n') {
                    if (l_sLine != "") {
                      l_vHeaders.push_back(l_sLine);
                      l_eState = enState::HeaderLine;
                      l_sLine = "";
                    }
                    else l_eState = enState::EndOfHeader;
                  }
                  break;

                case enState::EndOfHeader:
                  break;
              }
            }
          }
          while (l_iData > 0 && l_eState != enState::EndOfHeader);
          
          if (l_iRead > 0) {
            std::string l_sMethod  = "";
            std::string l_sPath    = "";
            std::string l_sLine    = "";
            std::string l_sVersion = "";
            std::string l_sKey     = "";
            std::string l_sValue   = "";

            std::map<std::string, std::string> l_mHeaders;

            for (std::vector<std::string>::iterator l_itHead = l_vHeaders.begin(); l_itHead != l_vHeaders.end(); l_itHead++) {
              if (l_itHead == l_vHeaders.begin()) {
                printf("First Line: \"%s\"\n", (*l_itHead).c_str());
                std::vector<std::string> l_vTokens = helpers::splitString(*l_itHead, ' ');

                for (auto l_sToken: l_vTokens) {
                  printf("Header: \"%s\"\n", l_sToken.c_str());
                }

                if (l_vTokens.size() == 3) {
                  l_sMethod  = l_vTokens[0];
                  l_sPath    = l_vTokens[1];
                  l_sVersion = l_vTokens[2];
                }
              }
              else {
                std::vector<std::string> l_vLine = helpers::splitString(*l_itHead, ':');
                if (l_vLine.size() == 2) {
                  l_mHeaders[helpers::trimString(l_vLine[0])] = helpers::trimString(l_vLine[1]);
                }
              }
            }

          

            printf("Connection: \"%s\"\"", l_mHeaders.find("Connection") != l_mHeaders.end() ? l_mHeaders["Connection"].c_str() : "-- Not Found --");
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
              // return;
            }
            else {
              printf("Method: %s, Read: %i, Version: %s\n", l_sMethod.c_str(), l_iRead, l_sVersion.c_str());
              // return;
            }
          }
          else {
            printf("Timeout, stopping thread.\n");
            l_bStop = true;
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
    * Get a JavaScript snippet filling the texture pattern dictionary
    * @return a JavaScript snippet filling the texture pattern dictionary
    */
    std::string CWebServerRequestBase::getTexturePatternJS() {
      std::string l_sReturn = "";

      for (auto l_sPattern: g_mPatterns) {
        l_sReturn += "      g_Patterns[\"" + l_sPattern.first + "\"] = new Image();\n";
        l_sReturn += "      g_Patterns[\"" + l_sPattern.first + "\"].src = \"data:image/png;base64," + l_sPattern.second + "\"\n";
      }

      return l_sReturn;
    }

    /**
    * Save the AI profiles transmitted by the browser
    * @param a_sData JSON encoded AI profile data
    * @param a_sResponse the message sent to the client
    */
    bool CWebServerRequestBase::saveAiProfileData(const std::string& a_sData, std::string &a_sResponse) {
      bool l_bRet = true;

      std::vector<std::tuple<std::string, std::string, std::string, int, int, float>> l_vAiPlayers;
      helpers::loadAiProfiles(l_vAiPlayers);

      std::vector<std::tuple<std::string, std::string, std::string, int, int, float>>::iterator l_itAi = l_vAiPlayers.begin();

      json::CIrrJSON l_cJson = json::CIrrJSON(a_sData);

      int l_iCount = 0;   // We update a maxmimum of 16 profiles!!
      int l_iState = 0;

      std::string l_sKey = "";

      while (l_cJson.read() && l_iCount < 16 && l_itAi != l_vAiPlayers.end()) {
        switch (l_iState) {
          case 0:
            if (l_cJson.getType() == json::CIrrJSON::enToken::ObjectStart) {
              l_iState = 1;
            }
            else if (l_cJson.getType() == json::CIrrJSON::enToken::ArrayEnd) {
              l_itAi = l_vAiPlayers.end();
            }
            break;

          case 1:
            if (l_cJson.getType() == json::CIrrJSON::enToken::ValueString) {
              l_sKey = l_cJson.asString();
              l_iState = 2;
            }
            break;

          case 2:
            if (l_cJson.getType() == json::CIrrJSON::enToken::Colon) {
              l_iState = 3;
            }

          case 3:
            if (l_cJson.getType() == json::CIrrJSON::enToken::ValueString) {
              printf("\"%s\": \"%s\" (%i)\n", l_sKey.c_str(), l_cJson.asString().c_str(), l_iCount);
              if (l_sKey == "name")
                std::get<0>(*l_itAi) = l_cJson.asString();
              else if (l_sKey == "short")
                std::get<1>(*l_itAi) = l_cJson.asString();
              else if (l_sKey == "texture")
                std::get<2>(*l_itAi) = l_cJson.asString();

              l_iState = 4;
            }
            break;

          case 4:
            if (l_cJson.getType() == json::CIrrJSON::enToken::Separator)
              l_iState = 1;
            else if (l_cJson.getType() == json::CIrrJSON::enToken::ObjectEnd) {
              l_itAi++;
              l_iCount++;
              l_iState = 0;
            }
            break;
        }
      }

      if (!helpers::saveAiProfiles(l_vAiPlayers))
        a_sResponse = "Saving AI profiles failed.";

      return l_bRet;
    }

    /**
    * Save the profiles transmitted by the browser
    * @param a_sData JSON encoded profile data
    * @param a_sResponse the message sent to the client
    */
    bool CWebServerRequestBase::saveProfileData(const std::string& a_sData, std::string &a_sResponse) {
      bool l_bRet = true;

      json::CIrrJSON l_cJson = json::CIrrJSON(a_sData);

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

      return l_bRet;
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
      const std::string l_sPathSaveAi     = "/saveaiprofiles?";

      std::string l_sPath = a_sUrl;

      std::map<std::string, std::string> l_mHeader;

      std::string l_sExtension = "";

      char *l_aBuffer = nullptr;
      int   l_iBufLen = 0;
      int   l_iResult = 0;

      if (l_sPath.substr(0, l_sPathSave.size()) == l_sPathSave) {
        std::string l_sJSON = messages::urlDecode(l_sPath.substr(l_sPathSave.length()));
        std::string l_sResponse = "Profiles Saved.";

        saveProfileData(l_sJSON, l_sResponse);

        l_aBuffer = new char[l_sResponse.size() + 1];
        memset(l_aBuffer,0, l_sResponse.size() + 1);
        memcpy(l_aBuffer, l_sResponse.c_str(), l_sResponse.size());
        l_iBufLen = (int)l_sResponse.size();
        l_sExtension = ".txt";
      }
      else if (l_sPath.substr(0, l_sPathSaveAi.size()) == l_sPathSaveAi) {
        std::string l_sJSON = messages::urlDecode(l_sPath.substr(l_sPathSaveAi.length()));
        std::string l_sResponse = "AI Profiles Saved.";

        saveAiProfileData(l_sJSON, l_sResponse);

        l_aBuffer = new char[l_sResponse.size() + 1];
        memset(l_aBuffer,0, l_sResponse.size() + 1);
        memcpy(l_aBuffer, l_sResponse.c_str(), l_sResponse.size());
        l_iBufLen = (int)l_sResponse.size();
        l_sExtension = ".txt";
      }
      else if (l_sPath.substr(0, l_sPathThumbnails.size()) == l_sPathThumbnails) {
        std::string l_sTrack = l_sPath.substr(l_sPathThumbnails.size());
        if (g_mTracks.find(l_sTrack) != g_mTracks.end()) {
          l_iBufLen = g_mTracks[l_sTrack]->m_iSize;
          l_aBuffer = new char[l_iBufLen];
          memset(l_aBuffer, 0, l_iBufLen);
          memcpy(l_aBuffer, g_mTracks[l_sTrack]->m_pBuffer, l_iBufLen);
        }
      }

      if (l_aBuffer == nullptr) {
        if (l_sPath == "/")
          l_sPath = "/index.html";

        if (l_sPath.find_last_of('.') != std::string::npos)
          l_sExtension = l_sPath.substr(l_sPath.find_last_of('.'));

#ifdef _DEBUG
        std::string l_sFile = "data/html" + l_sPath;

        irr::io::IFileSystem *l_pFs = CGlobal::getInstance()->getFileSystem();

        if (l_pFs->existFile(l_sFile.c_str())) {
          irr::io::IReadFile *l_pFile = l_pFs->createAndOpenFile(l_sFile.c_str());
          if (l_pFile) {
            std::string s = l_sPath.substr(1);

            if (g_mFiles.find(s) != g_mFiles.end()) {
              SWebFileBuffer *p = g_mFiles[s];
              delete p;
            }

            SWebFileBuffer *l_pNew = new SWebFileBuffer();

            bool l_bIsTextFile = l_sExtension == ".html" || l_sExtension == ".css" || l_sExtension == ".js";

            l_pNew->m_iSize   = l_pFile->getSize();

            if (l_bIsTextFile) {
              l_pNew->m_pBuffer = new char[l_pNew->m_iSize + 1];
              memset(l_pNew->m_pBuffer, 0, l_pNew->m_iSize + 1);
            }
            else l_pNew->m_pBuffer = new char[l_pNew->m_iSize];
            l_pFile->read(l_pNew->m_pBuffer, l_pFile->getSize());

            g_mFiles[s] = l_pNew;

            l_pFile->drop();
          }
          else send404(l_sPath);
        }
        else send404(l_sPath);
#endif

        l_sPath = l_sPath.substr(1);

        if (g_mFiles.find(l_sPath) != g_mFiles.end()) {
          l_iResult = 200;

          if (l_sExtension == ".html") {
            std::string l_sFile = g_mFiles[l_sPath]->m_pBuffer;

            bool l_bReplace = false;

            while (l_sFile.find("{!") != std::string::npos) {
              std::string l_sSub = l_sFile.substr(l_sFile.find("{!") + 2);
              if (l_sSub.find("}") != std::string::npos) {
                l_bReplace = true;

                std::string l_sReplace = l_sSub.substr(0, l_sSub.find("}"));
                
                if (l_sReplace == "tracknames") {
                  l_sReplace = g_sTrackNames;
                }
                else if (l_sReplace == "profiles") {
                  l_sReplace = g_sProfileData;
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
                  l_sReplace = "\"TouchControl\"";
#endif
                }
                else if (l_sReplace == "texturepatterns") {
                  l_sReplace = getTexturePatternJS();
                }
                else if (l_sReplace == "aihelpoptions") {
                  l_sReplace = createAiHelpOptionJSON();
                }
                else if (l_sReplace.substr(0, l_sReplaceInclude.size()) == l_sReplaceInclude) {
                  std::string l_sInclude = l_sReplace.substr(l_sReplaceInclude.size());

                  if (g_mFiles.find(l_sInclude) != g_mFiles.end()) {
                    l_sReplace = g_mFiles[l_sInclude]->m_pBuffer;
                  }
                  else l_sReplace = std::string("<!-- File not found: ") + l_sInclude + " -->";
                }
                else if (l_sReplace == "aiprofiles") {
                  l_sReplace = g_sAiProfileData;
                }
                else if (l_sReplace == "championship") {
                  l_sReplace = g_sChampionshipData;
                }
                else {
                  l_sReplace = "<!-- Unkonw replacement " + l_sReplace + " -->";
                }

                std::string l_sNewFile = l_sFile.substr(0, l_sFile.find("{!")) + l_sReplace + l_sSub.substr(l_sSub.find("}") + 1);
                l_sFile = l_sNewFile;
              }
            }

            if (l_aBuffer != nullptr)
              delete[] l_aBuffer;

            l_iBufLen = (int)l_sFile.size();
            l_aBuffer = new char[l_iBufLen];
            memset(l_aBuffer, 0, l_iBufLen);
            memcpy(l_aBuffer, l_sFile.c_str(), l_iBufLen);
          }
          else {
            l_iBufLen = (int)g_mFiles[l_sPath]->m_iSize;
            l_aBuffer = new char[l_iBufLen];
            memset(l_aBuffer, 0, l_iBufLen);
            memcpy(l_aBuffer, g_mFiles[l_sPath]->m_pBuffer, l_iBufLen);
          }
        }
      }

      if (l_aBuffer != nullptr && l_iBufLen > 0) {
        l_mHeader["Content-Type"  ] = getContentType(l_sExtension);
        l_mHeader["Content-Length"] = std::to_string(l_iBufLen);
        l_mHeader["Connection"    ] = "keep-alive";

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