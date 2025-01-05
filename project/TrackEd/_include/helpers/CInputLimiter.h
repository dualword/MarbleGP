// (w) 2025 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>
#include <string>
#include <vector>

namespace dustbin {
  namespace helpers {
    /**
    * @class CInputLimiter
    * @author Christian Keimel
    * This is a helper class which consumes key events
    * for keys which cannot be entered to an input field
    */
    class CInputLimiter {
      private:
        std::wstring m_sLimited;    /**< Some defined valid values */

        std::vector<irr::EKEY_CODE> m_vValid;   /**< Some default values which are never consumed */

        irr::gui::IGUIElement *m_pElement;    /**< The GUI element we limit the input for */

      public:
        CInputLimiter(irr::gui::IGUIElement *a_pElement, irr::io::E_ATTRIBUTE_TYPE a_eType);
        virtual ~CInputLimiter();

        bool consumeEvent(const irr::SEvent &a_cEvent);

        irr::gui::IGUIElement *getElement();
    };
  }
}