// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>
#include <string>

namespace dustbin {
  namespace helpers {
    /**
    * This function loads a menu from an XML file to the GUI environment
    * @param a_sFile the XML file
    * @param a_pGui the GUI environment
    */
    void loadMenuFromXML(const std::string &a_sFile, irr::gui::IGUIElement *a_pParent, irr::gui::IGUIEnvironment *a_pGui);
  }
}

