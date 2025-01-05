
#include <helpers/CInputLimiter.h>

namespace dustbin {
  namespace helpers {
    CInputLimiter::CInputLimiter(irr::gui::IGUIElement *a_pElement, irr::io::E_ATTRIBUTE_TYPE a_eType) : m_pElement(a_pElement) {
      switch (a_eType) {
	      case irr::io::EAT_INT         : m_sLimited = L"+-0123456789" ; break; 
	      case irr::io::EAT_FLOAT       : m_sLimited = L"+-0123456789."; break; 
        case irr::io::EAT_STRING      : break;
	      case irr::io::EAT_BOOL        : break;
	      case irr::io::EAT_COLOR       : m_sLimited = L"+-0123456789," ; break;
	      case irr::io::EAT_COLORF      : m_sLimited = L"+-0123456789.,"; break;
	      case irr::io::EAT_VECTOR3D    : m_sLimited = L"+-0123456789.,"; break;
	      case irr::io::EAT_POSITION2D  : m_sLimited = L"+-0123456789," ; break;
	      case irr::io::EAT_VECTOR2D    : m_sLimited = L"+-0123456789.,"; break;
	      case irr::io::EAT_RECT        : break;
	      case irr::io::EAT_MATRIX      : break;
	      case irr::io::EAT_QUATERNION  : break;
	      case irr::io::EAT_BBOX        : break;
	      case irr::io::EAT_PLANE       : break;
	      case irr::io::EAT_TRIANGLE3D  : break;
	      case irr::io::EAT_LINE2D      : break;
	      case irr::io::EAT_LINE3D      : break;
	      case irr::io::EAT_STRINGWARRAY: break;
	      case irr::io::EAT_FLOATARRAY  : break;
	      case irr::io::EAT_INTARRAY    : break;
	      case irr::io::EAT_BINARY      : break;
	      case irr::io::EAT_TEXTURE     : break;
	      case irr::io::EAT_USER_POINTER: break;
        case irr::io::EAT_DIMENSION2D : m_sLimited = L"+-0123456789,"; break;
      }

      m_vValid.push_back(irr::KEY_RETURN);
      m_vValid.push_back(irr::KEY_ESCAPE);
      m_vValid.push_back(irr::KEY_RIGHT);
      m_vValid.push_back(irr::KEY_LEFT);
      m_vValid.push_back(irr::KEY_DOWN);
      m_vValid.push_back(irr::KEY_UP);
      m_vValid.push_back(irr::KEY_END);
      m_vValid.push_back(irr::KEY_BACK);
      m_vValid.push_back(irr::KEY_HOME);
      m_vValid.push_back(irr::KEY_SHIFT);
      m_vValid.push_back(irr::KEY_DELETE);
    }

    CInputLimiter::~CInputLimiter() {
    }

    bool CInputLimiter::consumeEvent(const irr::SEvent &a_cEvent) {
      if (a_cEvent.EventType == irr::EET_KEY_INPUT_EVENT) {
        // Don't consume keys which are never invalid
        if (std::find(m_vValid.begin(), m_vValid.end(), a_cEvent.KeyInput.Key) != m_vValid.end())
          return false;

        if (a_cEvent.KeyInput.Control && a_cEvent.KeyInput.Key == irr::KEY_KEY_A)
          return false;

        // Only chars not found in the string are valid, i.e. not consumed
        if (m_sLimited.size() > 0 && m_sLimited.find_first_of(a_cEvent.KeyInput.Char) == std::string::npos)
          return true;
      }

      return false;
    }

    irr::gui::IGUIElement *CInputLimiter::getElement() {
      return m_pElement;
    }
  }
}