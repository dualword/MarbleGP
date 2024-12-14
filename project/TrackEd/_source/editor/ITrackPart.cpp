// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#include <editor/ITrackPart.h>
#include <IGlobal.h>

namespace dustbin {
  namespace tracked {
    int g_iInternalId = 0;

    /**
    * The constructor
    * @param a_sType the type ID of this track part
    * @param a_pPrevious the previous track part (nullptr for the starting part)
    * @param a_cNormal the normal at the starting point
    * @param a_pColMgr the Irrlicht scene collision manager
    */
    ITrackPart::ITrackPart(const std::string a_sType, ITrackPart *a_pPrevious, const irr::core::vector3df &a_cNormal, irr::scene::ISceneCollisionManager *a_pColMgr) :
      m_sType      (a_sType),
      m_bStart     (a_pPrevious == nullptr),
      m_iInternalId(++g_iInternalId),
      m_iPrevious  (-1),
      m_iNext      (-1),
      m_pPrevious  (a_pPrevious),
      m_pNext      (nullptr),
      m_cNormal    (a_cNormal)
    {
      if (m_pPrevious != nullptr) {
        m_iPrevious = m_pPrevious->getId();
        m_pPrevious->setNext(this);
      }
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
      if (a_pNext != nullptr)
        m_iNext = a_pNext->getId();
      else
        m_iNext = -1;

      m_pNext = a_pNext;

      printf("Track Part %i Next: %i\n", m_iInternalId, m_iNext);
    }

    /**
    * Set the previous track part
    * @param a_pPreviouse the previous track part
    */
    void ITrackPart::setPrevious(ITrackPart* a_pPrevious) {
      if (a_pPrevious != nullptr)
        m_iPrevious = a_pPrevious->getId();
      else
        m_iPrevious = -1;

      m_pPrevious = a_pPrevious;
      m_bStart = m_pPrevious == nullptr;

      printf("Track Part %i Previous: %i\n", m_iInternalId, m_iPrevious);
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

    /**
    * Get the ID of the track part
    * @return the ID of the track part
    */
    int ITrackPart::getId() {
      return m_iInternalId;
    }

    /**
    * Is this the start segment?
    * @return true if so, false otherwise
    */
    bool ITrackPart::isStartSegment() {
      return m_bStart;
    }


    /**
    * Get the type ID of this track part
    * @return the type ID of this track part
    */
    const std::string& ITrackPart::getType() {
      return m_sType;
    }

    /**
    * Get the id of the previous track part, needed for deserialization
    * @return the ID of the previous part
    */
    int ITrackPart::getPreviousID() {
      return m_iPrevious;
    }

    /**
    * Get the id of the next track part, needed for deserialization
    * @return the ID of the next part
    */
    int ITrackPart::getNextID() {
      return m_iNext;
    }

    /**
    * Serialize the track part
    * @param a_cAttr the attributes to serialize the data to
    */
    void ITrackPart::serialize(irr::io::IAttributes& a_cAttr) const {
      a_cAttr.addString  ("type"    , m_sType.c_str());
      a_cAttr.addInt     ("id"      , m_iInternalId);
      a_cAttr.addInt     ("previous", m_iPrevious);
      a_cAttr.addInt     ("next"    , m_iNext);
      a_cAttr.addVector3d("normal"  , m_cNormal);
    }

    /**
    * De-serialize the track part
    * @param a_cAttr the attributes to read the data from
    */
    void ITrackPart::deserialize(const irr::io::IAttributes& a_cAttr) {
      m_sType       = a_cAttr.getAttributeAsString  ("type").c_str();
      m_iInternalId = a_cAttr.getAttributeAsInt     ("id");
      m_iPrevious   = a_cAttr.getAttributeAsInt     ("previous");
      m_iNext       = a_cAttr.getAttributeAsInt     ("next");
      m_cNormal     = a_cAttr.getAttributeAsVector3d("normal");
    }

  }
}