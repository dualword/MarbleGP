// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <gui/CDustbinCheckbox.h>
#include <gui/CMenuBackground.h>
#include <gui/CGuiItemFactory.h>
#include <gui/CReactiveLabel.h>
#include <gui/CGuiImageList.h>
#include <gui/CControllerUi.h>
#include <gui/CClipImage.h>
#include <gui/CSelector.h>
#include <string>

#ifdef _TOUCH_CONTROL
#include <gui/CGuiTouchControl.h>
#endif

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
				irr::gui::IGUIElement* p = new CMenuButton(a_pParent != nullptr ? a_pParent : m_pGui->getRootGUIElement());
				p->drop();
				return p;
			}
			else if (l_sTypeName == g_MenuBackgroundName) {
				irr::gui::IGUIElement* p = new CMenuBackground(a_pParent != nullptr ? a_pParent : m_pGui->getRootGUIElement());
				p->drop();
				return p;
			}
			else if (l_sTypeName == g_ControllerUiName) {
				irr::gui::IGUIElement* p = new gui::CControllerUi(a_pParent != nullptr ? a_pParent : m_pGui->getRootGUIElement());
				p->drop();
				return p;
			}
			else if (l_sTypeName == g_DustbinCheckboxName) {
				irr::gui::IGUIElement* p = new gui::CDustbinCheckbox(a_pParent != nullptr ? a_pParent : m_pGui->getRootGUIElement());
				p->drop();
				return p;
			}
			else if (l_sTypeName == g_SelectorName) {
				irr::gui::IGUIElement* p = new gui::CSelector(a_pParent != nullptr ? a_pParent : m_pGui->getRootGUIElement());
				p->drop();
				return p;
			}
			else if (l_sTypeName == g_ReactiveLabelName) {
				irr::gui::IGUIElement* p = new gui::CReactiveLabel(a_pParent != nullptr ? a_pParent : m_pGui->getRootGUIElement());
				p->drop();
				return p;
			}
			else if (l_sTypeName == g_ClipImageName) {
				irr::gui::IGUIElement* p = new gui::CClipImage(a_pParent != nullptr ? a_pParent : m_pGui->getRootGUIElement());
				p->drop();
				return p;
			}
      else if (l_sTypeName == g_ImageListName) {
        irr::gui::IGUIElement *p = new gui::CGuiImageList(a_pParent != nullptr ? a_pParent : m_pGui->getRootGUIElement());
        p->drop();
        return p;
      }
#ifdef _TOUCH_CONTROL
      else if (l_sTypeName == g_TouchControlName) {
        irr::gui::IGUIElement *p = new gui::CGuiTouchControl(a_pParent != nullptr ? a_pParent : m_pGui->getRootGUIElement());
        p->drop();
        return p;
      }
#endif

			return nullptr;
		}

		//! Get amount of GUI element types this factory is able to create
		irr::s32 CGuiItemFactory::getCreatableGUIElementTypeCount() const {
#ifdef _TOUCH_CONTROL
      return 9;
#else
			return 8;
#endif
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

				case 2:
					return (irr::gui::EGUI_ELEMENT_TYPE)g_ControllerUiId;
					break;

				case 3:
					return (irr::gui::EGUI_ELEMENT_TYPE)g_DustbinCheckboxId;
					break;

				case 4:
					return (irr::gui::EGUI_ELEMENT_TYPE)g_SelectorId;
					break;

				case 5:
					return (irr::gui::EGUI_ELEMENT_TYPE)g_ReactiveLabelId;
					break;

				case 6:
					return (irr::gui::EGUI_ELEMENT_TYPE)g_ClipImageId;
					break;

        case 7:
          return (irr::gui::EGUI_ELEMENT_TYPE)g_ImageListId;
          break;
#ifdef _TOUCH_CONTROL
        case 8:
          return (irr::gui::EGUI_ELEMENT_TYPE)g_TouchControlId;
          break;
#endif

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

				case 2:
					return g_ControllerUiName;
					break;

				case 3:
					return g_DustbinCheckboxName;
					break;

				case 4:
					return g_SelectorName;
					break;

				case 5:
					return g_ReactiveLabelName;
					break;

				case 6:
					return g_ClipImageName;
					break;

        case 7:
          return g_ImageListName;
          break;
#ifdef _TOUCH_CONTROL
        case 8:
          return g_TouchControlName;
          break;
#endif

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
        case (irr::gui::EGUI_ELEMENT_TYPE)g_MenuButtonId:
					return g_MenuButtonName;
					break;

				case (irr::gui::EGUI_ELEMENT_TYPE)g_MenuBackgroundId:
					return g_MenuBackgroundName;
					break;

				case (irr::gui::EGUI_ELEMENT_TYPE)g_ControllerUiId:
					return g_ControllerUiName;
					break;

				case (irr::gui::EGUI_ELEMENT_TYPE)g_DustbinCheckboxId:
					return g_DustbinCheckboxName;
					break;

				case (irr::gui::EGUI_ELEMENT_TYPE)g_SelectorId:
					return g_SelectorName;
					break;

				case (irr::gui::EGUI_ELEMENT_TYPE)g_ReactiveLabelId:
					return g_ReactiveLabelName;
					break;

				case (irr::gui::EGUI_ELEMENT_TYPE)g_ClipImageId:
					return g_ClipImageName;
					break;

        case (irr::gui::EGUI_ELEMENT_TYPE)g_ImageListId:
          return g_ImageListName;
          break;

#ifdef _TOUCH_CONTROL
        case (irr::gui::EGUI_ELEMENT_TYPE)g_TouchControlId:
          return g_TouchControlName;
          break;
#endif

				default:
					return nullptr;
			}
		}

  }
}