// (w) 2021 by Dustbin::Games / Christian Keimel
#include <_generated/messages/CMessages.h>
#include <scenenodes/CGui3dItem.h>
#include <state/IState.h>
#include <CGlobal.h>

namespace dustbin {
  namespace scenenodes {
    CGui3dItem::CGui3dItem(irr::scene::ISceneNode* a_pParent, irr::scene::ISceneManager* a_pSmgr, irr::s32 a_iId) : CGui3dItemBase(a_pParent, a_pSmgr, a_iId), 
      m_bHovered (false), 
      m_bSelected(false), 
      m_pState   (CGlobal::getInstance()->getActiveState()) 
    {
    }

    CGui3dItem::~CGui3dItem() {
    }

    void CGui3dItem::itemEntered() {
      m_bHovered = true;
      updateRttText((m_eType == enGui3dType::Button || m_eType == enGui3dType::IconButton) ? m_cHoverColor : m_cBackground, m_cTextColor);

      if (m_pState != nullptr) {
        m_pState->handleMessage(new messages::CUiEvent("uielementhovered", getID(), getParent() != nullptr ? getParent()->getName() : "", ""));
      }
    }

    void CGui3dItem::itemLeftButtonDown() {
      if (m_eType == enGui3dType::Button || m_eType == enGui3dType::IconButton) {
        m_bSelected = true;
        updateRttText(m_cClickColor, m_cTextColor);
      }
    }

    void CGui3dItem::itemLeftButtonUp() {
      if (m_eType == enGui3dType::Button || m_eType == enGui3dType::IconButton) {
        m_bSelected = false;
        updateRttText(m_cHoverColor, m_cTextColor);

        if (m_pState != nullptr) {
          m_pState->handleMessage(new messages::CUiEvent("uibuttonclicked", getID(), getParent() != nullptr ? getParent()->getName() : "", ""));
        }
      }
    }

    void CGui3dItem::itemLeft() {
      m_bHovered  = false;
      m_bSelected = false;
      updateRttText(m_cBackground, m_cTextColor);

      if (m_pState != nullptr) {
        m_pState->handleMessage(new messages::CUiEvent("uielementleft", getID(), getParent() != nullptr ? getParent()->getName() : "", ""));
      }
    }

    void CGui3dItem::itemClicked() {
      if (m_eType == enGui3dType::Button || m_eType == enGui3dType::IconButton) {
        m_bSelected = false;
        updateRttText(m_cHoverColor, m_cTextColor);
      }
    }

    /**
    * Update the text of the 3d UI item
    * @param a_cBackgroundColor the background color to use
    * @param a_cTextColor the text color to use
    */
    void CGui3dItem::updateRttText(const irr::video::SColor& a_cBackgroundColor, const irr::video::SColor& a_cTextColor) {
      CGui3dItemBase::updateRttText(a_cBackgroundColor, a_cTextColor);
    }
  }
}