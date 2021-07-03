// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

#include <lua.hpp>
#include <string>
#include <vector>

namespace dustbin {
  class CGlobal;

  namespace lua {
    /**
    * @class CLuaGuiItem
    * @author Christian Keimel
    * This class is a binding for all used GUI items to LUA. It provides setter and getter methods
    * for all of the available Irrlicht GUI elements (all that are used in the game). The setters
    * return "bool" to indicate whether or not setting the attribute was possible
    */
    class CLuaGuiItem {
      private:
        irr::gui::IGUIElement* m_pElement;
        CGlobal              * m_pGlobal;

      public:
        CLuaGuiItem(irr::gui::IGUIElement* a_pElement);
        CLuaGuiItem(const CLuaGuiItem& a_cOther);
        virtual ~CLuaGuiItem();

        /**
        * This method gives read access to all serialized properties. This is a universal getter that 
        * is slower than the specific getters
        * @param a_sName Name of the requested property
        * @return the property value
        */
        std::string getProperty(const std::string& a_sName);

        /**
        * Modify a property. This is a universal setter which is slower than the specific setters
        * @param a_sName the name of the property
        * @param a_sValue the new value of the property
        * @return true if the property was found and modified, false otherwise
        */
        bool setProperty(const std::string& a_sName, const std::string& a_sValue);

        /**
        * Get a list of all available properties
        * @return a list of all available properties
        */
        std::vector<std::string> getProperties();

        /**
        * Change the visibility of the item
        * @param a_bVisible the new visibility
        */
        void setVisible(bool a_bVisible);

        /**
        * Is this item visible?
        * @return "true" if the item is visible
        */
        bool isVisible(bool a_bVisible);

        /**
        * Set the name of the item
        * @param a_sName the new name of the item
        */
        void setName(const std::string& a_sName);

        /**
        * Get the name of the item
        * @return the name of the item
        */
        std::string getName();

        /**
        * Set the id of the item
        * @param a_iId the new id of the item
        */
        void setId(int a_iId);

        /**
        * Get the id of the item
        * @return the id of the item
        */
        int getId();

        /**
        * Set the caption of the element
        * @param a_sText the new value
        */
        void setText(const std::string& a_sText);

        /**
        * Get the text of the item
        * @return the text of the item
        */
        std::string getText();

        /**
        * Clear the items of the combobox
        * @return "true" if the element is a combobox
        */
        bool clearItems();

        /**
        * Add an item to the combobox
        * @return "true" if the element is a combobox
        */
        bool addItem(const std::string& a_sItem);

        /**
        * Get an item from the combobox
        * @return the item or an empty string if the index is out of range or the item is not a combobox
        */
        std::string getItem(int a_iIndex);

        /**
        * Get the selected item of the combobox
        * @return the selected item of the combobox (-1 if no item is selected)
        */
        int getSelected();

        /**
        * Set the selected item of the combobox
        * @param a_iIndex the new selected item
        * @return "true" if the element is a combobox
        */
        bool setSelected(int a_iIndex);

        /**
        * Is the checkbox checked?
        * @return true if the checkbox is checked?
        */
        bool isChecked();

        /**
        * (Un)check the checkbox
        * @param a_bChecked the new value
        * @return "true" if the item is a checkbox
        */
        bool setChecked(bool a_bChecked);

        /**
        * Get numerical value (available for SpinBox and scrollbar)
        * @return the numerical value
        */
        float getValue();

        /**
        * Get numerical value as integer (available for SpinBox and scrollbar)
        * @return the numerical value
        */
        int getValueInt();

        /**
        * Set the numerical value (available for SpinBox and scrollbar)
        * @param a_fValue the new value
        * @return "true" if the item is a spinbox
        */
        bool setValue(float a_fValue);

        /**
        * Set the numerical value (available for SpinBox and scrollbar) as integer
        * @param a_fValue the new value
        * @return "true" if the item is a spinbox
        */
        bool setValueInt(int a_iValue);

        /**
        * Set the image
        * @param a_sImage the new image as a string
        * @return "true" if the image could be set
        */
        bool setImage(const std::string a_sImage);

        /**
        * Register the class to a LUA state
        */
        static void registerClass(lua_State* a_pState);
    };
  }
}
