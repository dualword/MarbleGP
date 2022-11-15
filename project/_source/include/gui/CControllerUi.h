// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>

#include <controller/IJoystickEventHandler.h>
#include <controller/CControllerBase.h>
#include <gui/CMenuBackground.h>
#include <state/IState.h>
#include <string>
#include <vector>
#include <tuple>
#include <map>

namespace dustbin {
  namespace menu {
    class IMenuManager;   /**< Forward declaration of the menu manager */
  }
  namespace gui {
    const int g_ControllerUiId = MAKE_IRR_ID('d', 'g', 'c', 'u');
    const irr::c8 g_ControllerUiName[] = "ControllerUi";

    class CDustbinScrollPane; /**< Forward declaration of the scroll pane */

    /**
    * @class CControllerUi
    * @author Christian Keimel
    * This class provides a UI interface to configure controls
    */
    class CControllerUi : public gui::CMenuBackground, public controller::CControllerBase, public controller::IJoystickEventHandler {
      private:
        enum class enControls {
          Forward,
          Backward,
          Left,
          Right,
          Brake,
          Rearview,
          Respawn,
          Pause,
          Cancel
        };

        irr::gui::ICursorControl *m_pCursor;
        irr::gui::IGUIFont       *m_pFont;
        menu::IMenuManager       *m_pMenuMgr;
        std::string               m_sSelected;    /**< The selected controller type */
        std::string               m_sConfigData;  /**< The serialized controller config string */
        irr::core::recti          m_cDraw;        /**< Draw rect for the image */
        irr::s32                  m_iFontHeight;  /**< Height of the font */

        std::map<std::string, std::tuple<irr::video::ITexture *, irr::core::recti>> m_mImages;    /**< The images. Key == image name (no path, no extension), value == (img ptr, source rect) */

        std::map<enControls, irr::core::vector2di> m_mTextPositions;

        /**
        * Get a readable string of the set controls
        * @return a readable string of the set controls
        */
        std::wstring getControlText(CControllerBase::SCtrlInput *a_pCtrl);

      public:
        CControllerUi(irr::gui::IGUIElement *a_pParent);
        ~CControllerUi();

        /**
        * This method builds the UI for the controller
        * @param a_pParent the parent element. The outbox of this element will be used for the UI
        */
        void buildUi(irr::gui::IGUIElement *a_pParent);

        virtual bool update(const irr::SEvent& a_cEvent) override;

        virtual bool OnEvent(const irr::SEvent &a_cEvent) override;

        /**
        * Get the configuration of the controller as XML string
        * @return the configuration of the controller
        */
        std::string getControllerString();

        virtual irr::gui::EGUI_ELEMENT_TYPE getType() override;

        /**
        * The "setText" method is abused to pass the configuration XML string to the UI
        * @param a_pText the new XML string
        */
        virtual void setText(const wchar_t* a_pText) override;

        /**
        * Change the font for the configuration dialog
        */
        void setFont(irr::gui::IGUIFont* a_pFont);

        /**
        * Is currently editing an item active? This will block the menu controller
        */
        bool isEditing();

        void setMenuManager(menu::IMenuManager *a_pMenuManager);

        virtual bool OnJoystickEvent(const irr::SEvent& a_cEvent) override;

        virtual void draw() override;

        virtual void serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const override;
        virtual void deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) override;
    };
  } // namespace controller 
} // namespace dustbin