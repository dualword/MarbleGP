// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

#include <lua.hpp>

#include <lua/CLuaGuiItem.h>
#include <CGlobal.h>
#include <string>
#include <vector>
#include <tuple>
#include <map>

namespace dustbin {
  class CGlobal;

  namespace gui {
    /**
    * @class CDialog
    * @author Christian Keimel
    * This is the class that handles dialogs which get loaded from a XML file
    */
    class CDialog {
      private:
        enum class enParseState {
          root,
          dialog,
          element,
          attributes,
          custom,
          children
        };

        struct SDialogElement {
          std::map<std::string, std::string> m_mAttributes,   /**< The override attributes */
                                             m_mCustom;       /**< Custom attributes that are not handled by Irrlicht directly */
          std::string m_sFontSize,                            /**< The font size (tiny, small, regular, big, huge) */
                      m_sToolTip,                             /**< The tooltip of the gui item */
                      m_sType;                                /**< The type of the element */

          irr::gui::IGUIElement* m_pElement;                  /**< The created UI element */

          std::tuple<dustbin::enLayout, irr::core::recti> m_cPosition;  /**< A tuple for the rect of the element */

          std::vector<SDialogElement *> m_vChildren;          /**< The children of the object */
          SDialogElement* m_pParent;                          /**< The parent of the element. "nullptr" for the root element */

          SDialogElement(irr::io::IXMLReaderUTF8* a_pXml, enParseState &a_eState);
          SDialogElement(std::string a_sType, std::string a_sFontSize, dustbin::enLayout a_ePosition, const irr::core::recti& a_cRect);
          ~SDialogElement();

          void parse(irr::io::IXMLReaderUTF8* a_pXml, enParseState& a_eState);

          irr::gui::IGUIElement* createGuiElement(CGlobal* a_pGlobal, irr::gui::IGUIElement *a_pParent);

          void findDefaultCancelButtons(irr::gui::IGUIElement** a_pDefault, irr::gui::IGUIElement** a_pCancel);
        };

        SDialogElement *m_pRoot;

        CGlobal* m_pGlobal; /**< The dustbin global singleton */

        irr::io::IFileSystem     * m_pFs;   /**< The Irrlicht file system */
        irr::gui::IGUIEnvironment* m_pGui;  /**< The Irrlicht GUI environment */
        irr::video::IVideoDriver * m_pDrv;  /**< The Irrlicht video driver */

        irr::gui::IGUIElement* m_pDefault,                  /**< The "default" button activated with the "Enter" key */
                             * m_pCancel;                   /**< The "cancel" button activated with the "ESC" key */        

        /**
        * Find a GUI element from it's name or id
        * @param a_sName the name of the queried element (empty string means that the ID is used)
        * @param a_iId the id of the queried element (-1 means the name is used)
        * @param a_pParent the current element
        * @return the element
        */
        irr::gui::IGUIElement* findElement(const std::string a_sName, int a_iId, irr::gui::IGUIElement *a_pParent);
    public:
        /**
        * The constructor
        * @param a_sFileName the name of the XML file to load the data from
        */
        CDialog(lua_State *a_pState);
        ~CDialog();

        /**
        * Load a dialog defined in a XML file
        * @param a_sFileName the dialog to load
        */
        void loadDialog(const std::string& a_sFileName);

        void createUi();
        void clear();
        void addLayoutRaster();

        /**
        * This is a callback that is envoked when "Enter" was pressed. 
        * @return the "Default" UI element, nullptr if none was defined
        */
        irr::gui::IGUIElement *defaultClicked();

        /**
        * This is a callback that is envoked when "ESC" was pressed.
        * @return the "Cancel" UI element, nullptr if none was defined
        */
        irr::gui::IGUIElement* cancelClicked();

        /**
        * Get a GUI item from it's name
        * @param a_sName the name of the item
        */
        lua::CLuaGuiItem getItemFromName(const std::string& a_sName);

        /**
        * Get a GUI item from it's id
        * @param a_iId the id of the item
        */
        lua::CLuaGuiItem getItemFromId(int a_iId);

        /**
        * Change the Z-Layer of the dialog for the menu controller
        * @param a_iZLayer the new Z-Value
        */
        void setZLayer(int a_iZLayer);
    };
  }
}