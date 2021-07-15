// (w) 2021 by Dustbin::Games / Christian Keimel
#include <lua/CLuaGuiItem.h>

#include <gui/CDustbinCheckbox.h>
#include <LuaBridge/LuaBridge.h>
#include <platform/CPlatform.h>
#include <gui/CSelector.h>
#include <CGlobal.h>

namespace dustbin {
  namespace lua {
    CLuaGuiItem::CLuaGuiItem(irr::gui::IGUIElement* a_pElement) {
      m_pElement = a_pElement;
      m_pGlobal  = CGlobal::getInstance();
    }

    CLuaGuiItem::CLuaGuiItem(const CLuaGuiItem& a_cOther) {
      m_pElement = a_cOther.m_pElement;
      m_pGlobal  = a_cOther.m_pGlobal;
    }

    CLuaGuiItem::~CLuaGuiItem() {
    }

    /**
    * This method gives read access to all serialized properties. This is a universal getter that
    * is slower than the specific getters
    * @param a_sName Name of the requested property
    * @return the property value
    */
    std::string CLuaGuiItem::getProperty(const std::string& a_sName) {
      if (m_pElement != nullptr) {
        irr::io::IAttributes* l_pAttr = m_pGlobal->getFileSystem()->createEmptyAttributes();
        std::string l_sValue = "";

        m_pElement->serializeAttributes(l_pAttr);

        if (l_pAttr->getAttributeAsString(a_sName.c_str()) != nullptr)
          l_sValue = l_pAttr->getAttributeAsString(a_sName.c_str()).c_str();

        l_pAttr->clear();
        l_pAttr->drop();

        return l_sValue;
      }
      else return "";
    }

    /**
    * Modify a property. This is a universal setter which is slower than the specific setters
    * @param a_sName the name of the property
    * @param a_sValue the new value of the property
    * @return true if the property was found and modified, false otherwise
    */
    bool CLuaGuiItem::setProperty(const std::string& a_sName, const std::string& a_sValue) {
      if (m_pElement != nullptr) {
        irr::io::IAttributes* l_pAttr = m_pGlobal->getFileSystem()->createEmptyAttributes();

        m_pElement->serializeAttributes(l_pAttr);

        // ToDo: respect attribute types
        l_pAttr->setAttribute(a_sName.c_str(), a_sValue.c_str());

        m_pElement->deserializeAttributes(l_pAttr);

        l_pAttr->clear();
        l_pAttr->drop();

        return true;
      }
      else return false;
    }

    /**
    * Get a list of all available properties
    * @return a list of all available properties
    */
    std::vector<std::string> CLuaGuiItem::getProperties() {
      std::vector<std::string> l_vRet;

      if (m_pElement != nullptr) {
        irr::io::IAttributes* l_pAttr = m_pGlobal->getFileSystem()->createEmptyAttributes();
        m_pElement->serializeAttributes(l_pAttr);

        for (unsigned i = 0; i < l_pAttr->getAttributeCount(); i++) {
          l_vRet.push_back(l_pAttr->getAttributeName((irr::s32)i));
        }

        l_pAttr->clear();
        l_pAttr->drop();
      }

      return l_vRet;
    }

    /**
    * Change the visibility of the item
    * @param a_bVisible the new visibility
    */
    void CLuaGuiItem::setVisible(bool a_bVisible) {
      if (m_pElement != nullptr)
        m_pElement->setVisible(a_bVisible);
    }

    /**
    * Is this item visible?
    * @return "true" if the item is visible
    */
    bool CLuaGuiItem::isVisible(bool a_bVisible) {
      return m_pElement != nullptr ? m_pElement->isVisible() : false;
    }

    /**
    * Set the name of the item
    * @param a_sName the new name of the item
    */
    void CLuaGuiItem::setName(const std::string& a_sName) {
      if (m_pElement != nullptr)
        m_pElement->setName(a_sName.c_str());
    }

    /**
    * Get the name of the item
    * @return the name of the item
    */
    std::string CLuaGuiItem::getName() {
      return m_pElement != nullptr ? m_pElement->getName() : "";
    }

