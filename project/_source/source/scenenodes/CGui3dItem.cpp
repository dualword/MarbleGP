// (w) 2021 by Dustbin::Games / Christian Keimel
#include <_generated/messages/CMessages.h>
#include <sound/CSoundInterface.h>
#include <scenenodes/CGui3dItem.h>
#include <state/IState.h>
#include <CGlobal.h>

namespace dustbin {
  namespace scenenodes {
    CGui3dItem::CGui3dItem(irr::scene::ISceneNode* a_pParent, irr::scene::ISceneManager* a_pSmgr, irr::s32 a_iId) : CGui3dItemBase(a_pParent, a_pSmgr, a_iId), 
      m_bHovered     (false), 
      m_bSelected    (false), 
      m_bDragging    (false),
      m_bInitialized (false),
      m_pState       (dustbin::CGlobal::getInstance()->getActiveState()),
      m_cStart       (irr::core::vector3df(0.0f)),
      m_cMin         (irr::core::vector3df(0.0f)),
      m_cMax         (irr::core::vector3df(0.0f)),
      m_cDirection   (irr::core::vector3df(1.0f, 0.0f, 0.0f)),
      m_fSliderOffset(0.0f)
    {
      m_cMaterial.setFlag(irr::video::EMF_WIREFRAME, false);
      m_cMaterial.setFlag(irr::video::EMF_LIGHTING, false);
      m_cMaterial.AmbientColor = irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF);
      m_cMaterial.Thickness = 12.5f;
    }

    CGui3dItem::~CGui3dItem() {
    }

    void CGui3dItem::itemEntered() {
      m_bHovered = true;
      updateRttText((m_eType == enGui3dType::Button || m_eType == enGui3dType::IconButton || m_eType == enGui3dType::Slider) ? m_cHoverColor : m_cBackground, m_cTextColor);
      
      if ((m_eType == enGui3dType::Button || m_eType == enGui3dType::IconButton || m_eType == enGui3dType::Slider) && dustbin::CGlobal::getInstance()->getSoundInterface() != nullptr)
        dustbin::CGlobal::getInstance()->getSoundInterface()->play2d(L"data/sounds/button_hover.ogg", 1.0f, 0.0f);

      if (m_pState != nullptr) {
        m_pState->handleMessage(new messages::CUiEvent("uielementhovered", getID(), getParent() != nullptr ? getParent()->getName() : "", ""));
      }
    }

    void CGui3dItem::itemLeftButtonDown() {
      if (m_eType == enGui3dType::Button || m_eType == enGui3dType::IconButton || m_eType == enGui3dType::Slider) {
        m_bSelected = true;
        updateRttText(m_cClickColor, m_cTextColor);
      }
    }

