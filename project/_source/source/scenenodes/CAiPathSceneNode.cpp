// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"
#include "IVideoDriver.h"
#include "ISceneManager.h"
#include "ICameraSceneNode.h"

#include <scenenodes/CAiPathSceneNode.h>

namespace dustbin
{
  namespace scenenodes
  {

    //! constructor
    CAiPathSceneNode::CAiPathSceneNode(irr::scene::ISceneNode* parent, irr::scene::ISceneManager* mgr, irr::s32 id,
      const irr::core::vector3df& position, const irr::core::dimension2d<irr::f32>& size,
      irr::video::SColor colorTop, irr::video::SColor colorBottom)
      : IBillboardSceneNode(parent, mgr, id, position)
      , Buffer(new irr::scene::SMeshBuffer())
      , m_bFirstCall(true)
    {
#ifdef _DEBUG
      setDebugName("CAiPathSceneNode");
#endif

      setSize(size);

      Buffer->Vertices.set_used(4);
      Buffer->Indices.set_used(6);

      Buffer->Indices[0] = 0;
      Buffer->Indices[1] = 2;
      Buffer->Indices[2] = 1;
      Buffer->Indices[3] = 0;
      Buffer->Indices[4] = 3;
      Buffer->Indices[5] = 2;

      Buffer->Vertices[0].TCoords.set(1.0f, 1.0f);
      Buffer->Vertices[0].Color = colorBottom;

      Buffer->Vertices[1].TCoords.set(1.0f, 0.0f);
      Buffer->Vertices[1].Color = colorTop;

      Buffer->Vertices[2].TCoords.set(0.0f, 0.0f);
      Buffer->Vertices[2].Color = colorTop;

      Buffer->Vertices[3].TCoords.set(0.0f, 1.0f);
      Buffer->Vertices[3].Color = colorBottom;

      Buffer->Material.MaterialType = irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL;
      Buffer->Material.setTexture(0, SceneManager->getVideoDriver()->getTexture("data/textures/aipath_marker.png"));
    }

    CAiPathSceneNode::~CAiPathSceneNode()
    {
      Buffer->drop();
    }

    //! pre render event
    void CAiPathSceneNode::OnRegisterSceneNode()
    {
      if (IsVisible)
        SceneManager->registerNodeForRendering(this);

      ISceneNode::OnRegisterSceneNode();
    }


    //! render
    void CAiPathSceneNode::render()
    {
      // My modification: don't render if not visible.
      // Rendering multiple viewports make this modification
      // necessary, the visiblity changes between beginScene
      // and the actual rendering
      if (!IsVisible) {
        return;
      }

      irr::video::IVideoDriver* driver = SceneManager->getVideoDriver();
      irr::scene::ICameraSceneNode* camera = SceneManager->getActiveCamera();

      if (!camera || !driver)
        return;

      driver->setTransform(irr::video::ETS_WORLD, irr::core::IdentityMatrix);

      irr::core::vector3df l_cPos = m_cPoint1;
      irr::core::vector3df l_cDir = (m_cPoint1 - getAbsolutePosition()).normalize();      
      irr::f32 l_fDist = m_cPoint1.getDistanceFrom(getAbsolutePosition());

      while (l_fDist > 0.0f) {
        // make billboard look to camera
        updateMesh(camera, l_cPos);

        l_cPos -= 25.0f * l_cDir;
        l_fDist -= 25.0f;

        driver->setMaterial(Buffer->Material);
        driver->drawMeshBuffer(Buffer);
      }

      l_cPos = m_cPoint1;
      l_cDir = (m_cPoint2 - m_cPoint1).normalize();
      l_fDist = m_cPoint1.getDistanceFrom(m_cPoint2);

      while (l_fDist > 0.0f) {
        updateMesh(camera, l_cPos);

        l_cPos += 12.5f * l_cDir;
        l_fDist -= 12.5f;

        driver->setMaterial(Buffer->Material);
        driver->drawMeshBuffer(Buffer);
      }
    }

