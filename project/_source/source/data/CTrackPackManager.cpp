// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#include <data/CTrackPackManager.h>
#include <helpers/CStringHelpers.h>
#include <platform/CPlatform.h>
#include <filesystem>
#include <map>

namespace dustbin {
  namespace data {
    CTrackPack::CTrackPack(const std::string &a_sFileName, irr::io::IFileSystem *a_pFs) : m_pFs(a_pFs), m_pArchive(nullptr), m_sFileName(a_sFileName) {
      m_sFullName = helpers::ws2s(platform::portableGetTrackPackPath()) + a_sFileName;

      for (irr::u32 i = 0; i < m_pFs->getArchiveLoaderCount(); i++) {
        if (m_pFs->getArchiveLoader(i)->isALoadableFileFormat(irr::io::EFAT_ZIP)) {
          m_pArchive = m_pFs->getArchiveLoader(i)->createArchive(m_sFullName.c_str(), true, false);
          break;
        }
      }

      if (m_pArchive != nullptr) {
        m_pFs->addFileArchive(m_pArchive);

        // We store the paths (key) where we found "track.xml" and "info.xml"
        // and two booleans (track and info) in this map
        std::map<std::string, std::tuple<bool, bool>> l_mPaths;

        const irr::io::IFileList *l_pFiles = m_pArchive->getFileList();

        printf("\n**** Content of %s\n", m_sFileName.c_str());

        for (irr::u32 i = 0; i < l_pFiles->getFileCount(); i++) {
          std::string l_sFullName = l_pFiles->getFullFileName(i).c_str();

          if (l_sFullName.find("track.xml") != std::string::npos || l_sFullName.find("info.xml") != std::string::npos) {
            std::string l_sFile = l_sFullName.find("track.xml") != std::string::npos ? "track.xml" : "info.xml";
            std::string l_sPath = l_sFullName.substr(0, l_sFullName.size() - l_sFile.size());

            if (l_mPaths.find(l_sPath) == l_mPaths.end()) {
              l_mPaths[l_sPath] = std::make_tuple(l_sFile == "track.xml", l_sFile == "info.xml");
            }
            else {
              if (l_sFile == "track.xml")
                std::get<0>(l_mPaths[l_sPath]) = true;
              else
                std::get<1>(l_mPaths[l_sPath]) = true;
            }
          }
        }

        for (auto &l_tPath: l_mPaths) {
          if (std::get<0>(l_tPath.second) == true && std::get<1>(l_tPath.second) == true) {
            std::string l_sPath = l_tPath.first.substr(std::string("data/levels/").size());
            l_sPath = l_sPath.substr(0, l_sPath.size() - 1);

            std::string l_sXml  = l_tPath.first + "info.xml";
            std::string l_sName = "";

            irr::io::IReadFile *l_pFile = m_pArchive->createAndOpenFile(l_sXml.c_str());
            if (l_pFile != nullptr) {
              irr::io::IXMLReader *l_pXml = m_pFs->createXMLReader(l_pFile);
              if (l_pXml != nullptr) {
                bool l_bElements[] = { false, false, false };

                while (l_pXml->read()) {
                  if (l_pXml->getNodeType() == irr::io::EXN_ELEMENT) {
                    std::wstring l_sNode = l_pXml->getNodeName();

                    if (l_sNode == L"marbles3")
                      l_bElements[0] = true;
                    else if (l_sNode == L"position")
                      l_bElements[1] = true;
                    else if (l_sNode == L"name") {
                      l_bElements[2] = true;
                      l_pXml->read();
                      if (l_pXml->getNodeType() == irr::io::EXN_TEXT) {
                        l_sName = helpers::ws2s(l_pXml->getNodeData());
                      }
                    }
                  }
                }

                if (l_bElements[0] == true && l_bElements[1] == true && l_bElements[2] == true && l_sName != "") {
                  printf("Track \"%s\" (%s) found in track pack \"%s\"\n", l_sName.c_str(), l_sPath.c_str(), m_sFileName.c_str());
                  m_vTracks.push_back(std::make_tuple(l_sName, l_sPath));
                }
              }
              l_pFile->drop();
            }
          }
        }

        printf("\n");
      }
    }

