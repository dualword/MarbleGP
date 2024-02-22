// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>
#include <string>

namespace dustbin {
  namespace gui {
    /**
    * @class IProgressCallback
    * @author Christian Keimel
    * This is the interface that needs to be implemented for loading progress callbacks
    */
    class IProgressCallback {
      private:
        std::wstring m_sMessage;    /**< The message of the current progress range */

        irr::u32 m_iCurrentMin;     /**< The minimum value of the current range (0..100) */
        irr::u32 m_iCurrentMax;     /**< The maximum value of the current range (0..100) */
        irr::u32 m_iRange;          /**< The current range (m_iCurrentMax - m_iCurrentMin) */
        irr::u32 m_iCurrent;        /**< The current value which can be used to count the number of items processed */
        irr::u32 m_iCount;          /**< The number of items of the current range */
        irr::s32 m_iValue;          /**< The current caluclated progress value */

      public:
        IProgressCallback();

        virtual ~IProgressCallback();

        /**
        * Set the current progress range
        * @param a_sMessage the message to be shown
        * @param a_iMin the min value of the current range
        * @param a_iMax the max value of the current range
        */
        void progressSetCurrentRange(const wchar_t *a_sMessage, irr::u32 a_iMin, irr::u32 a_iMax, irr::u32 a_iCount);

        /**
        * Get the min of the current range
        * @return the min of the current range
        */
        irr::u32 progressRangeMin();

        /**
        * Get the max of the current range
        * @return the max of the current range
        */
        irr::u32 progressRangeMax();

        /**
        * Get the current message
        * @return the current message
        */
        const wchar_t *progressGetMessage();

        /**
        * Update the current value
        */
        void progressInc();

        /**
        * This method is called when a progress update needs to be reported
        * @param a_iProgress the progress ranging from 0 to 100
        * @param a_sMessage the message
        */
        virtual void onProgress(irr::u32 a_iProgress, const wchar_t *a_sMessage) = 0;
    };
  }
}