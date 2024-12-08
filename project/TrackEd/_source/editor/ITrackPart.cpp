// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#include <editor/ITrackPart.h>
#include <IGlobal.h>

namespace dustbin {
  namespace tracked {
    /**
    * The constructor
    * @param a_pPrevious the previous track part (nullptr for the starting part)
    * @param a_cNormal the normal at the starting point
    * @param a_pColMgr the Irrlicht scene collision manager
    */
    ITrackPart::ITrackPart(ITrackPart *a_pPrevious, const irr::core::vector3df &a_cNormal, irr::scene::ISceneCollisionManager *a_pColMgr) :
      m_bSelected(false),
      m_pPrevious(a_pPrevious),
      m_pNext    (nullptr),
      m_cNormal  (a_cNormal)
    {
    }

    /**
    * The destructor
    */
    ITrackPart::~ITrackPart() {
    }

    const irr::core::vector3df &ITrackPart::getStartNormal() {
      return m_cNormal;
    }

    /**
    * Set the selection flag of this road part
    * @param a_bSelected the new value
    */
    void ITrackPart::setSelected(bool a_bSelected) {
      m_bSelected = a_bSelected;
    }

    /**
    * Is this track part selected?
    * @return true if the track part is selected, false otherwise
    */
    bool ITrackPart::isSelected() {
      return m_bSelected;
    }

    /**
    * Handle an event if this part is selected
    * @param a_cEvent the event to handle
    * @return true if the event was handled
    */
    bool ITrackPart::OnEvent(const irr::SEvent &a_cEvent) {
      return false;
    }

    /**
    * Set the next track part
    * @param a_pNext the next track part
    */
    void ITrackPart::setNext(ITrackPart *a_pNext) {
      m_pNext = a_pNext;
    }

    /**
    * Get the next track part
    * @return the next track part
    */
    ITrackPart *ITrackPart::getNext() {
      return m_pNext;
    }

    /**
    * Get the previous part
    * @return the previous part
    */
    ITrackPart *ITrackPart::getPrevious() {
      return m_pPrevious;
    }

    /**
    * Get the index of a vertex in a given mesh buffer
    * @param a_cVertex the vertex to find
    * @param a_pBuffer the mesh buffer to search in
    * @return the index of the vertex, -1 if the vertex was not found
    */
    irr::s32 ITrackPart::getIndexOfVertex(const irr::core::vector3df &a_cVertex, irr::scene::SMeshBuffer *a_pBuffer) {
      for (irr::u32 i = 0; i < a_pBuffer->getVertexCount(); i++) {
        if (a_pBuffer->Vertices[i].Pos == a_cVertex)
          return i;
      }

      return -1;
    }

    /**
    * Add a quad to a mesh buffer
    * @param a_aVertices the four vertices of the quad
    * @param a_aTexturePos the four texture positions of the vertices
    * @param a_pBuffer the buffer to be filled
    * @param a_bForceAdd add the first vertices regardless of whether or not the position is already in bool 
    */
    void ITrackPart::addQuadToMeshBuffer(
      irr::core::vector3df a_aVertices[4],
      irr::core::position2df a_aTexturePos[4],
      irr::scene::SMeshBuffer *a_pBuffer,
      bool a_bForceAdd)
    {
      irr::core::triangle3df l_aTriangles[] = {
        irr::core::triangle3df(a_aVertices[0], a_aVertices[1], a_aVertices[2]),
        irr::core::triangle3df(a_aVertices[1], a_aVertices[2], a_aVertices[3])
      };

      irr::s32 l_aIndices[4];


      for (irr::u32 i = 0; i < 4; i++) {
        l_aIndices[i] = getIndexOfVertex(a_aVertices[i], a_pBuffer);
      }

      if (a_bForceAdd) {
        l_aIndices[0] = -1;
        l_aIndices[1] = -1;
      }

      if (l_aIndices[0] == -1 || l_aIndices[1] == -1) { l_aIndices[0] = -1; l_aIndices[1] = -1; }
      if (l_aIndices[2] == -1 || l_aIndices[3] == -1) { l_aIndices[2] = -1; l_aIndices[3] = -1; }

      for (irr::u32 i = 0; i < 4; i++) {
        if (l_aIndices[i] == -1) {
          l_aIndices[i] = a_pBuffer->getVertexCount();
          a_pBuffer->Vertices.push_back(irr::video::S3DVertex(
            a_aVertices[i],
            l_aTriangles[i == 0 || i == 1 ? 0 : 1].getNormal(),
            irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF),
            a_aTexturePos[i]
          ));
        }
        else {
          a_pBuffer->Vertices[l_aIndices[i]].Normal = a_pBuffer->Vertices[l_aIndices[i]].Normal.getInterpolated(l_aTriangles[i == 0 || i == 1 ? 0 : 1].getNormal(), 0.5);
        }
      }

      a_pBuffer->Indices.push_back(l_aIndices[0]);
      a_pBuffer->Indices.push_back(l_aIndices[1]);
      a_pBuffer->Indices.push_back(l_aIndices[2]);

      a_pBuffer->Indices.push_back(l_aIndices[0]);
      a_pBuffer->Indices.push_back(l_aIndices[2]);
      a_pBuffer->Indices.push_back(l_aIndices[3]);
    }


    /**
    * Set the start of the part
    * @param a_cStart the start of the part
    */
    void ITrackPart::setStart(const irr::core::line3df &a_cStart) {
      m_cStart = a_cStart;
    }

    /**
    * Set the end of the part
    * @param a_cEnd the end of the part
    */
    void ITrackPart::setEnd(const irr::core::line3df &a_cEnd) {
      m_cEnd = a_cEnd;
    }

    /**
    * Get the start of the part
    * @return the start of the part
    */
    const irr::core::line3df &ITrackPart::getStart() {
      return m_cStart;
    }

    /**
    * Get the end of the part
    * @return the end of the part
    */
    const irr::core::line3df &ITrackPart::getEnd() {
      return m_cEnd;
    }
  }
}