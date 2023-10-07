#include <menu/datahandlers/CDataHandler_Customize.h>
#include <gui/CDustbinCheckbox.h>
#include <helpers/CMenuLoader.h>
#include <gui/CSelector.h>

namespace dustbin {
  namespace menu {

    CDataHandler_Customize::CDataHandler_Customize(data::SGameSettings* a_pSettings) :
      IMenuDataHandler(),
      m_pSettings     (a_pSettings)
    {
    }

    CDataHandler_Customize::~CDataHandler_Customize() {
    }

    /**
    * This is the main method of this class. Irrlicht events
    * get forwarded and may be handled by the handler
    * @param a_cEvent the event to handle
    * @return true if the event was handled, false otherwise
    */
    bool CDataHandler_Customize::handleIrrlichtEvent(const irr::SEvent& a_cEvent) {
      bool l_bRet = false;

      return l_bRet;
    }

  }
}