    void CGui3dItem::itemLeftButtonUp() {
      if (m_eType == enGui3dType::Button || m_eType == enGui3dType::IconButton || m_eType == enGui3dType::Slider) {
        m_bSelected = false;
        updateRttText(m_cHoverColor, m_cTextColor);

        if (m_pState != nullptr && m_eType != enGui3dType::Slider) {
          CGlobal::getInstance()->getSoundInterface()->play2d(L"data/sounds/button_press.ogg", 1.0f, 0.0f);
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

    bool CGui3dItem::canBeDragged() {
      return m_eType == enGui3dType::Slider;
    }

    /**
    * Set the "show text" flag useful for images
    * @param b the new value
    */
    void CGui3dItem::setShowText(bool b) {
      if (b != m_bShowText) {
        m_bShowText = b;
        updateRttText(m_cBackground, m_cTextColor);
      }
    }

    /**
    * Get the "show text" flag
    * @return the value of the "show text" flag
    */
    bool CGui3dItem::doesShowText() {
      return m_bShowText;
    }

    void CGui3dItem::stopDragging() {
      m_bDragging = false;
    }

    void CGui3dItem::deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) {
      CGui3dItemBase::deserializeAttributes(a_pIn, a_pOptions);
      if (m_eType == enGui3dType::Slider) {
        getParent()->updateAbsolutePosition();

        m_cDirection = irr::core::vector3df(m_eDirection == enGui3dAxis::AxisX ? 1.0f : 0.0f, m_eDirection == enGui3dAxis::AxisY ? 1.0f : 0.0f, m_eDirection == enGui3dAxis::AxisZ ? 1.0f : 0.0f);
        m_cStart = getParent()->getPosition() + m_fMinOffset * m_cDirection;

        setValue(m_fValue);
      }
    }

    void CGui3dItem::startDragging(const irr::core::line3df &a_cRay) {
      if (canBeDragged()) {
        if (!m_bInitialized) {
          getParent()->updateAbsolutePosition();
          irr::core::vector3df l_cPos = getParent()->getAbsolutePosition();

          irr::f32 l_fFactorMin = (m_fValue - m_fMinValue) / (m_fMaxValue - m_fMinValue),
                   l_fFactorMax = 1.0f - l_fFactorMin,
                   l_fTotal     = m_fMaxOffset - m_fMinOffset,
                   l_fMin       = l_fTotal * l_fFactorMin,
                   l_fMax       = l_fTotal * l_fFactorMax;

          m_cMin = l_cPos - (l_fMin * m_cDirection);
          m_cMax = l_cPos + (l_fMax * m_cDirection);

          m_bInitialized = true;
        }

        irr::core::vector3df l_cPlane = ((m_eDirection == enGui3dAxis::AxisX && m_eNormal == enGui3dAxis::AxisY) || (m_eDirection == enGui3dAxis::AxisY && m_eNormal == enGui3dAxis::AxisX)) ? irr::core::vector3df(0.0f, 0.0f, 1.0f) :
                                        ((m_eDirection == enGui3dAxis::AxisY && m_eNormal == enGui3dAxis::AxisZ) || (m_eDirection == enGui3dAxis::AxisZ && m_eNormal == enGui3dAxis::AxisY)) ? irr::core::vector3df(1.0f, 0.0f, 0.0f) :
                                        irr::core::vector3df(0.0f, 1.0f, 0.0f);

        m_cLine  = irr::core::line3df(m_cMin, m_cMax);
        m_cPlane = irr::core::plane3df(getAbsolutePosition(), getAbsolutePosition() + m_cDirection, getAbsolutePosition() + l_cPlane);

        m_bDragging = true;
        irr::core::vector3df l_cPoint;

        if (m_cPlane.getIntersectionWithLine(a_cRay.start, a_cRay.end, l_cPoint)) {
          irr::core::vector3df l_cClosest = m_cLine.getClosestPoint(l_cPoint) -  m_cMin;
          irr::f32 l_fFactor = l_cClosest.getLength() / (m_fMaxOffset - m_fMinOffset);

          irr::f32 l_fValue  = m_fMinValue + l_fFactor * (m_fMaxValue - m_fMinValue);
          m_fSliderOffset = l_fValue - m_fValue;
        }
      }
    }

    void CGui3dItem::drag(const irr::core::line3df& a_cRay) {
      irr::core::vector3df l_cPoint;

      if (m_cPlane.getIntersectionWithLine(a_cRay.start, a_cRay.end, l_cPoint)) {
        irr::core::vector3df l_cClosest = m_cLine.getClosestPoint(l_cPoint) -  m_cMin;
        irr::f32 l_fFactor = l_cClosest.getLength() / (m_fMaxOffset - m_fMinOffset);

        irr::f32 l_fValue = (m_fMinValue + l_fFactor * (m_fMaxValue - m_fMinValue));

        if (l_cClosest + m_cMin == m_cLine.start) {
          l_fValue        = m_fMinValue;
          l_fFactor       = 0.0f;
          m_fSliderOffset = 0.0f;
        }
        else if (l_cClosest + m_cMin == m_cLine.end) {
          l_fValue        = m_fMaxValue;
          l_fFactor       = 1.0f;
          m_fSliderOffset = 0.0f;
        }
        else {
          l_fValue -=  m_fSliderOffset;
          l_fValue = l_fValue < m_fMinValue ? m_fMinValue : l_fValue > m_fMaxValue ? m_fMaxValue : l_fValue;
          l_fFactor = l_fValue / (m_fMaxValue - m_fMinValue);
        }

        getParent()->setPosition(m_cStart + l_fFactor * (m_fMaxOffset - m_fMinOffset) * m_cDirection);

        if (l_fValue != m_fValue) {
          m_fValue = l_fValue;

          if (m_pState != nullptr) {
            m_pState->handleMessage(new messages::CUiEvent("uivaluechanged", getID(), getParent() != nullptr ? getParent()->getName() : "", std::to_string(m_fValue)));
          }
        }
      }
    }

    void CGui3dItem::setValue(float a_fValue) {
      m_fValue = a_fValue;

      if (m_fValue < m_fMinValue) m_fValue = m_fMinValue;
      if (m_fValue > m_fMaxValue) m_fValue = m_fMaxValue;

      irr::f32 l_fFactor = (m_fValue - m_fMinValue) / (m_fMaxValue - m_fMinValue);
      getParent()->setPosition(m_cStart + l_fFactor * (m_fMaxOffset - m_fMinOffset) * m_cDirection);
    }

    float CGui3dItem::getValue() {
      return m_fValue;
    }

    /**
    * Change the text of the item
    * @param a_sText the new text
    */
    void CGui3dItem::setText(const std::wstring& a_sText) {
      m_sText = a_sText;
      updateRttText(m_cBackground, m_cTextColor);
    }

    /**
    * Change the text of the item
    * @param a_sText the new text
    */
    const std::wstring CGui3dItem::getText() const {
      return m_sText;
    }

    /**
    * Change the background color of the item
    * @param a_cColor the new background color
    */
    void CGui3dItem::setBackgroundColor(const irr::video::SColor& a_cColor) {
      m_cBackground = a_cColor;
      updateRttText(m_cBackground, m_cTextColor);
    }

    /**
    * Update the text of the 3d UI item
    * @param a_cBackgroundColor the background color to use
    * @param a_cTextColor the text color to use
    */
    void CGui3dItem::updateRttText(const irr::video::SColor& a_cBackgroundColor, const irr::video::SColor& a_cTextColor) {
      CGui3dItemBase::updateRttText(a_cBackgroundColor, a_cTextColor);
    }

    void CGui3dItem::OnRegisterSceneNode() {
      // m_pSmgr->registerNodeForRendering(this);
    }

    void CGui3dItem::render() {
      if (m_bDragging) {
        irr::core::matrix4 l_cMatrix;
        l_cMatrix = l_cMatrix.makeIdentity();
        m_pSmgr->getVideoDriver()->setTransform(irr::video::ETS_WORLD, l_cMatrix);
        m_pSmgr->getVideoDriver()->setMaterial(m_cMaterial);
        m_pSmgr->getVideoDriver()->draw3DLine(m_cLine.start, m_cLine.end, irr::video::SColor(0xFF, 0xFF, 0, 0));
      }
    }
  }
}