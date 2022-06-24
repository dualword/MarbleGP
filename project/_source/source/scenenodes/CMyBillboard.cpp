// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"
#include "IVideoDriver.h"
#include "ISceneManager.h"
#include "ICameraSceneNode.h"

#include <scenenodes/CMyBillboard.h>

namespace dustbin
{
  namespace scenenodes
  {

    //! constructor
    CMyBillboard::CMyBillboard(irr::scene::ISceneNode* parent, irr::scene::ISceneManager* mgr, irr::s32 id,
      const irr::core::vector3df& position, const irr::core::dimension2d<irr::f32>& size,
      irr::video::SColor colorTop, irr::video::SColor colorBottom)
      : IBillboardSceneNode(parent, mgr, id, position)
      , Buffer(new irr::scene::SMeshBuffer())
    {
#ifdef _DEBUG
      setDebugName("CMyBillboard");
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
    }

    CMyBillboard::~CMyBillboard()
    {
      Buffer->drop();
    }

    //! pre render event
    void CMyBillboard::OnRegisterSceneNode()
    {
      if (IsVisible)
        SceneManager->registerNodeForRendering(this);

      ISceneNode::OnRegisterSceneNode();
    }


    //! render
    void CMyBillboard::render()
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

      // make billboard look to camera
      updateMesh(camera);

      driver->setTransform(irr::video::ETS_WORLD, irr::core::IdentityMatrix);
      driver->setMaterial(Buffer->Material);
      driver->drawMeshBuffer(Buffer);

      if (DebugDataVisible & irr::scene::EDS_BBOX)
      {
        driver->setTransform(irr::video::ETS_WORLD, AbsoluteTransformation);
        irr::video::SMaterial m;
        m.Lighting = false;
        driver->setMaterial(m);
        driver->draw3DBox(BBoxSafe, irr::video::SColor(0,208,195,152));
      }
    }

    void CMyBillboard::updateMesh(const irr::scene::ICameraSceneNode* camera)
    {
      // billboard looks toward camera
      irr::core::vector3df pos = getAbsolutePosition();

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
    const irr::core::aabbox3d<irr::f32>& CMyBillboard::getBoundingBox() const
    {
      // Really wrong when scaled.
      return BBoxSafe;
    }

    const irr::core::aabbox3d<irr::f32>& CMyBillboard::getTransformedBillboardBoundingBox(const irr::scene::ICameraSceneNode* camera)
    {
      updateMesh(camera);
      return Buffer->BoundingBox;
    }

    void CMyBillboard::setSize(const irr::core::dimension2d<irr::f32>& size)
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


    void CMyBillboard::setSize(irr::f32 height, irr::f32 bottomEdgeWidth, irr::f32 topEdgeWidth)
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


    irr::video::SMaterial& CMyBillboard::getMaterial(irr::u32 i)
    {
      return Buffer->Material;
    }


    //! returns amount of materials used by this scene node.
    irr::u32 CMyBillboard::getMaterialCount() const
    {
      return 1;
    }


    //! gets the size of the billboard
    const irr::core::dimension2d<irr::f32>& CMyBillboard::getSize() const
    {
      return Size;
    }


    //! Gets the widths of the top and bottom edges of the billboard.
    void CMyBillboard::getSize(irr::f32& height, irr::f32& bottomEdgeWidth,
      irr::f32& topEdgeWidth) const
    {
      height = Size.Height;
      bottomEdgeWidth = Size.Width;
      topEdgeWidth = TopEdgeWidth;
    }


    //! Writes attributes of the scene node.
    void CMyBillboard::serializeAttributes(irr::io::IAttributes* out, irr::io::SAttributeReadWriteOptions* options) const
    {
      IBillboardSceneNode::serializeAttributes(out, options);

      out->addFloat("Width", Size.Width);
      out->addFloat("TopEdgeWidth", TopEdgeWidth);
      out->addFloat("Height", Size.Height);
      out->addColor("Shade_Top", Buffer->Vertices[1].Color);
      out->addColor("Shade_Down", Buffer->Vertices[0].Color);
    }


    //! Reads attributes of the scene node.
    void CMyBillboard::deserializeAttributes(irr::io::IAttributes* in, irr::io::SAttributeReadWriteOptions* options)
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
    void CMyBillboard::setColor(const irr::video::SColor& overallColor)
    {
      for(irr::u32 vertex = 0; vertex < 4; ++vertex)
        Buffer->Vertices[vertex].Color = overallColor;
    }


    //! Set the color of the top and bottom vertices of the billboard
    //! \param topColor: the color to set the top vertices
    //! \param bottomColor: the color to set the bottom vertices
    void CMyBillboard::setColor(const irr::video::SColor& topColor,
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
    void CMyBillboard::getColor(irr::video::SColor& topColor,
      irr::video::SColor& bottomColor) const
    {
      bottomColor = Buffer->Vertices[0].Color;
      topColor = Buffer->Vertices[1].Color;
    }


    //! Creates a clone of this scene node and its children.
    irr::scene::ISceneNode* CMyBillboard::clone(irr::scene::ISceneNode* newParent, irr::scene::ISceneManager* newManager)
    {
      if (!newParent)
        newParent = Parent;
      if (!newManager)
        newManager = SceneManager;

      CMyBillboard* nb = new CMyBillboard(newParent,
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


  } // end namespace scene
} // end namespace irr
