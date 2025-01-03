// (w) 2024 by Dustbin::Games / Christian Keimel
#pragma once

#include <commands/ICommand.h>
#include <vector>
#include <tuple>
#include <map>

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
        /**
        * @class CInputLimiter
        * @author Christian Keimel
        * This is a helper class which consumes key events
        * for keys which cannot be entered to an input field
        */
        class CInputLimiter {
          private:
            std::wstring m_sLimited;    /**< Some defined valid values */

            std::vector<irr::EKEY_CODE> m_vValid;   /**< Some default values which are never consumed */

            irr::gui::IGUIElement *m_pElement;    /**< The GUI element we limit the input for */

          public:
            CInputLimiter(irr::gui::IGUIElement *a_pElement, irr::io::E_ATTRIBUTE_TYPE a_eType);
            virtual ~CInputLimiter();

            bool consumeEvent(const irr::SEvent &a_cEvent);

            irr::gui::IGUIElement *getElement();
        };

        bool m_bFinished;   /**< Is the command finished? */

        irr::scene::ISceneManager *m_pSmgr;           /**< The scene manager */
        irr::gui::IGUIEnvironment *m_pGui;            /**< The GUI environment */
        irr::scene::ISceneNode    *m_pNode;           /**< The root scene node for decorations */
        irr::scene::ISceneNode    *m_pSelected;       /**< The node selected in the tree view */
        irr::gui::IGUIWindow      *m_pDialog;         /**< The dialog window */
        irr::gui::IGUITreeView    *m_pScene;          /**< The tree view representing the scene */
        irr::gui::IGUITab         *m_aTabs[2];        /**< The tabs for scene editing */
        irr::gui::IGUITab         *m_aNodeTabs[3];    /**< The tabs for node stuff */
        irr::gui::IGUIElement     *m_aParameters[3];  /**< The parent elements for the parameters */
        irr::gui::IGUITabControl  *m_pTabMain;        /**< The main tab control */
        irr::gui::IGUITabControl  *m_pTabNode;        /**< The node tab control */
        irr::gui::IGUIComboBox    *m_pMaterial;       /**< Combobox for material selection */
        irr::gui::IGUIImage       *m_aTexture[8];     /**< The textures of the selected material */
        irr::gui::IGUIStaticText  *m_pMeshName;       /**< Static text displaying the mesh name */

        std::vector<CInputLimiter *> m_vLimiters;     /**< The input limiter */

        std::map<irr::gui::IGUITreeViewNode *, irr::scene::ISceneNode *> m_mNodeMap;  /**< Map mapping tree view nodes to scene nodes */

        /**
        * The list of node attributes (0 == name of the attribute, 1 == Label, 2 == Value Edit, 3 == attribute type)
        */
        std::vector<std::tuple<std::string, irr::gui::IGUIStaticText *, irr::gui::IGUIElement *, irr::io::E_ATTRIBUTE_TYPE>> m_vAttributes[3];

        /**
        * Get the type name of a scene node
        * @param a_pNode the scene node
        * @return the type name
        */
        std::string getTypeName(irr::scene::ISceneNode *a_pNode);

        /**
        * Create and fill an IAttributes object depending on the selected node tab
        * @return an IAttributes object or nullptr if something wasn't ok
        */
        irr::io::IAttributes *getAttributesForEditing();

        /**
        * Commit attributes depending on the selected node tab
        * @param a_pAttr the attributes to commit
        */
        void commitAttributes(irr::io::IAttributes *a_pAttr);

        /**
        * Find the decoration scene node
        * @param a_pNode the node to check (all children will be checked as well)
        * @return the decoration scene node
        */
        irr::scene::ISceneNode *getDecorationRoot(irr::scene::ISceneNode *a_pNode);

        /**
        * Update the list of decoration objects
        * @param a_pNode the node to add
        * @param a_pParent the parent tree view node
        */
        void updateDecorationList(irr::scene::ISceneNode *a_pNode, irr::gui::IGUITreeViewNode *a_pParent);

        /**
        * Update the attribute UI from the given attributes
        * @param a_pAttr the attributes to use
        * @param a_iIndex the tab index (0 == properties, 1 == materials, 2 == animators)
        * @param a_pParent the parent UI element to add the attributes to
        * @param a_cPos the start position to add the UI element
        */
        void updateAttributeList(irr::io::IAttributes *a_pAttr, int a_iIndex, irr::gui::IGUIElement *a_pParent, const irr::core::position2di &a_cPos);

        /**
        * Update a scene node in the tree
        * @param a_pNode the node to update
        */
        void updateNodeInTree(irr::scene::ISceneNode *a_pNode);

        /**
        * A scene node will be deleted
        * @param a_pNode the node to be deleted
        */
        void deleteNode(irr::scene::ISceneNode *a_pNode);

        /**
        * A scene node was added
        * @param a_pNode the new node
        */
        void addNode(irr::scene::ISceneNode *a_pNode);

        /**
        * Commit a text-input value to the attributes
        * @param a_sName name of the attribute
        * @param a_pEdit the edit field to get the value from
        * @param a_pAttr the attributes to modify
        */
        void commitTextInputValue(const std::string &a_sName, irr::gui::IGUIEditBox *a_pEdit, irr::io::IAttributes *a_pAttr);

        /**
        * Commit a text-input value to the attributes
        * @param a_sName name of the attribute
        * @param a_pEdit the edit field to get the value from
        * @param a_pAttr the attributes to modify
        */
        void revertTextInputValue(const std::string &a_sName, irr::gui::IGUIEditBox *a_pEdit, irr::io::IAttributes *a_pAttr);

        /**
        * Update the material tab
        */
        void updateMaterialTab();

        /**
        * Update the dropdown for the material selection with the data taken from "m_pSelected"
        */
        void updateMaterialDropdown();

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

        /**
        * Callback method for changes in the UI like modifying
        * font size or resizing window
        */
        virtual void onUiSetupModified();
    };
  }
}
