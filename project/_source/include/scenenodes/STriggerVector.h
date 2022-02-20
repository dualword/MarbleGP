/* (w) 2021 - 22 by Dustbin::Games (Christian Keimel) - This file is licensed under the terms of the zlib license */
#pragma once

#include <scenenodes/STriggerAction.h>
#include <vector>

namespace dustbin {
  namespace scenenodes {
    struct STriggerVector {
      std::vector<STriggerAction>           m_vActions;
      std::vector<STriggerAction>::iterator m_itAction;

      STriggerVector(const std::vector<STriggerAction>& a_vActions) {
        for (std::vector<STriggerAction>::const_iterator it = a_vActions.begin(); it != a_vActions.end(); it++)
          m_vActions.push_back(*it);

        m_itAction = m_vActions.begin();
      }
    };
  }
}