    /**
    * Set the id of the item
    * @param a_iId the new id of the item
    */
    void CLuaGuiItem::setId(int a_iId) {
      if (m_pElement != nullptr)
        m_pElement->setID((irr::s32)a_iId);
    }

    /**
    * Get the id of the item
    * @return the id of the item
    */
    int CLuaGuiItem::getId() {
      return m_pElement != nullptr ? m_pElement->getID() : -1;
    }

    /**
    * Set the caption of the element
    * @param a_sText the new value
    */
    void CLuaGuiItem::setText(const std::string& a_sText) {
      if (m_pElement != nullptr)
        m_pElement->setText(std::wstring(a_sText.begin(), a_sText.end()).c_str());
    }

    /**
    * Get the text of the item
    * @return the text of the item
    */
    std::string CLuaGuiItem::getText() {
      std::string s = "";

      if (m_pElement != nullptr) {
        std::wstring ws = m_pElement->getText();
        s = platform::ws2s(ws);
      }

      return s;
    }

    /**
    * Clear the items of the combobox
    * @return "true" if the element is a combobox
    */
    bool CLuaGuiItem::clearItems() {
      if (m_pElement != nullptr && m_pElement->getType() == irr::gui::EGUIET_COMBO_BOX) {
        reinterpret_cast<irr::gui::IGUIComboBox*>(m_pElement)->clear();
        return true;
      }
      else if (m_pElement != nullptr && m_pElement->getType() == gui::g_SelectorId) {
        reinterpret_cast<gui::CSelector*>(m_pElement)->clear();
        return true;
      }
      else return false;
    }

    /**
    * Add an item to the combobox
    * @return "true" if the element is a combobox
    */
    bool CLuaGuiItem::addItem(const std::string& a_sItem) {
      if (m_pElement != nullptr && m_pElement->getType() == irr::gui::EGUIET_COMBO_BOX) {
        reinterpret_cast<irr::gui::IGUIComboBox*>(m_pElement)->addItem(platform::s2ws(a_sItem).c_str());
        return true;
      }
      else if (m_pElement != nullptr && m_pElement->getType() == gui::g_SelectorId) {
        reinterpret_cast<gui::CSelector*>(m_pElement)->addItem(platform::s2ws(a_sItem).c_str());
        return true;
      }
      else return false;
    }

    /**
    * Get an item from the combobox
    * @return the item or an empty string if the index is out of range or the item is not a combobox
    */
    std::string CLuaGuiItem::getItem(int a_iIndex) {
      if (m_pElement != nullptr && m_pElement->getType() == irr::gui::EGUIET_COMBO_BOX) {
        irr::gui::IGUIComboBox* p = reinterpret_cast<irr::gui::IGUIComboBox*>(m_pElement);
        if (a_iIndex >= 0 && a_iIndex < (int)p->getItemCount()) {
          std::wstring s = p->getItem((unsigned)a_iIndex);
          return platform::ws2s(s);
        }
        else return "";
      }
      else if (m_pElement != nullptr && m_pElement->getType() == gui::g_SelectorId) {
        return platform::ws2s(reinterpret_cast<gui::CSelector*>(m_pElement)->getItem(a_iIndex));
      }
      else return "";
    }

    /**
    * Get the selected item of the combobox
    * @return the selected item of the combobox (-1 if no item is selected)
    */
    int CLuaGuiItem::getSelected() {
      if (m_pElement != nullptr && m_pElement->getType() == irr::gui::EGUIET_COMBO_BOX) {
        return reinterpret_cast<irr::gui::IGUIComboBox*>(m_pElement)->getSelected();
      }
      else if (m_pElement != nullptr && m_pElement->getType() == gui::g_SelectorId) {
        return reinterpret_cast<gui::CSelector*>(m_pElement)->getSelected();
      }
      else return false;
    }

