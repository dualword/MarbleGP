// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#include <irrlicht.h>

#include <gui/IProgressCallback.h>
#include <helpers/CSceneLoader.h>

namespace dustbin {
  namespace helpers {
    /**
    * @class CDustbinSceneLoader
    * @author Christian Keimel / Irrlicht Team
    * This class is a copy of the class "CSceneLoaderIrr" with added
    * progress callback functionality. It is defined here as I don't
    * want to show it in the project, it's only meant to be visible
    * to the "dustbin::helpers::loadScene" function
    */
    class CDustbinSceneLoader : public virtual irr::scene::ISceneLoader {
      private:
        irr::scene::ISceneManager *SceneManager;
        irr::io::IFileSystem      *FileSystem;
        irr::ILogger              *m_pLogger;
        gui::IProgressCallback    *m_pCallback;
        int                        m_iNodeCnt;

        //! constants for reading and writing XML.
        //! Not made static due to portability problems.
        // TODO: move to own header
        const irr::core::stringw IRR_XML_FORMAT_SCENE;
        const irr::core::stringw IRR_XML_FORMAT_NODE;
        const irr::core::stringw IRR_XML_FORMAT_NODE_ATTR_TYPE;
        const irr::core::stringw IRR_XML_FORMAT_ATTRIBUTES;
        const irr::core::stringw IRR_XML_FORMAT_MATERIALS;
        const irr::core::stringw IRR_XML_FORMAT_ANIMATORS;
        const irr::core::stringw IRR_XML_FORMAT_USERDATA;

        const wchar_t* convert(const irr::c8 *s) {
          return irr::core::stringw(irr::core::stringc(s)).c_str();
        }

        void logError(const std::wstring &a_pMessage, irr::ELOG_LEVEL a_eLevel) {
          if (m_pLogger != nullptr)
            m_pLogger->log(a_pMessage.c_str(), a_eLevel);
        }

        //! Recursively reads nodes from the xml file
        void readSceneNode(irr::io::IXMLReader* reader, irr::scene::ISceneNode* parent, irr::scene::ISceneUserDataSerializer* userDataSerializer) {
          if (!reader)
            return;

          irr::scene::ISceneNode* node = 0;

          if (!parent && IRR_XML_FORMAT_SCENE==reader->getNodeName())
            node = SceneManager->getRootSceneNode();
          else if (parent && IRR_XML_FORMAT_NODE==reader->getNodeName())
          {
            if (m_pCallback != nullptr)
              m_pCallback->progressInc();

            // find node type and create it
            irr::core::stringc attrName = reader->getAttributeValue(IRR_XML_FORMAT_NODE_ATTR_TYPE.c_str());

            node = SceneManager->addSceneNode(attrName.c_str(), parent);

            if (!node) {
              if (m_pLogger != nullptr)
                logError((std::wstring(L"Could not create scene node of unknown type \"") + convert(attrName.c_str()) + L"\"").c_str(), irr::ELL_WARNING);
            }
          }
          else
            node=parent;

          // read attributes
          while(reader->read())
          {
            bool endreached = false;

            const wchar_t* name = reader->getNodeName();

            switch (reader->getNodeType())
            {
            case irr::io::EXN_ELEMENT_END:
              if ((IRR_XML_FORMAT_NODE  == name) ||
                (IRR_XML_FORMAT_SCENE == name))
              {
                endreached = true;
              }
              break;
            case irr::io::EXN_ELEMENT:
              if (IRR_XML_FORMAT_ATTRIBUTES == name)
              {
                // read attributes
                irr::io::IAttributes* attr = FileSystem->createEmptyAttributes(SceneManager->getVideoDriver());
                attr->read(reader, true);

                if (node)
                  node->deserializeAttributes(attr);

                attr->drop();
              }
              else
                if (IRR_XML_FORMAT_MATERIALS == name)
                  readMaterials(reader, node);
                else
                  if (IRR_XML_FORMAT_ANIMATORS == name)
                    readAnimators(reader, node);
                  else
                    if (IRR_XML_FORMAT_USERDATA  == name)
                      readUserData(reader, node, userDataSerializer);
                    else
                      if ((IRR_XML_FORMAT_NODE  == name) ||
                        (IRR_XML_FORMAT_SCENE == name))
                      {
                        readSceneNode(reader, node, userDataSerializer);
                      }
                      else
                      {
                        if (m_pLogger != nullptr)
                          m_pLogger->log(L"Found unknown element in irrlicht scene file", irr::ELL_WARNING);
                      }
              break;
            default:
              break;
            }

            if (endreached)
              break;
          }
          if (node && userDataSerializer)
            userDataSerializer->OnCreateNode(node);
        }


