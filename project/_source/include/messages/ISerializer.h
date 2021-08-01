/*
(w) 2016 - 2021 by Christian Keimel / https://www.dustbin-online.de
This file is licensed under the terms of the ZLib license - https://www.zlib.net/zlib_license.html
*/
#pragma once

#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

namespace dustbin {
  namespace messages {
    /**
     * @interface ISerializer
     * @author Christian Keimel
     * @brief This is the interface that needs to be implemented for all kinds of serializers
     */
    class ISerializer {
      public:
        virtual irr::u8 getU8() = 0;      /**< Read an 8 bit unsigned integer from the current message position */
        virtual irr::s8 getS8() = 0;      /**< Read a 8 bit signed integer from the current message position */
        virtual irr::u16 getU16() = 0;    /**< Read a 16 bit unsigned integer from the current message position */
        virtual irr::s16 getS16() = 0;    /**< Read a 16 bit signed integer from the current message position */
        virtual irr::u32 getU32() = 0;    /**< Read a 32 bit unsigned integer from the current message position */
        virtual irr::s32 getS32() = 0;    /**< Read a 32 bit signed integer from the current message position */
        virtual irr::u64 getU64() = 0;    /**< Read a 64 bit unsigned integer from the current message position */
        virtual irr::s64 getS64() = 0;    /**< Read a 64 bit signed integer from the current message position */
        virtual irr::f32 getF32() = 0;    /**< Read a 32 bit float from the current message position */
        virtual irr::f64 getF64() = 0;    /**< Read a 64 bit float from the current message position */

        virtual const irr::core::vector3df getVector3df() = 0;  /**< Read 3d vector of 32 bit unsigned floats from the current message position */
        virtual std::string getString() = 0;                 /**< Read string from the current message position */

        virtual void addU8(irr::u8 a_iInput) = 0;     /**< Append an 8 bit unsigned integer to the message */
        virtual void addS8(irr::s8 a_iInput) = 0;     /**< Append an 8 bit signed integer to the message */
        virtual void addU16(irr::u16 a_iInput) = 0;   /**< Append a 16 bit unsigned integer to the message */
        virtual void addS16(irr::s16 a_iInput) = 0;   /**< Append a 16 bit signed integer to the message */
        virtual void addU32(irr::u32 a_iInput) = 0;   /**< Append a 32 bit unsigned integer to the message */
        virtual void addS32(irr::s32 a_iInput) = 0;   /**< Append a 32 bit signed integer to the message */
        virtual void addU64(irr::u64 a_iInput) = 0;   /**< Append a 64 bit unsigned integer to the message */
        virtual void addS64(irr::s64 a_iInput) = 0;   /**< Append a 64 bit signed integer to the message */
        virtual void addF32(irr::f32 a_fInput) = 0;   /**< Append a 32 bit float to the message */
        virtual void addF64(irr::f64 a_fInput) = 0;   /**< Append a 64 bit float to the message */

        virtual void addVector3df(const irr::core::vector3df &a_vInput) = 0;  /**< Append a 3d vector of 32 bit floats to the message */
        virtual void addString(const std::string &a_sInput) = 0;                  /**< Append a string to the message */

        virtual void reset() = 0;               /**< Reset the message */
        virtual irr::u16 getMessageType() = 0;  /**< Get the message type, i.e. the first 16 bit unsigned integer */

        virtual const void *getMessage(irr::s32 &a_iSize) = 0;  /**< Get the message */

        virtual const void *getBuffer() = 0;    /**< Get the message buffer */
        virtual irr::u32 getBufferSize() = 0;   /**< Get the message buffer size */
    };
  }
}