    /**
    * Set the selected item of the combobox
    * @param a_iIndex the new selected item
    * @return "true" if the element is a combobox
    */
    bool CLuaGuiItem::setSelected(int a_iIndex) {
      if (m_pElement != nullptr && m_pElement->getType() == irr::gui::EGUIET_COMBO_BOX) {
        reinterpret_cast<irr::gui::IGUIComboBox*>(m_pElement)->setSelected(a_iIndex);
        return true;
      }
      else if (m_pElement != nullptr && m_pElement->getType() == gui::g_SelectorId) {
        reinterpret_cast<gui::CSelector*>(m_pElement)->setSelected(a_iIndex);
        return true;
      }
      else return false;
    }

    /**
    * Is the checkbox checked?
    * @return true if the checkbox is checked?
    */
    bool CLuaGuiItem::isChecked() {
      if (m_pElement != nullptr && m_pElement->getType() == irr::gui::EGUIET_CHECK_BOX) {
        return reinterpret_cast<irr::gui::IGUICheckBox*>(m_pElement)->isChecked();
      }
      else if (m_pElement != nullptr && m_pElement->getType() == (irr::gui::EGUI_ELEMENT_TYPE)gui::g_DustbinCheckboxId) {
        return reinterpret_cast<gui::CDustbinCheckbox*>(m_pElement)->isChecked();
      }
      else return false;
    }

    /**
    * (Un)check the checkbox
    * @param a_bChecked the new value
    * @return "true" if the item is a checkbox
    */
    bool CLuaGuiItem::setChecked(bool a_bChecked) {
      if (m_pElement != nullptr && m_pElement->getType() == irr::gui::EGUIET_CHECK_BOX) {
        reinterpret_cast<irr::gui::IGUICheckBox*>(m_pElement)->setChecked(a_bChecked);
        return true;
      }
      else if (m_pElement != nullptr && m_pElement->getType() == (irr::gui::EGUI_ELEMENT_TYPE)gui::g_DustbinCheckboxId) {
        reinterpret_cast<gui::CDustbinCheckbox*>(m_pElement)->setChecked(a_bChecked);
        return true;
      }
      else return false;
    }

    /**
    * Get numerical value (available for SpinBox and scrollbar)
    * @return the numerical value
    */
    float CLuaGuiItem::getValue() {
      if (m_pElement != nullptr) {
        if (m_pElement->getType() == irr::gui::EGUIET_SPIN_BOX)
          return reinterpret_cast<irr::gui::IGUISpinBox*>(m_pElement)->getValue();
        else if (m_pElement->getType() == irr::gui::EGUIET_SCROLL_BAR)
          return (float)reinterpret_cast<irr::gui::IGUIScrollBar*>(m_pElement)->getPos();
        else
          return 0.0;
      }
      else return 0.0;
    }

    /**
    * Get numerical value as integer (available for SpinBox and scrollbar)
    * @return the numerical value
    */
    int CLuaGuiItem::getValueInt() {
      if (m_pElement != nullptr) {
        if (m_pElement->getType() == irr::gui::EGUIET_SPIN_BOX)
          return (int)reinterpret_cast<irr::gui::IGUISpinBox*>(m_pElement)->getValue();
        else if (m_pElement->getType() == irr::gui::EGUIET_SCROLL_BAR)
          return reinterpret_cast<irr::gui::IGUIScrollBar*>(m_pElement)->getPos();
        else
          return 0;
      }
      else return 0;
    }

    /**
    * Set the numerical value (available for SpinBox and scrollbar)
    * @param a_fValue the new value
    * @return "true" if the item is a spinbox
    */
    bool CLuaGuiItem::setValue(float a_fValue) {
      if (m_pElement != nullptr) {
        if (m_pElement->getType() == irr::gui::EGUIET_SPIN_BOX) {
          reinterpret_cast<irr::gui::IGUISpinBox*>(m_pElement)->setValue((irr::f32)a_fValue);
          
          irr::SEvent l_cEvent;
          l_cEvent.EventType = irr::EET_GUI_EVENT;
          l_cEvent.GUIEvent.EventType = irr::gui::EGET_SPINBOX_CHANGED;
          l_cEvent.GUIEvent.Caller = m_pElement;
          l_cEvent.GUIEvent.Element = m_pElement;
          m_pGlobal->OnEvent(l_cEvent);

          return true;
        }
        else if (m_pElement->getType() == irr::gui::EGUIET_SCROLL_BAR) {
          irr::gui::IGUIScrollBar* p = reinterpret_cast<irr::gui::IGUIScrollBar*>(m_pElement);

          if (a_fValue > p->getMax())
            p->setPos(p->getMax());
          else if (a_fValue < p->getMin())
            p->setPos(p->getMin());
          else
            p->setPos((irr::s32)a_fValue);

          irr::SEvent l_cEvent;
          l_cEvent.EventType = irr::EET_GUI_EVENT;
          l_cEvent.GUIEvent.EventType = irr::gui::EGET_SCROLL_BAR_CHANGED;
          l_cEvent.GUIEvent.Caller = m_pElement;
          l_cEvent.GUIEvent.Element = m_pElement;
          m_pGlobal->OnEvent(l_cEvent);

          return true;
        }
        else return false;
      }
      else return false;
    }

