// (w) 2024 by Dustbin::Games / Christian Keimel
#pragma once

#include <commands/ICommand.h>

namespace dustbin {
  namespace commands {
    /**
    * @class CCmdDecoration
    * @author Christian Keimel
    * This command allows the modification of track decoration. Track decoration
    * is just optical, it has no physics equivalent
    */
    class CCmdDecoration : public ICommand {
      private:
        enum class enButtonIDs {
          Close,
          Add,
          Delete
        };

        bool m_bFinished;   /**< Is the command finished? */

        irr::scene::ISceneManager *m_pSmgr;       /**< The scene manager */
        irr::gui::IGUIEnvironment *m_pGui;        /**< The GUI environment */
        irr::gui::IGUITab         *m_pBack;       /**< The background */
        irr::gui::IGUITab         *m_pParent;     /**< The parent of the dialog */
        irr::gui::IGUIListBox     *m_pList;       /**< The list of decorations */
        irr::scene::ISceneNode    *m_pNode;       /**< The root scene node for decorations */

        /**
        * Find the decoration scene node
        * @param a_pNode the node to check (all children will be checked as well)
        * @return the decoration scene node
        */
        irr::scene::ISceneNode *getDecorationRoot(irr::scene::ISceneNode *a_pNode);

        /**
        * Update the list of decoration objects
        */
        void updateDecorationList();

      public:
        CCmdDecoration(irr::scene::ISceneManager *a_pSmgr, irr::gui::IGUIEnvironment *a_pGui);
        virtual ~CCmdDecoration();

        /**
        * If this method returns "true" the command is finished and
        * will be removed from the vector of active commands
        * @return true if the command is finished and can be removed
        */
        virtual bool isFinished() override;

        /**
        * The Irrlicht event handler method. Implement it if the command
        * needs to react to user input
        */
        virtual bool OnEvent(const irr::SEvent &a_cEvent) override;
    };
  }
}
