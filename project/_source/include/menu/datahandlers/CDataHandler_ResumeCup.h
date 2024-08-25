// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
// #pragma once

#include <menu/datahandlers/IMenuDataHandler.h>
#include <irrlicht.h>
#include <vector>

namespace dustbin {
  namespace gameclasses {
    struct STournament;       /**< Forward declaration of the STournament struct */
    struct SRace;             /**< Forward declaration of the SRace struct */
  }

  namespace menu {
    class CDataHandler_ResumeCup : public IMenuDataHandler {
      private:
        gameclasses::STournament *m_pTournament;

        irr::u32 m_iNextChange;

        std::vector<gameclasses::SRace *>::iterator m_itRace;

        void updateTrackImage();

      public:
        CDataHandler_ResumeCup();
        virtual ~CDataHandler_ResumeCup();

        /**
        * This is the main method of this class. Irrlicht events
        * get forwarded and may be handled by the handler
        * @param a_cEvent the event to handle
        * @return true if the event was handled, false otherwise
        */
        virtual bool handleIrrlichtEvent(const irr::SEvent &a_cEvent) override;

        /**
        * Gets called every frame, maybe it's necessary to implement something
        */
        virtual void runDataHandler() override;

        /**
        * Get the tournament to be resumed
        * @return the tournament to be resumed
        */
        gameclasses::STournament *getTournament();
    };
  }
}
