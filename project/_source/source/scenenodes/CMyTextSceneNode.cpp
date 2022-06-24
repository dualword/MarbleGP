// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include <scenenodes/CMyTextSceneNode.h>

#include "ISceneManager.h"
#include "IVideoDriver.h"
#include "ICameraSceneNode.h"
#include "IGUISpriteBank.h"
#include "SMeshBuffer.h"
#include <irrlicht.h>


namespace dustbin
{
  namespace scenenodes
  {


    //! constructor
    CMyTextNode::CMyTextNode(ISceneNode* parent, irr::scene::ISceneManager* mgr, irr::s32 id,
      irr::gui::IGUIFont* font, irr::scene::ISceneCollisionManager* coll,
      const irr::core::vector3df& position, const wchar_t* text,
      irr::video::SColor color)
      : ITextSceneNode(parent, mgr, id, position), Text(text), Color(color),
      Font(font), Coll(coll)

    {
#ifdef _DEBUG
      setDebugName("CMyTextNode");
#endif

      if (Font)
        Font->grab();

      setAutomaticCulling(irr::scene::EAC_OFF);
    }

    //! destructor
    CMyTextNode::~CMyTextNode()
    {
      if (Font)
        Font->drop();
    }

    void CMyTextNode::OnRegisterSceneNode()
    {
      if (IsVisible)
        SceneManager->registerNodeForRendering(this, irr::scene::ESNRP_TRANSPARENT);

      ISceneNode::OnRegisterSceneNode();
    }

    //! renders the node.
    void CMyTextNode::render()
    {
      if (!IsVisible)
        return;

      if (!Font || !Coll)
        return;

      irr::core::position2d<irr::s32> pos = Coll->getScreenCoordinatesFrom3DPosition(getAbsolutePosition(),
        SceneManager->getActiveCamera());

      irr::core::rect<irr::s32> r(pos, irr::core::dimension2d<irr::s32>(1,1));
      Font->draw(Text, r, Color, true, true);
    }


    //! returns the axis aligned bounding box of this node
    const irr::core::aabbox3d<irr::f32>& CMyTextNode::getBoundingBox() const
    {
      return Box;
    }

    //! sets the text string
    void CMyTextNode::setText(const wchar_t* text)
    {
      Text = text;
    }

    //! get the text string
    const wchar_t* CMyTextNode::getText() const
    {
      return Text.c_str();
    }

    //! sets the color of the text
    void CMyTextNode::setTextColor(irr::video::SColor color)
    {
      Color = color;
    }

    //! get the color of the text
    irr::video::SColor CMyTextNode::getTextColor() const
    {
      return Color;
    }

    void CMyTextNode::setFont(irr::gui::IGUIFont* font)
    {
      if ( font != Font )
      {
        if ( font )
          font->grab();
        if ( Font )
          Font->drop();
        Font = font;
      }
    }

    //! Get the font used to draw the text
    irr::gui::IGUIFont* CMyTextNode::getFont() const
    {
      return Font;
    }


    //!--------------------------------- CMyBillboardText ----------------------------------------------


    //! constructor
    CMyBillboardText::CMyBillboardText(ISceneNode* parent, irr::scene::ISceneManager* mgr, irr::s32 id,
      irr::gui::IGUIFont* font,const wchar_t* text,
      const irr::core::vector3df& position, const irr::core::dimension2d<irr::f32>& size,
      irr::video::SColor colorTop,irr::video::SColor shade_bottom )
      : IBillboardTextSceneNode(parent, mgr, id, position),
      Font(0), ColorTop(colorTop), ColorBottom(shade_bottom), Mesh(0)
    {
#ifdef _DEBUG
      setDebugName("CMyBillboardText");
#endif

      Material.MaterialType = irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL;
      Material.MaterialTypeParam = 1.f / 255.f;
      Material.BackfaceCulling = false;
      Material.Lighting = false;
      Material.ZBuffer = irr::video::ECFN_LESSEQUAL;
      Material.ZWriteEnable = irr::video::EZW_OFF;

      if (font)
      {
        // doesn't support other font types
        if (font->getType() == irr::gui::EGFT_BITMAP)
        {
          Font = (irr::gui::IGUIFontBitmap*)font;
          Font->grab();

          // mesh with one buffer per texture
          Mesh = new irr::scene::SMesh();
          for (irr::u32 i=0; i<Font->getSpriteBank()->getTextureCount(); ++i)
          {
            irr::scene::SMeshBuffer *mb = new irr::scene::SMeshBuffer();
            mb->Material = Material;
            mb->Material.setTexture(0, Font->getSpriteBank()->getTexture(i));
            Mesh->addMeshBuffer(mb);
            mb->drop();
          }
        }
      }

      setText(text);
      setSize(size);

      setAutomaticCulling ( irr::scene::EAC_BOX );
    }



