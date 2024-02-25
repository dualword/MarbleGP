// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel

#include <gui/IProgressCallback.h>

namespace dustbin {
  namespace gui {
    IProgressCallback::IProgressCallback() : m_sMessage(L""), m_iCurrentMin(0), m_iCurrentMax(100), m_iRange(100), m_iCurrent(0), m_iCount(100), m_iValue(-1) {
    }

    IProgressCallback::~IProgressCallback() {
    }

    /**
    * Set the current progress range
    * @param a_sMessage the message to be shown
    * @param a_iMin the min value of the current range
    * @param a_iMax the max value of the current range
    * @param a_iCount the number of items for the current range
    */
    void IProgressCallback::progressSetCurrentRange(const wchar_t* a_sMessage, irr::u32 a_iMin, irr::u32 a_iMax, irr::u32 a_iCount) {
      m_sMessage    = a_sMessage;
      m_iCurrentMin = std::min((irr::u32)100, (irr::u32)std::max((irr::u32)0, a_iMin));
      m_iCurrentMax = std::min((irr::u32)100, (irr::u32)std::max((irr::u32)0, a_iMax));;
      m_iRange      = m_iCurrentMax - m_iCurrentMin;
      m_iCount      = a_iCount;
      m_iCurrent    = 0;
    }

    /**
    * Get the min of the current range
    * @return the min of the current range
    */
    irr::u32 IProgressCallback::progressRangeMin() {
      return m_iCurrentMin;
    }

    /**
    * Get the max of the current range
    * @return the max of the current range
    */
    irr::u32 IProgressCallback::progressRangeMax() {
      return m_iCurrentMax;
    }

    /**
    * Get the current message
    * @return the current message
    */
    const wchar_t *IProgressCallback::progressGetMessage() {
      return m_sMessage.c_str();
    }

    /**
    * Update the current value
    */
    void IProgressCallback::progressInc() {
      if (m_iCurrent < m_iCount) {
        m_iCurrent++;
        irr::u32 l_iProgress = m_iCurrentMin + (m_iRange * m_iCurrent / m_iCount);

        if ((irr::s32)l_iProgress != m_iValue) {
          onProgress(l_iProgress, m_sMessage.c_str());
          m_iValue = l_iProgress;
        }
      }
    }
  }
}