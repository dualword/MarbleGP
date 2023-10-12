#include <menu/datahandlers/IMenuDataHandler.h>
#include <helpers/CMenuLoader.h>
#include <CGlobal.h>

namespace dustbin {
  namespace menu {
    IMenuDataHandler::IMenuDataHandler() :
      m_pDevice(CGlobal::getInstance()->getIrrlichtDevice()),
      m_pDrv   (CGlobal::getInstance()->getVideoDriver   ()),
      m_pGui   (CGlobal::getInstance()->getGuiEnvironment()),
      m_pFs    (CGlobal::getInstance()->getFileSystem    ())
    {
    }

    IMenuDataHandler::~IMenuDataHandler() {
    }

    /**
    * Gets called every frame, maybe it's necessary to implement something
    */
    void IMenuDataHandler::runDataHandler() {
    }

    /**
    * Find a GUi element by name and optionally type
    * @param a_sName the name of the UI element
    * @param a_eType the type of the UI element (if irr::gui::EGUIET_COUNT (default) is passed the type is not considered)
    */
    irr::gui::IGUIElement* IMenuDataHandler::findElement(const std::string& a_sName, irr::gui::EGUI_ELEMENT_TYPE a_eType) {
      if (a_eType == irr::gui::EGUIET_COUNT)
        return helpers::findElementByName(a_sName, m_pGui->getRootGUIElement());
      else
        return helpers::findElementByNameAndType(a_sName, a_eType, m_pGui->getRootGUIElement());
    }
  }
}