    CMyBillboardText::~CMyBillboardText()
    {
      if (Font)
        Font->drop();

      if (Mesh)
        Mesh->drop();

    }


    //! sets the text string
    void CMyBillboardText::setText(const wchar_t* text)
    {
      if ( !Mesh )
        return;

      Text = text;

      Symbol.clear();

      // clear mesh
      for (irr::u32 j=0; j < Mesh->getMeshBufferCount(); ++j)
      {
        ((irr::scene::SMeshBuffer*)Mesh->getMeshBuffer(j))->Indices.clear();
        ((irr::scene::SMeshBuffer*)Mesh->getMeshBuffer(j))->Vertices.clear();
      }

      if (!Font)
        return;

      const irr::core::array< irr::core::rect<irr::s32> > &sourceRects = Font->getSpriteBank()->getPositions();
      const irr::core::array< irr::gui::SGUISprite > &sprites = Font->getSpriteBank()->getSprites();

      irr::f32 dim[2];
      irr::f32 tex[4];

      irr::u32 i;
      for ( i = 0; i != Text.size (); ++i )
      {
        SSymbolInfo info;

        irr::u32 spriteno = Font->getSpriteNoFromChar( &text[i] );
        irr::u32 rectno = sprites[spriteno].Frames[0].rectNumber;
        irr::u32 texno = sprites[spriteno].Frames[0].textureNumber;

        irr::video::ITexture* texture = Font->getSpriteBank()->getTexture(texno);
        if (texture)
        {
          const irr::core::dimension2d<irr::u32>& texSize = texture->getOriginalSize();
          dim[0] = irr::core::reciprocal((irr::f32)texSize.Width);
          dim[1] = irr::core::reciprocal((irr::f32)texSize.Height);
        }
        else
        {
          dim[0] = 0;
          dim[1] = 0;
        }

        const irr::core::rect<irr::s32>& s = sourceRects[rectno];

        // add space for letter to buffer
        irr::scene::SMeshBuffer* buf = (irr::scene::SMeshBuffer*)Mesh->getMeshBuffer(texno);
        irr::u32 firstInd = buf->Indices.size();
        irr::u32 firstVert = buf->Vertices.size();
        buf->Indices.set_used(firstInd + 6);
        buf->Vertices.set_used(firstVert + 4);

        tex[0] = (s.LowerRightCorner.X * dim[0]) + 0.5f*dim[0]; // half pixel
        tex[1] = (s.LowerRightCorner.Y * dim[1]) + 0.5f*dim[1];
        tex[2] = (s.UpperLeftCorner.Y  * dim[1]) - 0.5f*dim[1];
        tex[3] = (s.UpperLeftCorner.X  * dim[0]) - 0.5f*dim[0];

        buf->Vertices[firstVert+0].TCoords.set(tex[0], tex[1]);
        buf->Vertices[firstVert+1].TCoords.set(tex[0], tex[2]);
        buf->Vertices[firstVert+2].TCoords.set(tex[3], tex[2]);
        buf->Vertices[firstVert+3].TCoords.set(tex[3], tex[1]);

        buf->Vertices[firstVert+0].Color = ColorBottom;
        buf->Vertices[firstVert+3].Color = ColorBottom;
        buf->Vertices[firstVert+1].Color = ColorTop;
        buf->Vertices[firstVert+2].Color = ColorTop;

        buf->Indices[firstInd+0] = (irr::u16)firstVert+0;
        buf->Indices[firstInd+1] = (irr::u16)firstVert+2;
        buf->Indices[firstInd+2] = (irr::u16)firstVert+1;
        buf->Indices[firstInd+3] = (irr::u16)firstVert+0;
        buf->Indices[firstInd+4] = (irr::u16)firstVert+3;
        buf->Indices[firstInd+5] = (irr::u16)firstVert+2;

        wchar_t *tp = 0;
        if (i>0)
          tp = &Text[i-1];

        info.Width = (irr::f32)s.getWidth();
        info.bufNo = texno;
        info.Kerning = (irr::f32)Font->getKerningWidth(&Text[i], tp);
        info.firstInd = firstInd;
        info.firstVert = firstVert;

        Symbol.push_back(info);
      }
    }

