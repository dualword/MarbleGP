/*
(w) 2016 - 2020 by Christian Keimel / https://www.dustbin-online.de
This file is licensed under the terms of the ZLib license - https://www.zlib.net/zlib_license.html
*/
#include <messages/CMessageHelpers.h>
#include <messages/CSerializer64.h>

namespace dustbin {
  namespace messages {

    const irr::c8* CSerializer64::EncodeInt64(irr::s64 a_iIn) {
      int l_iIdx = 0,
        l_iLen = (int)strlen(_64_DIGITS_STRING);

      memset(m_sBuffer, '\0', 0xFF);

      if (a_iIn < 0) {
        a_iIn = -a_iIn;
        m_sBuffer[l_iIdx] = '-';
        l_iIdx++;
      }

      do {
        m_sBuffer[l_iIdx] = _64_DIGITS_STRING[a_iIn % l_iLen];
        a_iIn /= l_iLen;
        l_iIdx++;
      } while (a_iIn > 0);

      m_sBuffer[l_iIdx] = '\0';

      return m_sBuffer;
    }

    irr::s32 CSerializer64::strpos(const irr::c8* a_sIn, const irr::c8 a_cNeedle) {
      for (irr::s32 i = 0; a_sIn[i] != '\0'; i++) if (a_sIn[i] == a_cNeedle) return i;
      return -1;
    }

    irr::s64 CSerializer64::Decode64Int(const irr::c8* a_sIn) {
      irr::s64 l_iRet = 0,
        l_iFactor = 1,
        l_iSign = 1;

      for (int i = 0; a_sIn[i] != '\0'; i++) {
        if (i == 0 && a_sIn[i] == '-') {
          l_iSign = -1;
        }
        else {
          irr::s32 l_Mult = strpos(_64_DIGITS_STRING, a_sIn[i]);
          if (l_Mult == -1) {
            printf("\nOoops (%i / %c) / %s\n\n", (int)a_sIn[i], a_sIn[i], a_sIn);
            return 0;
          }
          l_iRet += l_iFactor * l_Mult;
          l_iFactor *= m_iBase;
        }
      }

      return l_iSign * l_iRet;
    }

    /**
    * This method makes sure that all values are separated by a semicolon
    */
    void CSerializer64::prepare() {
      if (m_sMessage.size() > 0)
        m_sMessage += ';';
    }

    CSerializer64::CSerializer64() {
      m_iBase = (irr::u32)strlen(_64_DIGITS_STRING);
    }

    CSerializer64::~CSerializer64() { }

    /**
    * Constructor
    * @param a_sData the string received from a socket or read from a file
    */
    CSerializer64::CSerializer64(const irr::core::stringc a_sData) {
      irr::u32 l_iPos = 0, l_iLast = 0;
      m_iPos = 0;

      for (l_iPos = 0; l_iPos < a_sData.size(); l_iPos++) {
        if (a_sData.c_str()[l_iPos] == ';') {
          m_aData.push_back(a_sData.subString(l_iLast, l_iPos - l_iLast));
          l_iLast = l_iPos + 1;
        }
      }
      m_aData.push_back(a_sData.subString(l_iLast, l_iPos - l_iLast));
      m_iBase = (irr::u32)strlen(_64_DIGITS_STRING);
    }

    /**
    * add an unsigned 8 bit value
    * @param a_iInput an unsigned 8 bit value
    */
    void CSerializer64::addU8(irr::u8 a_iInput) {
      prepare();
      m_sMessage += irr::core::stringc(EncodeInt64((irr::s64)a_iInput));
    }

    /**
    * add a signed 8 bit value
    * @param a_iInput a signed 8 bit value
    */
    void CSerializer64::addS8(irr::s8 a_iInput) {
      prepare();
      m_sMessage += irr::core::stringc(EncodeInt64((irr::s64)a_iInput));
    }

    /**
    * add an unsigned 16 bit value
    * @param a_iInput an unsigned 16 bit value
    */
    void CSerializer64::addU16(irr::u16 a_iInput) {
      prepare();
      m_sMessage += irr::core::stringc(EncodeInt64((irr::s64)a_iInput));
    }

    /**
    * add a signed 16 bit value
    * @param a_iInput a signed 16 bit value
    */
    void CSerializer64::addS16(irr::s16 a_iInput) {
      prepare();
      m_sMessage += irr::core::stringc(EncodeInt64((irr::s64)a_iInput));
    }

    /**
    * add an unsigned 32 bit value
    * @param a_iInput an unsigned 32 bit value
    */
    void CSerializer64::addU32(irr::u32 a_iInput) {
      prepare();
      m_sMessage += irr::core::stringc(EncodeInt64((irr::s64)a_iInput));
    }

    /**
    * add a signed 32 bit value
    * @param a_iInput a signed 32 bit value
    */
    void CSerializer64::addS32(irr::s32 a_iInput) {
      prepare();
      m_sMessage += irr::core::stringc(EncodeInt64((irr::s64)a_iInput));
    }

    void CSerializer64::addS64(irr::s64 a_iInput) {
      prepare();
      m_sMessage += irr::core::stringc(EncodeInt64(a_iInput));
    }

    void CSerializer64::addU64(irr::u64 a_iInput) {
      prepare();
      m_sMessage += irr::core::stringc(EncodeInt64(a_iInput));
    }

    /**
    * add a float value
    * @param a_fInput a float value
    * @param a_iPrecision decimal precision of the value
    */
    void CSerializer64::addF32(irr::f32 a_fInput) {
      int l_iPrecision = 5;
      while (l_iPrecision > 0) { a_fInput *= 10; l_iPrecision--; }
      addS64((irr::s64)a_fInput);
    }

