// (w) 2021 by Dustbin::Games / Christian Keimel
#include <gui/CDustbinCheckbox.h>
#include <CGlobal.h>

namespace dustbin {
  namespace gui {

    CDustbinCheckbox::CDustbinCheckbox(irr::gui::IGUIElement* a_pParent) : 
      irr::gui::IGUIElement((irr::gui::EGUI_ELEMENT_TYPE)g_DustbinCheckboxId, CGlobal::getInstance()->getGuiEnvironment(), a_pParent, -1, irr::core::recti()), 
      m_bChecked(false), 
      m_iBorder(CGlobal::getInstance()->getRasterSize() / 10), m_pDrv(CGlobal::getInstance()->getVideoDriver()) 
    {
    }

    CDustbinCheckbox::~CDustbinCheckbox() {
    }

    void CDustbinCheckbox::draw() {
      int l_iWidth  = getAbsoluteClippingRect().getWidth (),
          l_iHeight = getAbsoluteClippingRect().getHeight(),
          l_iDrawX  = 85 * l_iWidth  / 100,
          l_iDrawY  = 85 * l_iHeight / 100,
          l_iCheckX = 75 * l_iWidth  / 100,
          l_iCheckY = 75 * l_iWidth  / 100,
          l_iBorder = l_iDrawX  > l_iDrawY  ? l_iHeight - l_iDrawY  : l_iWidth - l_iDrawX,
          l_iCheckB = l_iCheckX > l_iCheckY ? l_iHeight - l_iCheckY : l_iWidth - l_iCheckX,
          l_iPosX   = getAbsoluteClippingRect().UpperLeftCorner.X,
          l_iPosY   = getAbsoluteClippingRect().UpperLeftCorner.Y;

      m_pDrv->draw2DRectangle(irr::video::SColor(0xFF,    0,    0,    0), irr::core::recti(l_iPosX            , l_iPosY            , l_iPosX + l_iWidth, l_iPosY + l_iHeight));
      m_pDrv->draw2DRectangle(irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF), irr::core::recti(l_iPosX + l_iBorder, l_iPosY + l_iBorder, l_iPosX + l_iDrawX, l_iPosY + l_iDrawY ));

      if (m_bChecked)
        m_pDrv->draw2DRectangle(irr::video::SColor(0xFF, 0, 0, 0), irr::core::recti(l_iPosX + l_iCheckB, l_iPosY + l_iCheckB, l_iPosX + l_iCheckX, l_iPosY + l_iCheckY));
    }

    irr::gui::EGUI_ELEMENT_TYPE CDustbinCheckbox::getType() {
      return (irr::gui::EGUI_ELEMENT_TYPE)g_DustbinCheckboxId;
    }

    bool CDustbinCheckbox::OnEvent(const irr::SEvent& a_cEvent) {
      if (a_cEvent.EventType == irr::EET_MOUSE_INPUT_EVENT && a_cEvent.MouseInput.Event == irr::EMIE_LMOUSE_LEFT_UP) {
        if (getAbsoluteClippingRect().isPointInside(irr::core::position2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y)))
          m_bChecked = !m_bChecked;
      }
      return false;
    }

    void CDustbinCheckbox::setChecked(bool a_bChecked) {
      m_bChecked = a_bChecked;
    }

    bool CDustbinCheckbox::isChecked() {
      return m_bChecked;
    }

    void CDustbinCheckbox::serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const {
      irr::gui::IGUIElement::serializeAttributes(a_pOut, a_pOptions);
      a_pOut->addBool("Checked", m_bChecked);
    }

    void CDustbinCheckbox::deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) {
      irr::gui::IGUIElement::deserializeAttributes(a_pIn, a_pOptions);
      m_bChecked = a_pIn->getAttributeAsBool("Checked");
    }

  } // namespace gui
}   // namespace dustbin