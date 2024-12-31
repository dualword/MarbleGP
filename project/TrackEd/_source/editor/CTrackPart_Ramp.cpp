// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#include <editor/CTrackPart_Ramp.h>
#include <scenenodes/CDustbinId.h>
#include <CDefines.h>
#include <IGlobal.h>

namespace dustbin {
  namespace tracked {
    CTrackPart_Ramp::CTrackPart_Ramp(ITrackPart *a_pPrevious, const irr::core::vector3df &a_cNormal) :
      ITrackPart_Bezier(c_sTypeRamp, a_pPrevious, a_cNormal) 
    {
      m_iRoadBufferIdx = tracked::c_iBufferIdxArrow;

      if (a_pPrevious != nullptr)
        m_cSide = a_pPrevious->getEndSide();
      else 
        m_cSide = irr::core::vector3df(1.0f, 0.0f, 0.0f);
    }

    CTrackPart_Ramp::~CTrackPart_Ramp() {
    }

    /**
    * Get the (normalized) normal vector of a point
    * @param a_iIndex the point index
    * @return the (normalized) normal vector of a point
    */
    irr::core::vector3df CTrackPart_Ramp::getNormalOfPoint(int a_iIndex) const {
      if (a_iIndex == 0) {
        return getSideOfPoint(0).crossProduct(m_vPoints[0] - m_vPoints[1]).normalize();
      }
      else if (a_iIndex == (int)m_vPoints.size() - 1) {
        return getSideOfPoint(a_iIndex).crossProduct(m_vPoints[a_iIndex - 1] - m_vPoints[a_iIndex]).normalize();
      }
      else {
        irr::core::vector3df v1 = getSideOfPoint(a_iIndex).crossProduct(m_vPoints[a_iIndex    ] - m_vPoints[a_iIndex + 1]).normalize();
        irr::core::vector3df v2 = getSideOfPoint(a_iIndex).crossProduct(m_vPoints[a_iIndex - 1] - m_vPoints[a_iIndex    ]).normalize();

        return v1.getInterpolated(v2, 0.5);
      }
    }

    /**
    * Get the (normalized) side vector of a point
    * @param a_iIndex the point index
    * @return the (normalized) side vector of a point
    */
    irr::core::vector3df CTrackPart_Ramp::getSideOfPoint(int a_iIndex) const {
      // For a ramp segment the side vector does not change
      return m_cSide;
    }

    /**
    * Get the side vector at a specific index of the road
    * @param a_iIndex the index
    * @return the side vector at the index
    */
    irr::core::vector3df CTrackPart_Ramp::getSideAtIndex(int a_iIndex) {
      // For a ramp segment the side vector does not change
      return m_cSide;
    }

    /**
    * Get the normal at a specific index of the road
    * @param a_iIndex the index
    * @return the normal at the index
    */
    irr::core::vector3df CTrackPart_Ramp::getNormalAtIndex(int a_iIndex) {
      if (m_vTrack.size() == 0)
        calculateCurve();

      if (a_iIndex == 0)
        return getStartNormal();
      else
        return (m_vTrack[a_iIndex] - m_vTrack[a_iIndex - 1]).crossProduct(m_cSide).normalize();
    }
 
    /**
    * Get the side vector of the end of this track part
    * @return the side vector of the end of this track part
    */
    irr::core::vector3df CTrackPart_Ramp::getEndSide() const {
      return m_cSide;
    }

    /**
    * Set the trak part's reference points
    * @param a_cPoints a vector with the reference points
    */
    void CTrackPart_Ramp::setReferencePoints(const std::vector<irr::core::vector3df> &a_cPoints) {
      std::vector<irr::core::vector3df> l_vPoints = a_cPoints;

      if (a_cPoints.size() == 4) {
        l_vPoints[3].Y = l_vPoints[2].Y;
      }

      ITrackPart_Bezier::setReferencePoints(l_vPoints);
      setEndNormal(getNormalOfPoint((int)m_vPoints.size() - 1));
    }

    /**
    * This method is called when the reference points of the previous track part have changed
    */
    void CTrackPart_Ramp::updateReferencePoints() {
      if (m_vPoints.size() == 4) {
        ITrackPart *l_pPrevious = getPrevious();

        if (l_pPrevious != nullptr) {
          std::vector<irr::core::vector3df> l_vRefPoints = getReferencePoints();

          irr::core::vector3df l_cDir = l_pPrevious->getEndDirection();

          irr::f32 l_fLength1 = (l_vRefPoints[1] - l_vRefPoints[0]).getLength();
          irr::f32 l_fLength2 = (irr::core::vector3df(l_vRefPoints[2].X, 0.0f, l_vRefPoints[2].Z) - irr::core::vector3df(l_vRefPoints[1].X, 0.0f, l_vRefPoints[1].Z)).getLength();
          irr::f32 l_fLength3 = (l_vRefPoints[3] - l_vRefPoints[2]).getLength();
          irr::f32 l_fDistY   = l_vRefPoints[2].Y - l_vRefPoints[1].Y;

          l_vRefPoints[0] = l_pPrevious->getEnd().getMiddle();
          l_vRefPoints[1] = l_vRefPoints[0] + l_fLength1 * l_cDir;
          l_vRefPoints[2] = l_vRefPoints[1] + l_fLength2 * l_cDir + l_fDistY * irr::core::vector3df(0.0f, 1.0f, 0.0f);
          l_vRefPoints[3] = l_vRefPoints[2] + l_fLength3 * l_cDir;

          for (int i = 0; i < 4; i++) {
            if (l_vRefPoints[i].Y < 0.0f) {
              l_vRefPoints[i].Y = 0.0f;
            }
          }

          m_cSide = l_pPrevious->getEndSide();

          setReferencePoints(l_vRefPoints);
        }
      }
    }

