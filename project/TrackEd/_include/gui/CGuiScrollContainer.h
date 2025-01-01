// (w) 2025 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>

namespace dustbin {
  namespace gui {
    /**
    * @class CGuiScrollContainer
    * @author Christian Keimel
    * This class provides a container which allows scrolling
    * in case the height of the children is larger than the
    * container's height
    */
    class CGuiScrollContainer : public irr::gui::IGUIElement {
      private:
        irr::video::IVideoDriver  *m_pDrv;
        irr::gui::IGUIScrollBar   *m_pScrollBar;
        irr::gui::IGUIEnvironment *m_pGui;

        irr::s32 m_iScrollPos;

        void updateScrollbar();

      public:
        CGuiScrollContainer(irr::gui::IGUIEnvironment *a_pGui, irr::gui::IGUIElement *a_pParent, const irr::core::recti &a_cRect);
        virtual ~CGuiScrollContainer();

        virtual void addChild(IGUIElement *a_pChild) override;

        virtual void removeChild(IGUIElement *a_pChild) override;

        virtual void draw() override;

        virtual bool OnEvent(const irr::SEvent &a_cEvent) override;
    };
  }
}
