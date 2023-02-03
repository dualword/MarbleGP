// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

namespace dustbin {
  namespace gui {
    /**
    * @class IGuiControllerUiCallback
    * @author Christian Keimel
    * This interface is a callback for the controller UI getting
    * notified when the controller UI enters editing a control
    */
    class IGuiControllerUiCallback {
      public:
        virtual ~IGuiControllerUiCallback() {
        }

        /**
        * The callback notified when the controller configuration changes
        * @param a_bEditing is the change because editing is started or not
        */
        virtual void editingController(bool a_bEditing) = 0;
    };
  }
}
