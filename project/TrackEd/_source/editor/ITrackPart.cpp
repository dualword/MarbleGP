// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#include <editor/ITrackPart.h>
#include <IGlobal.h>

namespace dustbin {
  namespace tracked {
    /**
    * The constructor
    * @param a_sType the type identifier
    * @param a_cPosition the starting position of the part
    */
    ITrackPart::ITrackPart() :
      m_bSelected(false)
    {
    }

    /**
    * The destructor
    */
    ITrackPart::~ITrackPart() {
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
  }
}