    void CAiPathSceneNode::updateMesh(const irr::scene::ICameraSceneNode* camera, const irr::core::vector3df &a_cPointPos)
    {
      // billboard looks toward camera
      irr::core::vector3df pos = a_cPointPos; // getAbsolutePosition();

      irr::core::vector3df campos = camera->getAbsolutePosition();
      irr::core::vector3df target = camera->getTarget();
      irr::core::vector3df up = camera->getUpVector();
      irr::core::vector3df view = target - campos;
      view.normalize();

      irr::core::vector3df horizontal = up.crossProduct(view);
      if ( horizontal.getLength() == 0 )
      {
        horizontal.set(up.Y,up.X,up.Z);
      }
      horizontal.normalize();
      irr::core::vector3df topHorizontal = horizontal * 0.5f * TopEdgeWidth;
      horizontal *= 0.5f * Size.Width;

      // pointing down!
      irr::core::vector3df vertical = horizontal.crossProduct(view);
      vertical.normalize();
      vertical *= 0.5f * Size.Height;

      view *= -1.0f;

      irr::core::array<irr::video::S3DVertex>& vertices = Buffer->Vertices;

      for (irr::s32 i=0; i<4; ++i)
        vertices[i].Normal = view;

      /* Vertices are:
      2--1
      |\ |
      | \|
      3--0
      */
      vertices[0].Pos = pos + horizontal + vertical;
      vertices[1].Pos = pos + topHorizontal - vertical;
      vertices[2].Pos = pos - topHorizontal - vertical;
      vertices[3].Pos = pos - horizontal + vertical;

      Buffer->setDirty(irr::scene::EBT_VERTEX);
      Buffer->recalculateBoundingBox();
    }


    //! returns the axis aligned bounding box of this node
    const irr::core::aabbox3d<irr::f32>& CAiPathSceneNode::getBoundingBox() const
    {
      // Really wrong when scaled.
      return BBoxSafe;
    }

    const irr::core::aabbox3d<irr::f32>& CAiPathSceneNode::getTransformedBillboardBoundingBox(const irr::scene::ICameraSceneNode* camera)
    {
      irr::core::vector3df l_cPos = getAbsolutePosition();

      irr::core::vector3df campos = camera->getAbsolutePosition();
      irr::core::vector3df target = camera->getTarget();
      irr::core::vector3df up = camera->getUpVector();
      irr::core::vector3df view = target - campos;
      view.normalize();

      irr::core::vector3df horizontal = up.crossProduct(view);
      if ( horizontal.getLength() == 0 )
      {
        horizontal.set(up.Y,up.X,up.Z);
      }
      horizontal.normalize();
      irr::core::vector3df topHorizontal = horizontal * 0.5f * TopEdgeWidth;
      horizontal *= 0.5f * Size.Width;

      // pointing down!
      irr::core::vector3df vertical = horizontal.crossProduct(view);
      vertical.normalize();
      vertical *= 0.5f * Size.Height;

      Buffer->BoundingBox.reset(l_cPos);
      Buffer->BoundingBox.addInternalPoint(m_cPoint1);
      Buffer->BoundingBox.addInternalPoint(m_cPoint2);
      
      Buffer->BoundingBox.addInternalPoint(l_cPos + horizontal + vertical);
      Buffer->BoundingBox.addInternalPoint(m_cPoint1 + horizontal + vertical);
      Buffer->BoundingBox.addInternalPoint(m_cPoint2 + horizontal + vertical);
      
      Buffer->BoundingBox.addInternalPoint(l_cPos - horizontal - vertical);
      Buffer->BoundingBox.addInternalPoint(m_cPoint1 - horizontal - vertical);
      Buffer->BoundingBox.addInternalPoint(m_cPoint2 - horizontal - vertical);
      
      return Buffer->BoundingBox;
    }

    void CAiPathSceneNode::setSize(const irr::core::dimension2d<irr::f32>& size)
    {
      Size = size;

      if (irr::core::equals(Size.Width, 0.0f))
        Size.Width = 1.0f;
      TopEdgeWidth = Size.Width;

      if (irr::core::equals(Size.Height, 0.0f))
        Size.Height = 1.0f;

      const irr::f32 avg = (Size.Width + Size.Height)/6;
      BBoxSafe.MinEdge.set(-avg,-avg,-avg);
      BBoxSafe.MaxEdge.set(avg,avg,avg);
    }


    void CAiPathSceneNode::setSize(irr::f32 height, irr::f32 bottomEdgeWidth, irr::f32 topEdgeWidth)
    {
      Size.set(bottomEdgeWidth, height);
      TopEdgeWidth = topEdgeWidth;

      if (irr::core::equals(Size.Height, 0.0f))
        Size.Height = 1.0f;

      if (irr::core::equals(Size.Width, 0.f) && irr::core::equals(TopEdgeWidth, 0.f))
      {
        Size.Width = 1.0f;
        TopEdgeWidth = 1.0f;
      }

      const irr::f32 avg = (irr::core::max_(Size.Width,TopEdgeWidth) + Size.Height)/6;
      BBoxSafe.MinEdge.set(-avg,-avg,-avg);
      BBoxSafe.MaxEdge.set(avg,avg,avg);
    }


    irr::video::SMaterial& CAiPathSceneNode::getMaterial(irr::u32 i)
    {
      return Buffer->Material;
    }


    //! returns amount of materials used by this scene node.
    irr::u32 CAiPathSceneNode::getMaterialCount() const
    {
      return 1;
    }


