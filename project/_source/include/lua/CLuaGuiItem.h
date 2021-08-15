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
        * Get the selected item of the combobox
        * @return the selected item of the combobox
        */
        std::string getSelectedItem();

        /**
        * Select an item by content. Will clear the selection if the item is not found
        * @param a_sItem the item to select
        */
        void setSelectedItem(const std::string& a_sItem);

        /**
        * Set the selected item of the combobox
        * @param a_iIndex the new selected item
        * @return "true" if the element is a combobox
        */
        bool setSelected(int a_iIndex);

        /**
        * Enable or disable the GUI item
        * @param a_bEnabled the new enabled flag
        * @return "true" if an element was changed, false otherwise
        */
        bool setEnabled(bool a_bEnabled);

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
        * @param a_sTextureName the file name to use for the texture internally. If present the old texture is removed.
        * @return "true" if the image could be set
        */
        bool setImage(const std::string a_sImage);

        /**
        * Get the number of children of this item
        * @return the number of children of this item
        */
        int getChildCount();

        /**
        * Get a child of this item
        * @param a_iIndex index of the child (LUA-Style: counting starts at "1" and ends at "childcount")
        * @return a child of this item
        */
        CLuaGuiItem getChild(int a_iIndex);

        /**
        * Set the background color of an item
        * @param a_pState the LUA state. The LUA table "SColor" is pulled from the stack
        * @return "0" as no return values are expected
        */
        int setBackgroundColor(lua_State* a_pState);

        /**
        * Set the tooltip of an icon
        * @param a_sTooltip the new tolltip text
        */
        void setTooltip(const std::string& a_sToolTip);

        /**
        * Get the Irrlicht UI element of the instance
        * @return the Irrlicht UI element of the instance
        */
        irr::gui::IGUIElement* getElement();

        /**
        * Register the class to a LUA state
        */
        static void registerClass(lua_State* a_pState);
    };
  }
}
