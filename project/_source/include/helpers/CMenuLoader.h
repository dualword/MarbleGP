// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>
#include <string>
#include <vector>

namespace dustbin {
  namespace helpers {
    /**
    * This function loads a menu from an XML file to the GUI environment
    * @param a_sFile the XML file
    * @param a_pGui the GUI environment
    */
    void loadMenuFromXML(const std::string &a_sFile, irr::gui::IGUIElement *a_pParent, irr::gui::IGUIEnvironment *a_pGui);

    /**
    * Find the first element with a specific ID and type
    * @param a_iId the id of the requested GUI element
    * @param a_eType the type of the requested GUI element
    * @param a_pElement the root element to search from
    * @return the requested element, "nullptr" if no element was found
    */
    irr::gui::IGUIElement* findElementByIdAndType(irr::s32 a_iId, irr::gui::EGUI_ELEMENT_TYPE a_eType, irr::gui::IGUIElement *a_pElement);

    /**
    * Find the first element with a specific Name and type
    * @param a_sName the name of the requested GUI element
    * @param a_eType the type of the requested GUI element
    * @param a_pElement the root element to search from
    * @return the requested element, "nullptr" if no element was found
    */
    irr::gui::IGUIElement* findElementByNameAndType(const std::string &a_sName, irr::gui::EGUI_ELEMENT_TYPE a_eType, irr::gui::IGUIElement *a_pElement);

    /**
    * Find the first element with a specific Name
    * @param a_sName the name of the requested GUI element
    * @param a_pElement the root element to search from
    * @return the requested element, "nullptr" if no element was found
    */
    irr::gui::IGUIElement* findElementByName(const std::string& a_sName, irr::gui::IGUIElement* a_pElement);

    /**
    * Find all elements with a specific name and type
    * @param a_sName name of the requested elements
    * @param a_eType type of the requested elements
    * @param a_pElement the root element to search from
    * @param a_vOutput [out] vector to be filled
    */
    void findAllElementsByNameAndType(const std::string &a_sName, irr::gui::EGUI_ELEMENT_TYPE a_eType, irr::gui::IGUIElement *a_pElement, std::vector<irr::gui::IGUIElement *> &a_vOutput);
  }
}

