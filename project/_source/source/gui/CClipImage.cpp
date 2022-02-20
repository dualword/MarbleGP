// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <helpers/CStringHelpers.h>
#include <platform/CPlatform.h>
#include <gui/CClipImage.h>
#include <state/IState.h>
#include <CGlobal.h>
#include <cstring>

namespace dustbin {
  namespace gui {

    CClipImage::CClipImage(irr::gui::IGUIElement* a_pParent) :
      irr::gui::IGUIElement((irr::gui::EGUI_ELEMENT_TYPE)g_ClipImageId, CGlobal::getInstance()->getGuiEnvironment(), a_pParent != nullptr ? a_pParent : CGlobal::getInstance()->getGuiEnvironment()->getRootGUIElement(), -1, irr::core::recti()),


      m_pGui    (CGlobal::getInstance()->getGuiEnvironment()),
      m_pDrv    (CGlobal::getInstance()->getVideoDriver()),
      m_pTexture(nullptr),
      m_sTexture(""),
      m_cHover  (irr::video::SColor(0xff, 0x33, 0x67, 0xb8)),
      m_cClick  (irr::video::SColor(0xff, 0xec, 0xf1, 0x63)),
      m_iX1     (-1),
      m_iY1     (-1),
      m_iX2     (-1),
      m_iY2     (-1),
      m_bLDown  (false),
      m_bHover  (false)
    {
    }

    CClipImage::~CClipImage() {
      if (m_pTexture != nullptr)
        m_pTexture->drop();
    }

    void CClipImage::draw() {
      if (IsVisible && m_pTexture != nullptr) {
        irr::core::recti l_cRect = getAbsoluteClippingRect();

        if (m_bHover) {
          m_pDrv->draw2DRectangle(m_bLDown ? m_cClick : m_cHover, l_cRect);
        }

        if (m_iX1 >= 0 && m_iY1 >= 0 && m_iX2 >= 0 && m_iY2 >= 0) {
          m_pDrv->draw2DImage(m_pTexture, l_cRect, irr::core::recti(m_iX1, m_iY1, m_iX2, m_iY2), &l_cRect, nullptr, true);
        }
        else {
          m_pDrv->draw2DImage(m_pTexture, l_cRect, irr::core::recti(irr::core::vector2di(0, 0), m_pTexture->getSize()), nullptr, nullptr, true);
        }
      }
    }

    /**
    * Change the image of the item
    * @param a_pTexture the new image
    */
    void CClipImage::setImage(irr::video::ITexture* a_pTexture) {
      if (m_pTexture != nullptr)
        m_pTexture->drop();

      m_pTexture = a_pTexture;

      if (m_pTexture != nullptr)
        m_pTexture->grab();
    }

    /**
    * Get the image of the item
    * @return the image of the item
    */
    irr::video::ITexture *CClipImage::getImage() {
      return m_pTexture;
    }

    void CClipImage::serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const {
      irr::gui::IGUIElement::serializeAttributes(a_pOut, a_pOptions);

      a_pOut->addInt("Left"  , m_iX1);
      a_pOut->addInt("Top"   , m_iY1);
      a_pOut->addInt("Right" , m_iX2);
      a_pOut->addInt("Bottom", m_iY1);

      a_pOut->addString("Texture", helpers::s2ws(m_sTexture).c_str());
    }

    void CClipImage::deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) {
      irr::gui::IGUIElement::deserializeAttributes(a_pIn, a_pOptions);

      m_iX1 = a_pIn->getAttributeAsInt("Left"  );
      m_iY1 = a_pIn->getAttributeAsInt("Top"   );
      m_iX2 = a_pIn->getAttributeAsInt("Right" );
      m_iY2 = a_pIn->getAttributeAsInt("Bottom");

#ifdef NO_XEFFECT
      irr::c8 l_sBuffer[2048];
      a_pIn->getAttributeAsString("Texture", l_sBuffer);
      m_sTexture = l_sBuffer;
#else
      m_sTexture = a_pIn->getAttributeAsString("Texture", irr::core::stringc("")).c_str();
#endif

      if (m_sTexture != "")
        m_pTexture = CGlobal::getInstance()->createTexture(m_sTexture);
    }

    bool CClipImage::OnEvent(const irr::SEvent& a_cEvent) {
      bool l_bRet = false;

      if (a_cEvent.EventType == irr::EET_MOUSE_INPUT_EVENT) {
        if (a_cEvent.MouseInput.isLeftPressed())
          m_bLDown = true;
        else if (!a_cEvent.MouseInput.isLeftPressed()) {
          if (m_bLDown) {
            irr::SEvent l_cEvent;
            
            l_cEvent.EventType = irr::EET_GUI_EVENT;
            l_cEvent.GUIEvent.EventType = irr::gui::EGET_BUTTON_CLICKED;
            l_cEvent.GUIEvent.Caller    = this;
            l_cEvent.GUIEvent.Element   = this;

            CGlobal::getInstance()->OnEvent(l_cEvent);
          }
          m_bLDown = false;
        }
      }
      else if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
        if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_ELEMENT_HOVERED) {
          if (a_cEvent.GUIEvent.Caller == this) {
            m_bHover = true;
            l_bRet = true;
          }
        }
        else if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_ELEMENT_LEFT) {
          if (a_cEvent.GUIEvent.Caller == this) {
            m_bHover = false;
            l_bRet = false;
          }
        }
      }

      return l_bRet;
    }

  } // namespace gui
}   // namespace dustbin