// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>
#include <stdlib.h>

#include <sstream>
#include <stdexcept>
#include <iomanip>
#include <string>

#include <messages/ISerializer.h>

namespace dustbin {
  namespace messages {

    /**
    * @class CSerializer64
    * @brief This class is used to serialize and deserialize messages
    * @author Christian Keimel / https://www.dustbin-online.de
    * This class serializes all data in a way to send and store messages. It encodes and decodes numerical values with a base of 64, strings are URL encoded
    */
    class CSerializer64 : public ISerializer {
      protected:
        irr::core::stringc m_sMessage;                      /**< the message buffer */
        irr::core::array<irr::core::stringc> m_aData;       /**< the data fields */
        irr::u32 m_iPos;                                    /**< the current read position */
        irr::c8  m_sBuffer[0xFF];                           /**< buffer for number conversion */
        irr::u32 m_iBase;                                   /**< base for the number to string conversion */
        irr::core::vector3df m_vVector;
        irr::core::stringc m_sRet;

        const irr::c8* EncodeInt64(irr::s64 a_iIn);

        irr::s32 strpos(const irr::c8* a_sIn, const irr::c8 a_cNeedle);

        irr::s64 Decode64Int(const irr::c8* a_sIn);

        /**
        * This method makes sure that all values are separated by a semicolon
        */
        void prepare();

      public:
        CSerializer64(const irr::core::stringc a_sData);
        CSerializer64();

        virtual ~CSerializer64();

        virtual irr::u8 getU8();
        virtual irr::s8 getS8();
        virtual irr::u16 getU16();
        virtual irr::s16 getS16();
        virtual irr::u32 getU32();
        virtual irr::s32 getS32();
        virtual irr::u64 getU64();
        virtual irr::s64 getS64();
        virtual irr::f32 getF32();
        virtual irr::f64 getF64();
        virtual const irr::core::vector3df getVector3df();
        virtual std::string getString();

        virtual void addU8(irr::u8 a_iInput);
        virtual void addS8(irr::s8 a_iInput);
        virtual void addU16(irr::u16 a_iInput);
        virtual void addS16(irr::s16 a_iInput);
        virtual void addU32(irr::u32 a_iInput);
        virtual void addS32(irr::s32 a_iInput);
        virtual void addU64(irr::u64 a_iInput);
        virtual void addS64(irr::s64 a_iInput);
        virtual void addF32(irr::f32 a_fInput);
        virtual void addF64(irr::f64 a_fInput);
        virtual void addVector3df(const irr::core::vector3df& a_vInput);
        virtual void addString(const std::string& a_sInput);

        virtual void reset();
        virtual irr::u16 getMessageType();

        virtual const void* getMessage(irr::s32& a_iSize);

        virtual const void* getBuffer();
        virtual irr::u32 getBufferSize();

        std::string getMessageAsString();

        virtual bool hasMoreMessages();
    };

  } // namespace dustbin
}   // namespace messages
