// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

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
          children
        };

        struct SDialogElement {
          std::map<std::string, std::string> m_mAttributes;   /**< The override attributes */
          std::string m_sFontSize,                            /**< The font size (tiny, small, regular, big, huge) */
                      m_sType;                                /**< The type of the element */

          std::tuple<dustbin::enLayout, irr::core::recti> m_cPosition;  /**< A tuple for the rect of the element */

          std::vector<SDialogElement *> m_vChildren;          /**< The children of the object */
          SDialogElement* m_pParent;                          /**< The parent of the element. "nullptr" for the root element */

          SDialogElement(irr::io::IXMLReaderUTF8* a_pXml, enParseState &a_eState);
          SDialogElement(std::string a_sType, std::string a_sFontSize, dustbin::enLayout a_ePosition, const irr::core::recti& a_cRect);
          ~SDialogElement();

          irr::gui::IGUIElement* createGuiElement(CGlobal* a_pGlobal, irr::gui::IGUIElement *a_pParent);
        };

        SDialogElement *m_pRoot;

        CGlobal* m_pGlobal; /**< The dustbin global singleton */

        irr::io::IFileSystem     * m_pFs;   /**< The Irrlicht file system */
        irr::gui::IGUIEnvironment* m_pGui;  /**< The Irrlicht GUI environment */
        irr::video::IVideoDriver * m_pDrv;  /**< The Irrlicht video driver */

      public:
        /**
        * The constructor
        * @param a_sFileName the name of the XML file to load the data from
        */
        CDialog();
        ~CDialog();

        void loadDialog(const std::string& a_sFileName);

        void createUi();
    };
  }
}