    /**
    * Set the numerical value (available for SpinBox and scrollbar) as integer
    * @param a_fValue the new value
    * @return "true" if the item is a spinbox
    */
    bool CLuaGuiItem::setValueInt(int a_iValue) {
      if (m_pElement != nullptr) {
        if (m_pElement->getType() == irr::gui::EGUIET_SPIN_BOX) {
          reinterpret_cast<irr::gui::IGUISpinBox*>(m_pElement)->setValue((irr::f32)a_iValue);
          return true;
        }
        else if (m_pElement->getType() == irr::gui::EGUIET_SCROLL_BAR) {
          reinterpret_cast<irr::gui::IGUIScrollBar*>(m_pElement)->setPos((irr::s32)a_iValue);
          return true;
        }
        else return false;
      }
      else return false;
    }

    /**
    * Set the image
    * @param a_sImage the new image as a string
    * @return "true" if the image could be set
    */
    bool CLuaGuiItem::setImage(const std::string a_sImage) {
      if (m_pElement != nullptr && m_pElement->getType() == irr::gui::EGUIET_IMAGE) {
        reinterpret_cast<irr::gui::IGUIImage*>(m_pElement)->setImage(m_pGlobal->createTexture(a_sImage));
        return true;
      }
      else return false;
    }

    /**
    * Register the class to a LUA state
    */
    void CLuaGuiItem::registerClass(lua_State* a_pState) {
      luabridge::getGlobalNamespace(a_pState)
        .beginClass<CLuaGuiItem>("GuiItem")
          .addFunction("getproperty"  , &CLuaGuiItem::getProperty)
          .addFunction("setproperty"  , &CLuaGuiItem::setProperty)
          .addFunction("getproperties", &CLuaGuiItem::getProperties)
          .addFunction("setvisible"   , &CLuaGuiItem::setVisible)
          .addFunction("isvisible"    , &CLuaGuiItem::isVisible)
          .addFunction("setname"      , &CLuaGuiItem::setName)
          .addFunction("getname"      , &CLuaGuiItem::getName)
          .addFunction("setid"        , &CLuaGuiItem::setId)
          .addFunction("getid"        , &CLuaGuiItem::getId)
          .addFunction("settext"      , &CLuaGuiItem::setText)
          .addFunction("gettext"      , &CLuaGuiItem::getText)
          .addFunction("clearitems"   , &CLuaGuiItem::clearItems)
          .addFunction("additem"      , &CLuaGuiItem::addItem)
          .addFunction("getitem"      , &CLuaGuiItem::getItem)
          .addFunction("getselected"  , &CLuaGuiItem::getSelected)
          .addFunction("setselected"  , &CLuaGuiItem::setSelected)
          .addFunction("ischecked"    , &CLuaGuiItem::isChecked)
          .addFunction("setchecked"   , &CLuaGuiItem::setChecked)
          .addFunction("getvalue"     , &CLuaGuiItem::getValue)
          .addFunction("getvalueint"  , &CLuaGuiItem::getValueInt)
          .addFunction("setvalue"     , &CLuaGuiItem::setValue)
          .addFunction("setvalueint"  , &CLuaGuiItem::setValueInt)
          .addFunction("setimage"     , &CLuaGuiItem::setImage)
        .endClass();
    }
  }
}