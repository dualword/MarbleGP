// (w) 2024 by Dustbin::Games / Christian Keimel
#pragma once

#include <commands/ICommand.h>

namespace dustbin {
  namespace shaders {
    class CDustbinShaders;  /**< Forward declaration of the Dustbin Shaders class */
  }

  namespace commands {
    /**
    * @class CDustbinRenderer
    * @author Christian Keimel
    * This class takes care of rendering with the DustbinGames MarbleGP Shader
    */
    class CCmdDustbinRenderer : public ICommand {
      private:
        shaders::CDustbinShaders  *m_pShader;     /**< The Dustbin Shaders instance */
        irr::IrrlichtDevice       *m_pDevice;     /**< The Irrlicht device */
        irr::video::IVideoDriver  *m_pDrv;        /**< The Irrlicht video driver */
        irr::scene::ISceneManager *m_pSmgr;       /**< The Irrlicht scene manager */
        irr::gui::IGUIEnvironment *m_pGui;        /**< The Irrlicht GUI environment */

        irr::scene::ISceneNode *findNodeByType(irr::scene::ISceneNode *a_pParent, irr::scene::ESCENE_NODE_TYPE a_eType);

        irr::scene::ISceneNode *findNodeByName(irr::scene::ISceneNode *a_pParent, const std::string &a_sName);

      public:
        CCmdDustbinRenderer(irr::IrrlichtDevice *a_pDevice);
        virtual ~CCmdDustbinRenderer();

        /**
        * If this method returns "true" the command is finished and
        * will be removed from the vector of active commands
        * @return true if the command is finished and can be removed
        */
        virtual bool isFinished() override;

        /**
        * This method is called on every frame.
        */
        virtual void run() override;

        /**
        * The Irrlicht event handler method. Implement it if the command
        * needs to react to user input
        */
        virtual bool OnEvent(const irr::SEvent &a_cEvent) override;
    };
  }
}
