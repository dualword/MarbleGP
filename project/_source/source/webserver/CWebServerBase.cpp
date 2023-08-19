#include <webserver/CWebServerBase.h>
#include <helpers/CStringHelpers.h>
#include <helpers/CDataHelpers.h>
#include <platform/CPlatform.h>
#include <CGlobal.h>
#include <fstream>
#include <ios>

namespace dustbin {
  namespace webserver {
    /**
    * The constructor. Takes the port number to listen to as argument
    * @param a_iPortNo the port number to listen to 
    */
    CWebServerBase::CWebServerBase(int a_iPortNo) : m_bRunning(false), m_bStop(false), m_iPortNo(a_iPortNo), m_iError(0),
#ifdef _WINDOWS
      m_iSocket(INVALID_SOCKET)
#else
      m_iServer(-1) 
#endif
    {
#ifdef _WINDOWS
      WSADATA l_cWsa;

      m_iError = WSAStartup(MAKEWORD(2, 2), &l_cWsa);
      if (m_iError != 0) {
        printf("WSAStartup failed with code %i\n", m_iError);
        return;
      }

      struct addrinfo  l_cAddr;
      struct addrinfo *l_pData;

      ZeroMemory(&l_cAddr, sizeof(l_cAddr));
      l_cAddr.ai_family   = AF_INET;
      l_cAddr.ai_socktype = SOCK_STREAM;
      l_cAddr.ai_protocol = IPPROTO_TCP;
      l_cAddr.ai_flags    = AI_PASSIVE;

      m_iError = getaddrinfo(nullptr, std::to_string(a_iPortNo).c_str(), &l_cAddr, &l_pData);
      if (m_iError != 0) {
        printf("getaddrinfo failed with error %i\n", WSAGetLastError());
        freeaddrinfo(l_pData);
        WSACleanup();
        return;
      }

      m_iSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP  );
      if (m_iSocket == INVALID_SOCKET) {
        printf("Socket failed with error code %i\n", WSAGetLastError());
        return;
      }

      int l_iBufLen = 0;
      m_iError = setsockopt(m_iSocket, SOL_SOCKET, SO_SNDBUF, (char *)&l_iBufLen, sizeof(int));
      if (m_iError != 0) {
        printf("Error while setting socket options: %i\n", WSAGetLastError());
        return;
      }

      m_iError = bind(m_iSocket, l_pData->ai_addr, (int)l_pData->ai_addrlen);
      if (m_iError != 0) {
        printf("Bind failed with error %i\n", WSAGetLastError());
        return;
      }

      char l_sHost[255];
      memset(l_sHost, 0, 255);

      if (gethostname(l_sHost, 255) == 0) {
        printf("Host name: \"%s\"\n", l_sHost);
        m_sHostName = l_sHost;
      }

      freeaddrinfo(l_pData);
#endif
    }

