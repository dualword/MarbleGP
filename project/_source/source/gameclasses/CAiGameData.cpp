/* (w) 2021 - 22 by Dustbin::Games (Christian Keimel) - This file is licensed under the terms of the zlib license */
#include <scenenodes/CStartingGridSceneNode.h>
#include <scenenodes/CCheckpointNode.h>
#include <gameclasses/CAiGameData.h>
#include <helpers/CStringHelpers.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
#include <tuple>

namespace dustbin {
  namespace gameclasses {
    CAiGameData::CAiGameData(irr::video::IVideoDriver *a_pDrv, irr::scene::ISceneManager *a_pSmgr, scenenodes::CAiNode *a_pAiNode) :
      m_pDrv (a_pDrv),
      m_pSmgr(a_pSmgr),
      m_pNode(a_pAiNode)
    {
      if (m_pNode != nullptr)
        m_pNode->grab();

      printf("Ready.\n");
    }

    CAiGameData::~CAiGameData() {
      if (m_pNode != nullptr)
        m_pNode->drop();
    }

    void CAiGameData::draw() {
      irr::video::SMaterial l_cMaterial;
      l_cMaterial.AmbientColor  = irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF);
      l_cMaterial.EmissiveColor = irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF);
      l_cMaterial.DiffuseColor  = irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF);
      l_cMaterial.SpecularColor = irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF);

      l_cMaterial.Lighting        = false;
      l_cMaterial.Thickness       = 5.0f;
      l_cMaterial.Wireframe       = true;
      l_cMaterial.BackfaceCulling = false;

      m_pDrv->setMaterial(l_cMaterial);
      m_pDrv->setTransform(irr::video::ETS_WORLD, irr::core::matrix4());

      // for (std::vector<SAiSegment*>::iterator it = m_vPath.begin(); it != m_vPath.end(); it++) {
      //   irr::video::SColor l_cColor = irr::video::SColor(0xFF, 0x80, 0x80, 0xFF);
      // }
    }
  }
}