        //! read a node's materials
        void readMaterials(irr::io::IXMLReader* reader, irr::scene::ISceneNode* node){
          irr::u32 nr = 0;

          while(reader->read())
          {
            const wchar_t* name = reader->getNodeName();

            switch(reader->getNodeType())
            {
            case irr::io::EXN_ELEMENT_END:
              if (IRR_XML_FORMAT_MATERIALS == name)
                return;
              break;
            case irr::io::EXN_ELEMENT:
              if (IRR_XML_FORMAT_ATTRIBUTES == name)
              {
                // read materials from attribute list
                irr::io::IAttributes* attr = FileSystem->createEmptyAttributes(SceneManager->getVideoDriver());
                attr->read(reader);

                if (node && node->getMaterialCount() > nr)
                {
                  SceneManager->getVideoDriver()->fillMaterialStructureFromAttributes(
                    node->getMaterial(nr), attr);
                }

                attr->drop();
                ++nr;
              }
              break;
            default:
              break;
            }
          }
        }

        //! read a node's animators
        void readAnimators(irr::io::IXMLReader* reader, irr::scene::ISceneNode* node){
          while(reader->read())
          {
            const wchar_t* name = reader->getNodeName();

            switch(reader->getNodeType())
            {
            case irr::io::EXN_ELEMENT_END:
              if (IRR_XML_FORMAT_ANIMATORS == name)
                return;
              break;
            case irr::io::EXN_ELEMENT:
              if (IRR_XML_FORMAT_ATTRIBUTES == name)
              {
                // read animator data from attribute list
                irr::io::IAttributes* attr = FileSystem->createEmptyAttributes(SceneManager->getVideoDriver());
                attr->read(reader);

                if (node)
                {
                  irr::core::stringc typeName = attr->getAttributeAsString("Type");
                  irr::scene::ISceneNodeAnimator* anim = SceneManager->createSceneNodeAnimator(typeName.c_str(), node);

                  if (anim)
                  {
                    anim->deserializeAttributes(attr);
                    anim->drop();
                  }
                }

                attr->drop();
              }
              break;
            default:
              break;
            }
          }
        }


        //! read any other data into the user serializer
        void readUserData(irr::io::IXMLReader* reader, irr::scene::ISceneNode* node, irr::scene::ISceneUserDataSerializer* userDataSerializer){
          while(reader->read())
          {
            const wchar_t* name = reader->getNodeName();

            switch(reader->getNodeType())
            {
            case irr::io::EXN_ELEMENT_END:
              if (IRR_XML_FORMAT_USERDATA == name)
                return;
              break;
            case irr::io::EXN_ELEMENT:
              if (IRR_XML_FORMAT_ATTRIBUTES == name)
              {
                // read user data from attribute list
                irr::io::IAttributes* attr = FileSystem->createEmptyAttributes(SceneManager->getVideoDriver());
                attr->read(reader);

                if (node && userDataSerializer)
                {
                  userDataSerializer->OnReadUserData(node, attr);
                }

                attr->drop();
              }
              break;
            default:
              break;
            }
          }
        }


      public:
        //! Constructor
        CDustbinSceneLoader(irr::scene::ISceneManager *smgr, irr::io::IFileSystem* fs, irr::ILogger *a_pLogger, gui::IProgressCallback *a_pCallback) :
          SceneManager                 (smgr), 
          FileSystem                   (fs),
          m_pLogger                    (a_pLogger),
          m_pCallback                  (a_pCallback),
          m_iNodeCnt                   (0),
          IRR_XML_FORMAT_SCENE         (L"irr_scene"), 
          IRR_XML_FORMAT_NODE          (L"node"), 
          IRR_XML_FORMAT_NODE_ATTR_TYPE(L"type"),
          IRR_XML_FORMAT_ATTRIBUTES    (L"attributes"), 
          IRR_XML_FORMAT_MATERIALS     (L"materials"),
          IRR_XML_FORMAT_ANIMATORS     (L"animators"), 
          IRR_XML_FORMAT_USERDATA(     L"userData")
        {
        }

