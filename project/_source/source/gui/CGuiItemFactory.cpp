// (w) 2021 by Dustbin::Games / Christian Keimel
#include <gui/CMenuBackground.h>
#include <gui/CGuiItemFactory.h>
#include <string>

namespace dustbin {
  namespace gui {
		CGuiItemFactory::CGuiItemFactory(irr::gui::IGUIEnvironment* a_pGui) : m_pGui(a_pGui) {

		}

		CGuiItemFactory::~CGuiItemFactory() {
		}

		//! adds an element to the gui environment based on its type id
		/** \param type: Type of the element to add.
		\param parent: Parent scene node of the new element, can be null to add to the root.
		\return Pointer to the new element or null if not successful. */
		irr::gui::IGUIElement* CGuiItemFactory::addGUIElement(irr::gui::EGUI_ELEMENT_TYPE a_eType, irr::gui::IGUIElement* a_pParent) {
			return addGUIElement(getCreateableGUIElementTypeName(a_eType), a_pParent);
		}

		//! adds a GUI element to the GUI Environment based on its type name
		/** \param typeName: Type name of the element to add.
		\param parent: Parent scene node of the new element, can be null to add it to the root.
		\return Pointer to the new element or null if not successful. */
		irr::gui::IGUIElement* CGuiItemFactory::addGUIElement(const irr::c8* a_sTypeName, irr::gui::IGUIElement* a_pParent) {
			std::string l_sTypeName = std::string(a_sTypeName);

			if (l_sTypeName == g_MenuButtonName) {
				irr::gui::IGUIElement* p = new CMenuButton(a_pParent);
				p->drop();
				return p;
			}
			else if (l_sTypeName == g_MenuBackgroundName) {
				irr::gui::IGUIElement* p = new CMenuBackground(a_pParent);
				p->drop();
				return p;
			}
			else
				return nullptr;
		}

		//! Get amount of GUI element types this factory is able to create
		irr::s32 CGuiItemFactory::getCreatableGUIElementTypeCount() const {
			return 2;
		}

		//! Get type of a createable element type
		/** \param idx: Index of the element type in this factory. Must be a value between 0 and
		getCreatableGUIElementTypeCount() */
		irr::gui::EGUI_ELEMENT_TYPE CGuiItemFactory::getCreateableGUIElementType(irr::s32 a_iIdx) const {
			switch (a_iIdx) {
				case 0:
					return (irr::gui::EGUI_ELEMENT_TYPE)g_MenuButtonId;
					break;

				case 1:
					return (irr::gui::EGUI_ELEMENT_TYPE)g_MenuBackgroundId;
					break;

				default:
					return irr::gui::EGUIET_COUNT;
					break;
			}
		}

		//! Get type name of a createable GUI element type by index
		/** \param idx: Index of the type in this factory. Must be a value between 0 and
		getCreatableGUIElementTypeCount() */
		const irr::c8* CGuiItemFactory::getCreateableGUIElementTypeName(irr::s32 a_iIdx) const {
			switch (a_iIdx) {
				case 0:
					return g_MenuButtonName;
					break;

				case 1:
					return g_MenuBackgroundName;
					break;

				default:
					return nullptr;
					break;
			}
		}

		//! returns type name of a createable GUI element
		/** \param type: Type of GUI element.
		\return Name of the type if this factory can create the type, otherwise 0. */
		const irr::c8* CGuiItemFactory::getCreateableGUIElementTypeName(irr::gui::EGUI_ELEMENT_TYPE a_eType) const {
			switch (a_eType) {
				case g_MenuButtonId:
					return g_MenuButtonName;
					break;

				case g_MenuBackgroundId:
					return g_MenuBackgroundName;
					break;

				default:
					return nullptr;
			}
		}

  }
}