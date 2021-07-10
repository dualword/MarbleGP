// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

#include <controller/CControllerBase.h>
#include <gui/CMenuBackground.h>
#include <string>
#include <map>

namespace dustbin {
  namespace gui {
    const int g_ControllerUiId = MAKE_IRR_ID('d', 'g', 'c', 'u');
    const irr::c8 g_ControllerUiName[] = "ControllerUi";

    /**
    * @class CControllerUi
    * @author Christian Keimel
    * This class provides a UI interface to configure controls
    */
    class CControllerUi : public gui::CMenuBackground, public controller::CControllerBase {
      private:
        irr::gui::IGUIEnvironment  * m_pGui;
        irr::gui::IGUIElement      * m_pParent;
        irr::gui::ICursorControl   * m_pCursor;

        std::string m_sConfigXml;
        std::string m_sHeadline;
        std::wstring m_sReturn;

        irr::gui::IGUIElement *m_pSelectedOne,  /**< The first selected control static text */
                              *m_pSelectedTwo,  /**< The second selected control static text*/
                              *m_pHoveredOne,   /**< The first hovered control static text */
                              *m_pHoveredTwo;   /**< The second hovered control static text */

        bool m_bMouseDown;

        std::map<irr::gui::IGUIElement*, irr::gui::IGUIElement*> m_mTextPairs;  /**< A map that links the first and second control static texts */

        

        std::map<irr::gui::IGUIElement*, std::vector<controller::CControllerBase::SCtrlInput>::iterator> m_mTextControls; /**< A map that links static texts to control items */

        std::map<std::vector<controller::CControllerBase::SCtrlInput>::iterator, irr::gui::IGUIStaticText*> m_mControlText; /**< The label for the controllers */

        irr::gui::IGUIFont* m_pFont;

        std::map<irr::gui::IGUIStaticText*, controller::CControllerBase::SCtrlInput*> m_mElements;

        std::vector<controller::CControllerBase::SCtrlInput>::iterator m_itHovered,   /**< The hovered control */
                                                                       m_itSelected;  /**< The selected control */

        void elementEvent(irr::gui::IGUIElement* a_pElement, bool a_bEnter);

        void updateConfigXml();

      public:
        CControllerUi(irr::gui::IGUIElement *a_pParent);
        ~CControllerUi();

        /**
        * This method builds the UI for the controller
        * @param a_pParent the parent element. The outbox of this element will be used for the UI
        */
        void buildUi(irr::gui::IGUIElement *a_pParent);

        virtual bool OnEvent(const irr::SEvent& a_cEvent);

        /**
        * Get the configuration of the controller as XML string
        * @return the configuration of the controller
        */
        std::string getControllerString();

        virtual irr::gui::EGUI_ELEMENT_TYPE getType();

        /**
        * The "setText" method is abused to pass the configuration XML string to the UI
        * @param a_pText the new XML string
        */
        virtual void setText(const wchar_t* a_pText);

        /**
        * Change the font for the configuration dialog
        */
        void setFont(irr::gui::IGUIFont* a_pFont);

        virtual void serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const;
        virtual void deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions);
    };
  } // namespace controller 
} // namespace dustbin