    CTrackPack::~CTrackPack() {
      if (m_pArchive != nullptr) {
        m_pFs->removeFileArchive(m_pArchive);
        m_pArchive->drop();
      }
    }

    /**
    * Get the number of tracks in this track pack
    * @return the number of tracks in this track pack
    */
    irr::u32 CTrackPack::getTrackCount() const {
      return (irr::u32)m_vTracks.size();
    }

    /**
    * Get the name of a track at an index
    * @param a_iIndex the index of the track
    * @return the name of a track at the given index
    */
    std::string CTrackPack::getTrackName(irr::u32 a_iIndex) const {
      if (a_iIndex < m_vTracks.size())
        return std::get<0>(m_vTracks[a_iIndex]);
      else
        return "";
    }

    /**
    * Get the path of a track at an index
    * @param a_iIndex the index of the track
    * @return the path of a track at the given index
    */
    std::string CTrackPack::getTrackPath(irr::u32 a_iIndex) const {
      if (a_iIndex < m_vTracks.size())
        return std::get<1>(m_vTracks[a_iIndex]);
      else
        return "";
    }

    /**
    * Get the file name of this track pack
    * @return the file name of this track pack
    */
    const std::string &CTrackPack::getName() {
      return m_sFileName;
    }

    /**
    * Get the filename including path for this track pack
    */
    const std::string &CTrackPack::getFileName() {
      return m_sFullName;
    }


    CTrackPackManager::CTrackPackManager(irr::io::IFileSystem *a_pFs) : m_pFs(a_pFs) {
      updateTrackPacks();
    }

    CTrackPackManager::~CTrackPackManager() {
      clearTrackPacks();
    }

    /**
    * Clear the track pack vector
    */
    void CTrackPackManager::clearTrackPacks() {
      while (m_vArchives.size() > 0) {
        CTrackPack *p = m_vArchives.front();
        m_vArchives.erase(m_vArchives.begin());
        delete p;
      }
    }

    /**
    * Update the TrackPacks
    */
    void CTrackPackManager::updateTrackPacks() {
      clearTrackPacks();

      std::string l_sPackDir = helpers::ws2s(platform::portableGetTrackPackPath()).c_str();
      irr::io::path l_sCurrentDir = m_pFs->getWorkingDirectory();
      m_pFs->changeWorkingDirectoryTo(l_sPackDir.c_str());
      irr::io::IFileList *l_pPacks = m_pFs->createFileList();
      m_pFs->changeWorkingDirectoryTo(l_sCurrentDir);

      std::string l_sExtension = ".trackpack";

      if (l_pPacks != nullptr) {
          for (irr::u32 i = 0; i < l_pPacks->getFileCount(); i++) {
              std::string l_sFile = l_pPacks->getFileName(i).c_str();
              if (l_sFile.size() > l_sExtension.size() &&
                  l_sFile.substr(l_sFile.size() - l_sExtension.size()) == l_sExtension) {
                  std::wstring l_sMessage;
                  if (verifyTrackPack(l_sPackDir + l_sFile, l_sMessage))
                      m_vArchives.push_back(new CTrackPack(l_sFile, m_pFs));
                  else
                      printf("%ls\n", l_sMessage.c_str());
              }
          }

          l_pPacks->drop();
      }
    }

    /**
    * Get the number of installed trackpacks
    * @return the number of installed trackpacks
    */
    irr::u32 CTrackPackManager::getTrackPackCount() const {
      return (irr::u32)m_vArchives.size();
    }