    /**
    * Get the information on the route for the marbles
    * @return a vector with positions to pass for the marbles
    */
    std::vector<SAiPath> CTrackPart_Ramp::getAiPathData() {
      std::vector<SAiPath> l_vRet = ITrackPart_Bezier::getAiPathData();

      // Now let's analyse the path and search for inclines, declines and jumps
      for (size_t i = 2; i < l_vRet.size(); i++) {
        if (l_vRet[i].m_cPosition.Y - l_vRet[i - 1].m_cPosition.Y > 0.001) {
          l_vRet[i].m_eType = enPathType::Incline;

          irr::f32 f1 = l_vRet[i - 1].m_cPosition.Y - l_vRet[i - 2].m_cPosition.Y;
          irr::f32 f2 = l_vRet[i    ].m_cPosition.Y - l_vRet[i - 1].m_cPosition.Y;

          if (i < l_vRet.size() - 1 && f1 > f2) {
            l_vRet[i].m_eType = enPathType::Jump;
          }
        }

        if (l_vRet[i].m_cPosition.Y - l_vRet[i - 1].m_cPosition.Y < -0.001) {
          l_vRet[i].m_eType = enPathType::Decline;
          if (l_vRet[i - 1].m_eType != enPathType::Decline && l_vRet[i - 1].m_eType != enPathType::Jump)
            l_vRet[i].m_eType = enPathType::Jump;
        }
      }

      return l_vRet;
    }

    /**
    * Can a reference point be moved freely in the 2d plane
    * @param a_iIndex index of the reference point
    * @param a_cLine [out] the line in which the point can be moved if false is returned. A line with length of 0 is returned if the point can't move at all. Only used if "false" is returned
    * @param a_cPlanePoint [out] The point of the plane the reference point is moved in. Only used if "true" is returned.
    * @param a_cPlaneNormal [out] The normal of the plane the reference point is moved in.
    * @return true if the reference point can move freely, false otherwise
    */
    bool CTrackPart_Ramp::canPointMoveFreely(int a_iIndex, irr::core::line3df &a_cLine, irr::core::vector3df &a_cPlanePoint, irr::core::vector3df &a_cPlaneNormal) {
      if (getPrevious() == nullptr) {
        a_cLine.start = irr::core::vector3df(0.0f);
        a_cLine.end   = a_cLine.start;

        return false;
      }
      else if (a_iIndex == 1) {
        a_cLine.start = m_vPoints[0];
        a_cLine.end   = a_cLine.start + 1000.0f * (m_vPoints[1] - m_vPoints[0]).normalize();

        a_cPlanePoint  = m_vPoints[0];
        a_cPlaneNormal = getStartNormal();

        return false;
      }
      else if (a_iIndex == 2) {
        a_cPlanePoint  = m_vPoints[1];
        a_cPlaneNormal = getSideOfPoint(1);

        return true;
      }
      else {
        a_cLine.start = m_vPoints[2];
        a_cLine.end   = a_cLine.start + 1000.0f * (m_vPoints[3] - m_vPoints[2]).normalize();

        a_cPlanePoint  = m_vPoints[2];
        a_cPlaneNormal = getSideOfPoint(2);

        return false;
      }
    }

    /**
    * Add custom entries to the right-click context menu
    * @param a_iRefPoint the hovered reference point
    */
    std::vector<std::wstring> CTrackPart_Ramp::contextMenuEntries(int a_iRefPoint) {
      std::vector<std::wstring> l_vRet;

      if (a_iRefPoint == m_vPoints.size() - 1) {
        l_vRet.push_back(L"Flatten end point");
      }

      return l_vRet;
    }

    /**
    * Handle an event if this part is selected
    * @param a_cEvent the event to handle
    * @return true if the event was handled
    */
    bool CTrackPart_Ramp::OnEvent(const irr::SEvent &a_cEvent) {
      bool l_bRet = false;

      if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
        if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_MENU_ITEM_SELECTED) {

          if (a_cEvent.GUIEvent.Caller->getType() == irr::gui::EGUIET_CONTEXT_MENU) {
            irr::gui::IGUIContextMenu *l_pMenu = reinterpret_cast<irr::gui::IGUIContextMenu *>(a_cEvent.GUIEvent.Caller);

            std::wstring l_sItem = l_pMenu->getItemText(l_pMenu->getSelectedItem());

            if (l_sItem == L"Flatten end point") {
              m_vPoints.back().Y = m_vPoints[m_vPoints.size() - 2].Y;
              setEndNormal(irr::core::vector3df(0.0f, 1.0f, 0.0f));

              irr::SEvent l_cEvent{};
              l_cEvent.EventType = irr::EET_USER_EVENT;
              l_cEvent.UserEvent.UserData1 = c_iRecalcTrack;
              l_cEvent.UserEvent.UserData2 = c_iRecalcTrack;
              IGlobal::getInstance()->getDevice()->postEventFromUser(l_cEvent);
            }
          }
        }
      }

      return l_bRet;
    }

    /**
    * De-serialize the track part
    * @param a_cAttr the attributes to read the data from
    */
    void CTrackPart_Ramp::deserialize(const irr::io::IAttributes &a_cAttr) {
      ITrackPart_Bezier::deserialize(a_cAttr);
    }

    /**
    * Does this reference point allow addition of another reference point?
    * @param a_iIndex index of the reference point
    * @return true if so, false otherwise
    */
    bool CTrackPart_Ramp::refPointAdditionAllowed(int a_iIndex) {
      return false;
    }
  }
}