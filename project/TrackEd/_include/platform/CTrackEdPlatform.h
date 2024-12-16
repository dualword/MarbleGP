/* (w) 2021 - 22 by Dustbin::Games (Christian Keimel) - This file is licensed under the terms of the zlib license */

#include <string>

namespace dustbin {
  namespace platform {
    /**
    * This file contains functions for the Windows platform
    */

    /**
    * Get the path to the system's fonts
    * @return the path to the system's fonts
    */
    std::string portableGetFontPath();

    /**
    * Select a font using the system's font selection dialog
    * @return the selected font
    */
    std::string portableSelectFont(int& a_iFontSize);

    /**
    * Show a select folder dialog
    * @return the selected folder, empty string if the dialog was cancelled
    */
    std::string portableSelectFolder();

    /**
    * Show an open file dialog
    * @param a_sFilter the file filter string
    * @param a_sDefault the folder to open the dialog in or the file name
    * @param a_sFileName the default file name
    * @return the selected file, empty string if the dialog was cancelled
    */
    std::string portableSelectFileOpen(const wchar_t* a_sFilter, const wchar_t* a_sFolder, const wchar_t* a_sFileName);

    /**
    * Show a save file dialog
    * @param a_sFilter the file filter string
    * @param a_sDefault the folder to open the dialog in or the file name
    * @param a_sFileName the default file name
    * @return the selected file, empty string if the dialog was cancelled
    */
    std::string portableSelectFileSave(const wchar_t* a_sFilter, const wchar_t* a_sFolder, const wchar_t* a_sFileName);
  }
}