        //! Destructor
        virtual ~CDustbinSceneLoader() {
        }

        //! Returns true if the class might be able to load this file.
        virtual bool isALoadableFileExtension(const irr::io::path& filename) const IRR_OVERRIDE {
          return irr::core::hasFileExtension(filename, "irr");
        }

        //! Returns true if the class might be able to load this file.
        virtual bool isALoadableFileFormat(irr::io::IReadFile* file) const IRR_OVERRIDE {
          // todo: check inside the file
          return true;
        }

        //! Loads the scene into the scene manager.
        virtual bool loadScene(irr::io::IReadFile* file, irr::scene::ISceneUserDataSerializer* userDataSerializer = 0, irr::scene::ISceneNode* rootNode = 0) IRR_OVERRIDE {
          if (!file)
          {
            // irr::os::Printer::log("Unable to open scene file", ELL_ERROR);
            return false;
          }

          irr::io::IXMLReader* reader = FileSystem->createXMLReader(file);
          if (!reader)
          {
            if (m_pLogger != nullptr)
              m_pLogger->log((std::wstring(L"Scene is not a valid XML file (") + convert(file->getFileName().c_str()) + L")").c_str(), irr::ELL_ERROR);
            return false;
          }

          while (reader->read()) {
            if (reader->getNodeType() == irr::io::EXN_ELEMENT && IRR_XML_FORMAT_NODE == reader->getNodeName())
              m_iNodeCnt++;
          }
          reader->drop();

          if (m_pCallback != nullptr) {
            m_pCallback->progressSetCurrentRange(
              m_pCallback->progressGetMessage(),
              m_pCallback->progressRangeMin(),
              m_pCallback->progressRangeMax(),
              m_iNodeCnt
            );
          }

          file->seek(0);
          reader = FileSystem->createXMLReader(file);

          // TODO: COLLADA_CREATE_SCENE_INSTANCES can be removed when the COLLADA loader is a scene loader
          bool oldColladaSingleMesh = SceneManager->getParameters()->getAttributeAsBool(irr::scene::COLLADA_CREATE_SCENE_INSTANCES);
          SceneManager->getParameters()->setAttribute(irr::scene::COLLADA_CREATE_SCENE_INSTANCES, false);

          // read file
          while (reader->read())
          {
            if (m_pCallback != nullptr)
              m_pCallback->progressInc();

            readSceneNode(reader, rootNode, userDataSerializer);
          }

          // restore old collada parameters
          SceneManager->getParameters()->setAttribute(irr::scene::COLLADA_CREATE_SCENE_INSTANCES, oldColladaSingleMesh);

          // clean up
          reader->drop();
          return true;
        }

    };

    /**
    * This function loads a scene to a given scene manager reporting loading progress to a progress callback
    * @param a_sFileName name of the file to load
    * @param a_pSmgr the scene manager to load the scene to
    * @param a_pFs the Irrlicht file system
    * @param a_pLogger the Irrlicht logger
    * @param a_pProgress the progress callback to call
    * @return "true" if the scene was loaded successfully, "false" otherwise
    */
    bool loadScene(const std::string& a_sFileName, irr::scene::ISceneManager* a_pSmgr, irr::io::IFileSystem* a_pFs, irr::ILogger* a_pLogger, gui::IProgressCallback* a_pProgress) {
      CDustbinSceneLoader l_cLoader = CDustbinSceneLoader(a_pSmgr, a_pFs, a_pLogger, a_pProgress);

      irr::io::IReadFile *l_pScene = a_pFs->createAndOpenFile(a_sFileName.c_str());
      bool l_bRet = l_cLoader.loadScene(l_pScene);
      l_pScene->drop();
      return l_bRet;
    }
  }
}