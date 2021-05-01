// (w) 2021 by Dustbin::Games / Christian Keimel
#include <scenenodes/CGui3dItem.h>

namespace dustbin {
  namespace scenenodes {
    CGui3dItem::CGui3dItem(irr::scene::ISceneNode* a_pParent, irr::scene::ISceneManager* a_pSmgr, irr::s32 a_iId) : CGui3dItemBase(a_pParent, a_pSmgr, a_iId), m_bHovered(false) {
    }

    CGui3dItem::~CGui3dItem() {
    }

    void CGui3dItem::itemEntered() {
      m_bHovered = true;
      updateRttText(m_cHoverColor, m_cTextColor);
    }

    void CGui3dItem::itemLeft() {
      m_bHovered = false;
      updateRttText(m_cHoverColor, m_cTextColor);
    }


    /**
    * Update the text of the 3d UI item
    * @param a_cBackgroundColor the background color to use
    * @param a_cTextColor the text color to use
    */
    void CGui3dItem::updateRttText(const irr::video::SColor& a_cBackgroundColor, const irr::video::SColor& a_cTextColor) {
      bool l_bDrawHovered = m_bHovered && (m_mSerializerMap.find(m_eType) != m_mSerializerMap.end() && std::find(m_mSerializerMap[m_eType].begin(), m_mSerializerMap[m_eType].end(), g_sHoverColorName) != m_mSerializerMap[m_eType].end());
      CGui3dItemBase::updateRttText(l_bDrawHovered ? m_cHoverColor : m_cBackground, m_cTextColor);
    }
  }
}