    //! get the text string
    const wchar_t* CMyBillboardText::getText() const
    {
      return Text.c_str();
    }

    //! pre render event
    void CMyBillboardText::OnAnimate(irr::u32 timeMs)
    {
      ISceneNode::OnAnimate(timeMs);

      if (!IsVisible || !Font || !Mesh)
        return;

      irr::scene::ICameraSceneNode* camera = SceneManager->getActiveCamera();
      if (!camera)
        return;

      // TODO: Risky - if camera is later in the scene-graph then it's not yet updated here
      //       CBillBoardSceneNode does it different, but maybe real solution would be to enforce cameras to update earlier?
      //       Maybe we can also unify the code by using a common base-class or having updateMesh functionality in an animator instead.
      updateMesh(camera);

      // mesh uses vertices with absolute coordinates so to get a bbox for culling we have to get back to local ones.
      BBox = Mesh->getBoundingBox();
      irr::core::matrix4 mat( getAbsoluteTransformation(), irr::core::matrix4::EM4CONST_INVERSE );
      mat.transformBoxEx(BBox);
    }

    const irr::core::aabbox3d<irr::f32>& CMyBillboardText::getTransformedBillboardBoundingBox(const irr::scene::ICameraSceneNode* camera)
    {
      updateMesh(camera);
      return Mesh->getBoundingBox();
    }

    void CMyBillboardText::updateMesh(const irr::scene::ICameraSceneNode* camera)
    {
      // get text width
      irr::f32 textLength = 0.f;
      irr::u32 i;
      for(i=0; i!=Symbol.size(); ++i)
      {
        SSymbolInfo &info = Symbol[i];
        textLength += info.Kerning + info.Width;
      }
      if (textLength<0.0f)
        textLength=1.0f;

      //const core::matrix4 &m = camera->getViewFrustum()->Matrices[ video::ETS_VIEW ];

      // make billboard look to camera
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
      irr::core::vector3df space = horizontal;

      horizontal *= 0.5f * Size.Width;

      irr::core::vector3df vertical = horizontal.crossProduct(view);
      vertical.normalize();
      vertical *= 0.5f * Size.Height;

      view *= -1.0f;

      // center text
      pos += space * (Size.Width * -0.5f);

      for ( i = 0; i!= Symbol.size(); ++i )
      {
        SSymbolInfo &info = Symbol[i];
        irr::f32 infw = info.Width / textLength;
        irr::f32 infk = info.Kerning / textLength;
        irr::f32 w = (Size.Width * infw * 0.5f);
        pos += space * w;

        irr::scene::SMeshBuffer* buf = (irr::scene::SMeshBuffer*)Mesh->getMeshBuffer(info.bufNo);

        buf->Vertices[info.firstVert+0].Normal = view;
        buf->Vertices[info.firstVert+1].Normal = view;
        buf->Vertices[info.firstVert+2].Normal = view;
        buf->Vertices[info.firstVert+3].Normal = view;

        buf->Vertices[info.firstVert+0].Pos = pos + (space * w) + vertical;
        buf->Vertices[info.firstVert+1].Pos = pos + (space * w) - vertical;
        buf->Vertices[info.firstVert+2].Pos = pos - (space * w) - vertical;
        buf->Vertices[info.firstVert+3].Pos = pos - (space * w) + vertical;

        pos += space * (Size.Width*infk + w);
      }

      // make bounding box
      for (i=0; i< Mesh->getMeshBufferCount() ; ++i)
        Mesh->getMeshBuffer(i)->recalculateBoundingBox();
      Mesh->recalculateBoundingBox();
    }

    void CMyBillboardText::OnRegisterSceneNode()
    {
      if (IsVisible && Font && Mesh)
      {
        SceneManager->registerNodeForRendering(this, irr::scene::ESNRP_TRANSPARENT);
        ISceneNode::OnRegisterSceneNode();
      }
    }


