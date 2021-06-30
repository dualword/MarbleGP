// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

#include <gui/CMenuButton.h>

namespace dustbin {
  namespace gui {
    /**
    * @class CGuiItemFactory
    * @author Christian Keimel
    * This is the GUI factory that creates all custom GUI elements
    */
    class CGuiItemFactory : public irr::gui::IGUIElementFactory {
			private:
				irr::gui::IGUIEnvironment* m_pGui;

      public:
				CGuiItemFactory(irr::gui::IGUIEnvironment *a_pGui);
				virtual ~CGuiItemFactory();

				//! adds an element to the gui environment based on its type id
				/** \param type: Type of the element to add.
				\param parent: Parent scene node of the new element, can be null to add to the root.
				\return Pointer to the new element or null if not successful. */
				virtual irr::gui::IGUIElement* addGUIElement(irr::gui::EGUI_ELEMENT_TYPE a_eType, irr::gui::IGUIElement* a_pParent = 0);

				//! adds a GUI element to the GUI Environment based on its type name
				/** \param typeName: Type name of the element to add.
				\param parent: Parent scene node of the new element, can be null to add it to the root.
				\return Pointer to the new element or null if not successful. */
				virtual irr::gui::IGUIElement* addGUIElement(const irr::c8* a_sTypeName, irr::gui::IGUIElement* a_pParent = 0);

				//! Get amount of GUI element types this factory is able to create
				virtual irr::s32 getCreatableGUIElementTypeCount() const;

				//! Get type of a createable element type
				/** \param idx: Index of the element type in this factory. Must be a value between 0 and
				getCreatableGUIElementTypeCount() */
				virtual irr::gui::EGUI_ELEMENT_TYPE getCreateableGUIElementType(irr::s32 a_iIdx) const;

				//! Get type name of a createable GUI element type by index
				/** \param idx: Index of the type in this factory. Must be a value between 0 and
				getCreatableGUIElementTypeCount() */
				virtual const irr::c8* getCreateableGUIElementTypeName(irr::s32 a_iIdx) const;

				//! returns type name of a createable GUI element
				/** \param type: Type of GUI element.
				\return Name of the type if this factory can create the type, otherwise 0. */
				virtual const irr::c8* getCreateableGUIElementTypeName(irr::gui::EGUI_ELEMENT_TYPE a_eType) const;
		};
  }
}

