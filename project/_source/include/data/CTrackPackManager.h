// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>
#include <vector>
#include <string>
#include <tuple>

namespace dustbin {
  namespace data {
    /**
    * @class CTrackPack
    * @author Christian Keimel
    * This class holds the information for a single track pack
    */
    class CTrackPack {
      private:
        irr::io::IFileSystem  *m_pFs;         /**< The Irrlicht file system */
        irr::io::IFileArchive *m_pArchive;    /**< The Irrlicht archive for this track pack */
        std::string            m_sFileName;   /**< The file name of this track pack */
        std::string            m_sFullName;   /**< The file name including the path */

        std::vector<std::tuple<std::string, std::string>> m_vTracks;    /**< A vector with the tracks (0 == name, 1 == path) */

      public:
        CTrackPack(const std::string &a_sFileName, irr::io::IFileSystem *a_pFs);
        virtual ~CTrackPack();

        /**
        * Get the number of tracks in this track pack
        * @return the number of tracks in this track pack
        */
        irr::u32 getTrackCount() const;

        /**
        * Get the name of a track at an index
        * @param a_iIndex the index of the track
        * @return the name of a track at the given index
        */
        std::string getTrackName(irr::u32 a_iIndex) const;

        /**
        * Get the path of a track at an index
        * @param a_iIndex the index of the track
        * @return the path of a track at the given index
        */
        std::string getTrackPath(irr::u32 a_iIndex) const;

        /**
        * Get the file name of this track pack
        * @return the file name of this track pack
        */
        const std::string &getName();

        /**
        * Get the filename including path for this track pack
        */
        const std::string &getFileName();
    };

    /**
    * @class CTrackPackManager
    * @author Christian Keimel
    * This class is responsible for managing trackpacks
    */
    class CTrackPackManager {
      private:
        irr::io::IFileSystem *m_pFs;    /**< The Irrlicht file system */

        std::vector<CTrackPack *> m_vArchives;   /**< The file archives (aka trackpacks) */

        /**
        * Clear the track pack vector
        */
        void clearTrackPacks();

      public:
        CTrackPackManager(irr::io::IFileSystem *a_pFs);

        virtual ~CTrackPackManager();

        /**
        * Update the TrackPacks
        */
        void updateTrackPacks();

        /**
        * Get the number of installed trackpacks
        * @return the number of installed trackpacks
        */
        irr::u32 getTrackPackCount() const;

        /**
        * Get the track pack at an index
        * @param a_iIndex the index
        * @return the track pack at the index
        */
        CTrackPack *getTrackPack(irr::u32 a_iIndex) const;

        /**
        * Get the name of a track pack
        * @param a_iIndex the index
        * @return the name of the track pack at the index
        */
        std::string getTrackPackName(irr::u32 a_iIndex) const;

        /**
        * Get the file paths and track names of all loaded track packs
        * @return the file paths and track names of all loaded track packs (0 == name, 1 == path)
        */
        std::vector<std::tuple<std::string, std::string>> getTracks();

        /**
        * Delete a trackpack
        * @param a_sPack the name of the track pack to delete
        */
        void deleteTrackPack(const std::string &a_sPack);

        /**
        * Verify that the given file is a valid track pack
        * @param a_sPack file to check
        * @param a_sMessage output message
        * @return true if this is a valid trackpack, false otherwise
        */
        bool verifyTrackPack(const std::string &a_sPack, std::wstring &a_sMessage);
    };
  }
}