    /**
    * Get the track pack at an index
    * @param a_iIndex the index
    * @return the track pack at the index
    */
    CTrackPack *CTrackPackManager::getTrackPack(irr::u32 a_iIndex) const {
      if (a_iIndex < m_vArchives.size())
        return m_vArchives[a_iIndex];
      else
        return nullptr;
    }

    /**
    * Get the name of a track pack
    * @param a_iIndex the index
    * @return the name of the track pack at the index
    */
    std::string CTrackPackManager::getTrackPackName(irr::u32 a_iIndex) const {
      if (a_iIndex < m_vArchives.size())
        return m_vArchives[a_iIndex]->getName();
      else
        return "";
    }

    /**
    * Get the file paths and track names of all loaded track packs
    * @return the file paths and track names of all loaded track packs (0 == name, 1 == path)
    */
    std::vector<std::tuple<std::string, std::string>> CTrackPackManager::getTracks() {
      std::vector<std::tuple<std::string, std::string>> l_vRet;

      for (auto l_pPack: m_vArchives) {
        for (irr::u32 i = 0; i < l_pPack->getTrackCount(); i++) {
          l_vRet.push_back(std::make_tuple(l_pPack->getTrackName(i), l_pPack->getTrackPath(i)));
        }
      }

      return l_vRet;
    }

    /**
    * Delete a trackpack
    * @param a_sPack the name of the track pack to delete
    */
    void CTrackPackManager::deleteTrackPack(const std::string &a_sPack) {
      for (auto l_itPack = m_vArchives.begin(); l_itPack != m_vArchives.end(); l_itPack++) {
        if ((*l_itPack)->getName() == a_sPack) {
          std::string l_sFileName = (*l_itPack)->getFileName();
          CTrackPack *p = *l_itPack;
          m_vArchives.erase(l_itPack);
          delete p;

          std::filesystem::remove(l_sFileName);
          updateTrackPacks();

          return;
        }
      }
    }

    /**
    * Verify that the given file is a valid track pack
    * @param a_sPack file to check
    * @param a_sMessage output message
    * @return true if this is a valid trackpack, false otherwise
    */
    bool CTrackPackManager::verifyTrackPack(const std::string &a_sPack, std::wstring &a_sMessage) {
      bool l_bRet = true;

      irr::io::IFileArchive *l_pArchive = nullptr;
      for (irr::u32 i = 0; i < m_pFs->getArchiveLoaderCount(); i++) {
        if (m_pFs->getArchiveLoader(i)->isALoadableFileFormat(irr::io::EFAT_ZIP)) {
          l_pArchive = m_pFs->getArchiveLoader(i)->createArchive(a_sPack.c_str(), true, false);
          break;
        }
      }

      if (l_pArchive == nullptr) {
        a_sMessage = L"\"" + helpers::s2ws(a_sPack) + L"\" is not a valid track pack archive.";
        return false;
      }

      const irr::io::IFileList *l_pFiles = l_pArchive->getFileList();

      if (l_pFiles->getFileCount() == 0) {
        a_sMessage = L"\"" + helpers::s2ws(a_sPack) + L"\" does not contain any files.";
        l_bRet = false;
      }

      if (l_bRet) {
        printf("\n**** Content of %s\n", a_sPack.c_str());

        for (irr::u32 i = 0; i < l_pFiles->getFileCount(); i++) {
          std::string l_sFullName = l_pFiles->getFullFileName(i).c_str();
          if (!l_pFiles->isDirectory(i)) {
            std::string l_sFolder = "data/levels/";

            if (l_sFullName.substr(0, l_sFolder.size()) != l_sFolder) {
              a_sMessage = L"Invalid file \"" + helpers::s2ws(l_sFullName) + L"\" found in \"" + helpers::s2ws(a_sPack) + L"\".";
              l_bRet = false;
              break;
            }

            printf("Filename: %s\n", l_sFullName.c_str());
          }
        }
      }

      l_pArchive->drop();
      return l_bRet;
    }
  }
}