    CWebServerBase::~CWebServerBase() {
#ifdef _WINDOWS
#endif
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
      return new CWebServerRequestBase(a_iSocket);
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
      closesocket(m_iSocket);
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
#ifdef _WINDOWS
        int l_iResult = listen(m_iSocket, SOMAXCONN);
        if (l_iResult == SOCKET_ERROR) {
          printf("Listen error %i\n", WSAGetLastError());
          break;
        }

        SOCKET l_iRequest = accept(m_iSocket, nullptr, nullptr);
        if (l_iRequest == INVALID_SOCKET) {
          printf("Accept error %i\n", WSAGetLastError());
          break;
        }

        IWebServerRequest *l_pRequest = createRequest(l_iRequest);
#endif
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
      printf("\t%i of %i bytes sent.\n", l_iSent, a_iLen);
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
      sendData(l_sResponse.c_str(), (int)l_sResponse.size());

      for (std::map<std::string, std::string>::const_iterator l_itHeader = a_mHeader.begin(); l_itHeader != a_mHeader.end(); l_itHeader++) {
        std::string l_sHeader = l_itHeader->first + ": " + l_itHeader->second + "\r\n";
        sendData(l_sHeader.c_str(), (int)l_sHeader.size());
      }

      sendData("\r\n", 2);
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
    CWebServerRequestBase::CWebServerRequestBase(
#ifdef _WINDOWS
      SOCKET a_iSocket
#else
      int a_iSocket
#endif
    ) : m_iSocket(a_iSocket), m_bFinished(false), m_pFs(nullptr) {
      m_pFs = CGlobal::getInstance()->getFileSystem();

      m_cThread = std::thread([this] { execute(); m_bFinished = true; detach(); delete(this); });
    }

    CWebServerRequestBase::~CWebServerRequestBase() {
      if (m_iSocket != INVALID_SOCKET) {
        // closesocket(m_iSocket);
      }
    }

    /**
    * The execution method
    */
    void CWebServerRequestBase::execute() {
      if (m_iSocket != INVALID_SOCKET) {
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

  #ifdef _WINDOWS
            int l_iThisRead = recv(m_iSocket, &c, 1, 0);
            if (l_iThisRead != 0) {
              l_iRead += l_iThisRead;
  #endif
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

          if (l_iRead > 0 && l_sMethod != "" && l_sVersion != "" && l_sMethod != "") {
            printf("Method: \"%s\", Path: \"%s\", Version: \"%s\"\n", l_sMethod.c_str(), l_sPath.c_str(), l_sVersion.c_str());

            if (l_sMethod == "GET") {
              handleGet(l_sPath, l_mHeaders);
              return;
            }
          }
          else {
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
    * Handle a GET request
    * @param a_sUrl the requested URL
    * @param a_mHeader a map with the header data
    * @return the HTTP result code 
    */
    int CWebServerRequestBase::handleGet(const std::string a_sUrl, const std::map<std::string, std::string> a_mHeader) {
      const std::string l_sPathThumbnails = "/thumbnails/";
      const std::string l_sPathResultXML  = "/championship.xml";
      const std::string l_sPathTrackNames = "/tracknames.json";

      std::string l_sPath = a_sUrl;

      std::map<std::string, std::string> l_mHeader;

      std::string l_sExtension = "";

      char *l_aBuffer = nullptr;
      int   l_iBufLen = 0;
      int   l_iResult = 0;

      if (l_sPath == l_sPathResultXML) {
        std::string l_sPath = helpers::ws2s(platform::portableGetDataPath()) + "championship_result.xml";

        irr::io::IReadFile *l_pFile = m_pFs->createAndOpenFile(l_sPath.c_str());

        if (l_pFile != nullptr) {
          l_iBufLen = l_pFile->getSize();
          l_aBuffer = new char[l_iBufLen];
          l_pFile->read(l_aBuffer, l_iBufLen);

          l_pFile->drop();

          l_sExtension = ".xml";
        }
      }
      else if (l_sPath == l_sPathTrackNames) {
        std::map<std::string, std::string> l_mTracks = helpers::getTrackNameMap();

        std::string l_sReturn = "{\n";

        for (std::map<std::string, std::string>::iterator l_itName = l_mTracks.begin(); l_itName != l_mTracks.end(); l_itName++) {
          if (l_itName != l_mTracks.begin())
            l_sReturn += ",\n";

          l_sReturn += "  \"" + l_itName->first + "\": \"" + l_itName->second + "\"";
        }

        l_sReturn += "\n}\n";

        l_iBufLen = (int)l_sReturn.size();
        l_aBuffer = new char[l_iBufLen];

        memcpy(l_aBuffer, l_sReturn.c_str(), l_iBufLen);

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

        std::string l_sRelativePath = "data/html/" + l_sPath;

        irr::io::IReadFile *l_pFile = m_pFs->createAndOpenFile(l_sRelativePath.c_str());

        if (l_pFile != nullptr) {
          l_aBuffer = new char[l_pFile->getSize()];
          l_iBufLen = l_pFile->getSize();

          l_pFile->read(l_aBuffer, l_iBufLen);
          l_pFile->drop();

          l_iResult = 200;
        }
      }

      if (l_aBuffer != nullptr && l_iBufLen > 0) {
        l_mHeader["Content-Type"  ] = getContentType(l_sExtension);
        l_mHeader["Content-Length"] = std::to_string(l_iBufLen);
        l_mHeader["Connection"    ] = "close";

        char *l_aToDelete = l_aBuffer;

        printf("File \"%s\": Size = %i, Type = \"%s\" (Result %i)\n", l_sPath.c_str(), l_iBufLen, l_sExtension.c_str(), l_iResult);

        sendHeader(l_iResult, l_mHeader);
        while (l_iBufLen > 0) {
          sendData(l_aBuffer, l_iBufLen > 1024 ? 1024 : l_iBufLen);
          if (l_iBufLen > 1024) {
            l_iBufLen -= 1024;
            l_aBuffer += 1024;
          }
          else l_iBufLen = 0;
        }
        send(m_iSocket, "", 0, 0);

        delete l_aToDelete;

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
      return 404;
    }

    /**
    * Handle a DELETE request
    * @param a_sUrl the requested URL
    * @param a_mHeader a map with the header data
    * @return the HTTP result code 
    */
    int CWebServerRequestBase::handleDelete(const std::string a_sUrl, const std::map<std::string, std::string> a_mHeader) {
      return 404;
    }

  }
}