    //! render
    void CMyBillboardText::render()
    {
      if (!IsVisible)
        return;

      if ( !Mesh )
        return;

      irr::video::IVideoDriver* driver = SceneManager->getVideoDriver();

      // draw
      irr::core::matrix4 mat;
      driver->setTransform(irr::video::ETS_WORLD, mat);

      for (irr::u32 i = 0; i < Mesh->getMeshBufferCount(); ++i)
      {
        driver->setMaterial(Mesh->getMeshBuffer(i)->getMaterial());
        driver->drawMeshBuffer(Mesh->getMeshBuffer(i));
      }

      if ( DebugDataVisible & irr::scene::EDS_BBOX )
      {
        driver->setTransform(irr::video::ETS_WORLD, AbsoluteTransformation);
        irr::video::SMaterial m;
        m.Lighting = false;
        driver->setMaterial(m);
        driver->draw3DBox(BBox, irr::video::SColor(0,208,195,152));
      }
    }


    //! returns the axis aligned bounding box of this node
    const irr::core::aabbox3d<irr::f32>& CMyBillboardText::getBoundingBox() const
    {
      return BBox;
    }


    //! sets the size of the billboard
    void CMyBillboardText::setSize(const irr::core::dimension2d<irr::f32>& size)
    {
      Size = size;

      if (Size.Width == 0.0f)
        Size.Width = 1.0f;

      if (Size.Height == 0.0f )
        Size.Height = 1.0f;

      //f32 avg = (size.Width + size.Height)/6;
      //BBox.MinEdge.set(-avg,-avg,-avg);
      //BBox.MaxEdge.set(avg,avg,avg);
    }


    irr::video::SMaterial& CMyBillboardText::getMaterial(irr::u32 i)
    {
      if (Mesh && Mesh->getMeshBufferCount() > i )
        return Mesh->getMeshBuffer(i)->getMaterial();
      else
        return Material;
    }


    //! returns amount of materials used by this scene node.
    irr::u32 CMyBillboardText::getMaterialCount() const
    {
      if (Mesh)
        return Mesh->getMeshBufferCount();
      else
        return 0;
    }


    //! gets the size of the billboard
    const irr::core::dimension2d<irr::f32>& CMyBillboardText::getSize() const
    {
      return Size;
    }

    //! Get the font used to draw the text
    irr::gui::IGUIFont* CMyBillboardText::getFont() const
    {
      return Font;
    }

    //! Set the color of all vertices of the billboard
    //! \param overallColor: the color to set
    void CMyBillboardText::setColor(const irr::video::SColor & overallColor)
    {
      if ( !Mesh )
        return;

      for ( irr::u32 i = 0; i != Text.size (); ++i )
      {
        const SSymbolInfo &info = Symbol[i];
        irr::scene::SMeshBuffer* buf = (irr::scene::SMeshBuffer*)Mesh->getMeshBuffer(info.bufNo);
        buf->Vertices[info.firstVert+0].Color = overallColor;
        buf->Vertices[info.firstVert+1].Color = overallColor;
        buf->Vertices[info.firstVert+2].Color = overallColor;
        buf->Vertices[info.firstVert+3].Color = overallColor;
      }
    }


    //! Set the color of the top and bottom vertices of the billboard
    //! \param topColor: the color to set the top vertices
    //! \param bottomColor: the color to set the bottom vertices
    void CMyBillboardText::setColor(const irr::video::SColor & topColor, const irr::video::SColor & bottomColor)
    {
      if ( !Mesh )
        return;

      ColorBottom = bottomColor;
      ColorTop = topColor;
      for ( irr::u32 i = 0; i != Text.size (); ++i )
      {
        const SSymbolInfo &info = Symbol[i];
        irr::scene::SMeshBuffer* buf = (irr::scene::SMeshBuffer*)Mesh->getMeshBuffer(info.bufNo);
        buf->Vertices[info.firstVert+0].Color = ColorBottom;
        buf->Vertices[info.firstVert+3].Color = ColorBottom;
        buf->Vertices[info.firstVert+1].Color = ColorTop;
        buf->Vertices[info.firstVert+2].Color = ColorTop;
      }
    }


    //! Gets the color of the top and bottom vertices of the billboard
    //! \param topColor: stores the color of the top vertices
    //! \param bottomColor: stores the color of the bottom vertices
    void CMyBillboardText::getColor(irr::video::SColor & topColor, irr::video::SColor & bottomColor) const
    {
      topColor = ColorTop;
      bottomColor = ColorBottom;
    }


  } // end namespace scene
} // end namespace irr

