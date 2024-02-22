// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <gameclasses/COffTrackDetector.h>
#include <gui/IProgressCallback.h>

namespace dustbin {
  namespace gameclasses {
    COfftrackDetector::COfftrackDetector(irr::scene::ISceneManager* a_pSmgr, gui::IProgressCallback *a_pProgress) : m_fMinY(-1000.0f), m_pSmgr(a_pSmgr) {
      calculateRespawnAreas(a_pProgress);
    }

    COfftrackDetector::~COfftrackDetector() {
    }

    /**
    * Check whether or not a point is on the track
    * @param a_cPos the point to verify
    * @return true if the point is on the track, false otherwise
    */
    bool COfftrackDetector::isOnTrack(const irr::core::vector3df& a_cPos) {
      if (a_cPos.Y < m_fMinY)
        return false;
      else if (a_cPos.X < m_cMin.X || a_cPos.X > m_cMax.X)
        return false;
      else if (a_cPos.Z < m_cMin.Y || a_cPos.Z > m_cMax.Y)
        return false;
      else {
        irr::f32 f1 = a_cPos.X - m_cMin.X;
        irr::f32 f2 = a_cPos.Z - m_cMin.Y;

        f1 /= 10.0f;
        f2 /= 10.0f;

        irr::s32 l_iIndexX = (irr::s32)(f1);
        irr::s32 l_iIndexY = (irr::s32)(f2);

        if (l_iIndexY >= 0 && l_iIndexY < m_vSectors.size()) {
          if (l_iIndexX >= 0 && l_iIndexX < m_vSectors[l_iIndexY].size()) {
            return m_vSectors[l_iIndexY][l_iIndexX];
          }
        }

        return false;
      }
    }


    /**
    * Iterate the scene nodes to get the bounding box
    * @param a_cNode the node to iterate
    * @param a_pSelector the triangle selector to be filled
    * @return the bounding box
    */
    void COfftrackDetector::getBoundingBox(irr::scene::ISceneNode* a_pNode, irr::core::aabbox3df* a_pBox, irr::scene::IMetaTriangleSelector* a_pSelector, int& a_iSelectorCount) {
      if (a_pNode->getType() == irr::scene::ESNT_MESH) {
        irr::scene::IMeshSceneNode *l_pNode = reinterpret_cast<irr::scene::IMeshSceneNode *>(a_pNode);

        a_pBox->addInternalBox(a_pNode->getTransformedBoundingBox());

        irr::scene::ITriangleSelector *l_pSelector = m_pSmgr->createTriangleSelector(l_pNode->getMesh(), l_pNode);
        a_pSelector->addTriangleSelector(l_pSelector);

        a_iSelectorCount++;
      }

      for (irr::core::list<irr::scene::ISceneNode *>::ConstIterator it = a_pNode->getChildren().begin(); it != a_pNode->getChildren().end(); it++) {
        getBoundingBox(*it, a_pBox, a_pSelector, a_iSelectorCount);
      }
    }

    /**
    * Calculate the respawn areas for the track
    */
    void COfftrackDetector::calculateRespawnAreas(gui::IProgressCallback *a_pProgress) {
      irr::scene::IMetaTriangleSelector *l_pSelector = m_pSmgr->createMetaTriangleSelector();
      irr::core::aabbox3df l_cBox;
      int l_iCount = 0;
      getBoundingBox(m_pSmgr->getRootSceneNode(), &l_cBox, l_pSelector, l_iCount);

      irr::core::triangle3df l_aTris[5];

      irr::f32 l_fWidth = l_cBox.getExtent().X;
      irr::f32 l_fDepth = l_cBox.getExtent().Z;

      irr::core::vector3df l_cMin = irr::core::vector3df(l_cBox.MinEdge.X        , l_cBox.MinEdge.Y, l_cBox.MinEdge.Z        );
      irr::core::vector3df l_cMax = irr::core::vector3df(l_cBox.MinEdge.X + 10.0f, l_cBox.MaxEdge.Y, l_cBox.MinEdge.Z + 10.0f);

      m_cSectors = irr::core::vector2di(
        (irr::s32)(l_fWidth / 10.0f),
        (irr::s32)(l_fDepth / 10.0f)
      );

      int l_iZ = 0;

      m_cMin  = irr::core::vector2df(l_cBox.MinEdge.X, l_cBox.MinEdge.Z);
      m_cMax  = irr::core::vector2df(l_cBox.MaxEdge.X, l_cBox.MaxEdge.Z);

      if (a_pProgress != nullptr) {
        int l_iCount = (int)(l_cBox.getExtent().X / 10.0f * l_cBox.getExtent().Z / 10.0f);

        a_pProgress->progressSetCurrentRange(
          a_pProgress->progressGetMessage(),
          a_pProgress->progressRangeMin(),
          a_pProgress->progressRangeMax(),
          l_iCount
        );
      }

      m_fMinY = l_cBox.MinEdge.Y;
      m_fMinY = l_cBox.MinEdge.Y;

      while (l_cMin.Z < l_cBox.MaxEdge.Z) {
        m_vSectors.push_back(std::vector<bool>());

        int l_iX = 0;

        while (l_cMin.X < l_cBox.MaxEdge.X) {
          if (a_pProgress != nullptr)
            a_pProgress->progressInc();

          irr::core::aabbox3df l_cOther = irr::core::aabbox3df(l_cMin, l_cMax);

          irr::s32 l_iOut = 0;

          l_pSelector->getTriangles(l_aTris, 5, l_iOut, l_cOther);

          if (l_iOut > 0) {
            m_vSectors.back().push_back(true);
          }
          else {
            m_vSectors.back().push_back(false);
          }

          l_cMin.X += 10.0f;
          l_cMax.X += 10.0f;

          l_iX++;
        }

        l_iZ++;

        l_cMin.X = l_cBox.MinEdge.X        ; l_cMin.Z += 10.0f;
        l_cMax.X = l_cBox.MinEdge.X + 10.0f; l_cMax.Z += 10.0f;
      }

      l_pSelector->drop();
    }
  }
}