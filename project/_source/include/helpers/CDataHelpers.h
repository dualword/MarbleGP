// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <data/CDataStructs.h>
#include <vector>
#include <string>
#include <map>

namespace dustbin {
  namespace helpers {
    void saveProfiles(std::vector<data::SPlayerData> &a_cProfiles);

    /**
    * Load the AI players from ai_profiles.xml
    * @param a_vAiPlayers the vector that will be filled (0 == Name, 1 == Abbreviation, 2 == Texture, 3 == Preferred Class 1, 4 == Preferred Class 2, 5 == Deviation)
    * @return true if loading succeeded, false otherwise
    */
    bool loadAiProfiles(std::vector<std::tuple<std::string, std::string, std::string, int, int, float>> &a_vAiPlayers);

    /**
    * Save the AI profiles to ai_profiles.xml
    * @param a_vAiPlayers the vector that will be filled (0 == Name, 1 == Abbreviation, 2 == Texture, 3 == Preferred Class 1, 4 == Preferred Class 2, 5 == Deviation)
    * @return true if saving succeeded, false otherwise
    */
    bool saveAiProfiles(const std::vector<std::tuple<std::string, std::string, std::string, int, int, float>> &a_vAiPlayers);

    /**
    * Get the data of all tracks
    * @return a std map with the track folder as key and the corresponding name as value
    */
    std::map<std::string, std::string> getTrackNameMap();

    /**
    * Get the possible AI help options
    * @return a vector with the possible AI help options
    */
    std::vector<std::string> getAiHelpOptions();

    /**
    * Get the string value for an AI help option
    */
    std::string getAiHelpString(data::SPlayerData::enAiHelp a_eAiHelp);

    /**
    * Get the default controller string for Keyboard game controls
    * @return the default controller string for Keyboard game controls
    */
    std::string getDefaultGameCtrl_Keyboard();

    /**
    * Get the default controller string for Gamepad game controls
    * @return the default controller string for Keyboard game controls
    */
    std::string getDefaultGameCtrl_Gamepad();

    /**
    * Get the icon folder depending on the viewport height
    * @param a_iHeight the viewport height
    * @return the corresponding icon folder
    */
    std::string getIconFolder(irr::s32 a_iHeight);

    /**
    * Load all cup definitions
    * @return a vector with all cup definitions
    */
    std::vector<data::SMarbleGpCup> loadCupDefinitions();

    /**
    * Prepare and update the global data for the next race
    * @param a_sTrack the next track identifier
    * @param a_sInfo additional race information
    * @param a_iLaps the number of laps
    */
    void prepareNextRace(const std::string &a_sTrack, const std::string &a_sInfo, int a_iLaps);

    /**
    * Read the lines of a text file
    * @param a_sPath the path of the file to read
    * @return the lines of the file in a std::vector
    */
    std::vector<std::string> readLinesOfFile(const std::string& a_sPath);
  }
}