    //! gets the size of the billboard
    const irr::core::dimension2d<irr::f32>& CAiPathSceneNode::getSize() const
    {
      return Size;
    }


    //! Gets the widths of the top and bottom edges of the billboard.
    void CAiPathSceneNode::getSize(irr::f32& height, irr::f32& bottomEdgeWidth,
      irr::f32& topEdgeWidth) const
    {
      height = Size.Height;
      bottomEdgeWidth = Size.Width;
      topEdgeWidth = TopEdgeWidth;
    }


    //! Writes attributes of the scene node.
    void CAiPathSceneNode::serializeAttributes(irr::io::IAttributes* out, irr::io::SAttributeReadWriteOptions* options) const
    {
      IBillboardSceneNode::serializeAttributes(out, options);

      out->addFloat("Width", Size.Width);
      out->addFloat("TopEdgeWidth", TopEdgeWidth);
      out->addFloat("Height", Size.Height);
      out->addColor("Shade_Top", Buffer->Vertices[1].Color);
      out->addColor("Shade_Down", Buffer->Vertices[0].Color);
    }


    //! Reads attributes of the scene node.
    void CAiPathSceneNode::deserializeAttributes(irr::io::IAttributes* in, irr::io::SAttributeReadWriteOptions* options)
    {
      IBillboardSceneNode::deserializeAttributes(in, options);

      Size.Width = in->getAttributeAsFloat("Width");
      Size.Height = in->getAttributeAsFloat("Height");

      if (in->existsAttribute("TopEdgeWidth"))
      {
        TopEdgeWidth = in->getAttributeAsFloat("TopEdgeWidth");
        if (Size.Width != TopEdgeWidth)
          setSize(Size.Height, Size.Width, TopEdgeWidth);
      }
      else
        setSize(Size);
      Buffer->Vertices[1].Color = in->getAttributeAsColor("Shade_Top");
      Buffer->Vertices[0].Color = in->getAttributeAsColor("Shade_Down");
      Buffer->Vertices[2].Color = Buffer->Vertices[1].Color;
      Buffer->Vertices[3].Color = Buffer->Vertices[0].Color;
    }


    //! Set the color of all vertices of the billboard
    //! \param overallColor: the color to set
    void CAiPathSceneNode::setColor(const irr::video::SColor& overallColor)
    {
      for(irr::u32 vertex = 0; vertex < 4; ++vertex)
        Buffer->Vertices[vertex].Color = overallColor;

      Buffer->Material.AmbientColor = overallColor;
    }


    //! Set the color of the top and bottom vertices of the billboard
    //! \param topColor: the color to set the top vertices
    //! \param bottomColor: the color to set the bottom vertices
    void CAiPathSceneNode::setColor(const irr::video::SColor& topColor,
      const irr::video::SColor& bottomColor)
    {
      Buffer->Vertices[0].Color = bottomColor;
      Buffer->Vertices[1].Color = topColor;
      Buffer->Vertices[2].Color = topColor;
      Buffer->Vertices[3].Color = bottomColor;
    }


    //! Gets the color of the top and bottom vertices of the billboard
    //! \param[out] topColor: stores the color of the top vertices
    //! \param[out] bottomColor: stores the color of the bottom vertices
    void CAiPathSceneNode::getColor(irr::video::SColor& topColor,
      irr::video::SColor& bottomColor) const
    {
      bottomColor = Buffer->Vertices[0].Color;
      topColor = Buffer->Vertices[1].Color;
    }


    //! Creates a clone of this scene node and its children.
    irr::scene::ISceneNode* CAiPathSceneNode::clone(irr::scene::ISceneNode* newParent, irr::scene::ISceneManager* newManager)
    {
      if (!newParent)
        newParent = Parent;
      if (!newManager)
        newManager = SceneManager;

      CAiPathSceneNode* nb = new CAiPathSceneNode(newParent,
        newManager, ID, RelativeTranslation, Size);

      nb->cloneMembers(this, newManager);
      nb->Buffer->Material = Buffer->Material;
      nb->Size = Size;
      nb->TopEdgeWidth = this->TopEdgeWidth;

      irr::video::SColor topColor,bottomColor;
      getColor(topColor,bottomColor);
      nb->setColor(topColor,bottomColor);

      if ( newParent )
        nb->drop();
      return nb;
    }


    /**
    * Set the two AI points. The lines will be shown from position -> point 1 and point 1 -> point 2
    * @param a_cPoint1 the first point calculated by the AI
    * @param a_cPoint2 the second point calculated by the AI
    */
    void CAiPathSceneNode::setAiData(const irr::core::vector3df& a_cPoint1, const irr::core::vector3df& a_cPoint2) {
      m_cPoint1 = a_cPoint1;
      m_cPoint2 = a_cPoint2;
    }
  } // end namespace scene
} // end namespace irr
