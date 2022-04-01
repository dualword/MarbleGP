// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>
#include <vector>

namespace dustbin {
  namespace gui {
    /**
    * @class CGuiAiDebug
    * @author Christian Keimel
    * This is an UI element that shows all of the data
    * the AI is using for debug purposes
    */
    class CGuiAiDebug : public irr::gui::IGUIElement {
      private:
        irr::gui::IGUIEnvironment *m_pGui;
        irr::video::IVideoDriver  *m_pDrv;

        std::vector<irr::core::line2df> m_vLines;

      public:
        CGuiAiDebug(irr::gui::IGUIEnvironment *a_pGui);
        virtual ~CGuiAiDebug();

        virtual void draw();

        void clearLines();
        void addLine(const irr::core::line2df &a_cLine);
    };
  }
}