    void CSerializer64::addF64(irr::f64 a_fInput) {
      int l_iPrecision = 5;
      while (l_iPrecision > 0) { a_fInput *= 10; l_iPrecision--; }
      addS64((irr::s64)a_fInput);
    }

    /**
    * Add a 3d vector
    * @param a_vInput the vectro to add
    */
    void CSerializer64::addVector3df(const irr::core::vector3df& a_vInput) {
      addF32(a_vInput.X);
      addF32(a_vInput.Y);
      addF32(a_vInput.Z);
    }

    /**
    * add a string
    * @param a_sInput a string
    */
    void CSerializer64::addString(const std::string& a_sInput) {
      prepare();
      m_sMessage += urlEncode(a_sInput).c_str();
    }

    /**
    * Get the coded message string
    * @return the coded message string
    */
    const void* CSerializer64::getMessage(irr::s32& a_iSize) {
      a_iSize = sizeof(irr::c8) * m_sMessage.size();
      return (const void*)m_sMessage.c_str();
    }

    const void* CSerializer64::getBuffer() {
      return (const void*)m_sMessage.c_str();
    }

    irr::u32 CSerializer64::getBufferSize() {
      return sizeof(irr::c8) * m_sMessage.size();
    }

    /**
    * Reset the message
    */
    void CSerializer64::reset() {
      m_sMessage = "";
      m_iPos = 0;
    }

    irr::u16 CSerializer64::getMessageType() {
      m_iPos = 0;
      return getU16();
    }

    /**
    * Get an unsigned 8 Bit value at the current reading postition and increment the reading position by one
    * @return an unsigned 8 Bit value
    */
    irr::u8 CSerializer64::getU8() {
      irr::u8 l_iRet = m_iPos < m_aData.size() ? (irr::u8)Decode64Int(m_aData[m_iPos].c_str()) : 0;
      m_iPos++;
      return l_iRet;
    }

    /**
    * Get an signed 8 Bit value at the current reading postition and increment the reading position by one
    * @return an unsigned 8 Bit value
    */
    irr::s8 CSerializer64::getS8() {
      irr::u8 l_iRet = m_iPos < m_aData.size() ? (irr::s8)Decode64Int(m_aData[m_iPos].c_str()) : 0;
      m_iPos++;
      return l_iRet;
    }

    /**
    * Get an unsigned 16 Bit value at the current reading postition and increment the reading position by one
    * @return an unsigned 16 Bit value
    */
    irr::u16 CSerializer64::getU16() {
      irr::u16 l_iRet = m_iPos < m_aData.size() ? (irr::u16)Decode64Int(m_aData[m_iPos].c_str()) : 0;
      m_iPos++;
      return l_iRet;
    }

    /**
    * Get a signed 16 Bit value at the current reading postition and increment the reading position by one
    * @return an unsigned 16 Bit value
    */
    irr::s16 CSerializer64::getS16() {
      irr::s16 l_iRet = m_iPos < m_aData.size() ? (irr::s16)Decode64Int(m_aData[m_iPos].c_str()) : 0;
      m_iPos++;
      return l_iRet;
    }

    /**
    * Get an unsigned 32 Bit value at the current reading postition and increment the reading position by one
    * @return an unsigned 32 Bit value
    */
    irr::u32 CSerializer64::getU32() {
      irr::u32 l_iRet = m_iPos < m_aData.size() ? (irr::u32)Decode64Int(m_aData[m_iPos].c_str()) : 0;
      m_iPos++;
      return l_iRet;
    }

    /**
    * Get a signed 32 Bit value at the current reading postition and increment the reading position by one
    * @return an signed 32 Bit value
    */
    irr::s32 CSerializer64::getS32() {
      irr::s32 l_iRet = m_iPos < m_aData.size() ? (irr::s32)Decode64Int(m_aData[m_iPos].c_str()) : 0;
      m_iPos++;
      return l_iRet;
    }

    irr::s64 CSerializer64::getS64() {
      irr::s64 l_iRet = m_iPos < m_aData.size() ? Decode64Int(m_aData[m_iPos].c_str()) : 0;
      m_iPos++;
      return l_iRet;
    }

    irr::u64 CSerializer64::getU64() {
      irr::u64 l_iRet = m_iPos < m_aData.size() ? Decode64Int(m_aData[m_iPos].c_str()) : 0;
      m_iPos++;
      return l_iRet;
    }

    /**
    * Get a 32 bit float value at the current reading postition and increment the reading position by one
    * @return an 32 bit float value
    */
    irr::f32 CSerializer64::getF32() {
      int l_iPrecision = 5;
      irr::f64 l_fDummy = (irr::f64)getS64();
      while (l_iPrecision > 0) { l_fDummy /= 10; l_iPrecision--; }
      return (irr::f32)l_fDummy;
    }

    irr::f64 CSerializer64::getF64() {
      int l_iPrecision = 5;
      irr::f64 l_fDummy = (irr::f64)getS64();
      while (l_iPrecision > 0) { l_fDummy /= 10; l_iPrecision--; }
      return l_fDummy;
    }

    /**
    * Get a string from the current reading postition and increment the reading position by one
    * @return a string
    */
    std::string CSerializer64::getString() {
      m_sRet = "";
      const irr::c8* l_sRet = m_iPos < m_aData.size() ? m_aData[m_iPos].c_str() : m_sRet.c_str();
      m_iPos++;
      return urlDecode(l_sRet);
    }

    const irr::core::vector3df CSerializer64::getVector3df() {
      irr::core::vector3df l_vRet;

      l_vRet.X = getF32();
      l_vRet.Y = getF32();
      l_vRet.Z = getF32();

      return l_vRet;
    }

  }  // namespace dustbin
}    // namespace messages
