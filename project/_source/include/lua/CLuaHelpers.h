// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

#include <_generated/lua/lua_tables.h>
#include <CGlobal.h>
#include <string>

namespace dustbin {
  namespace lua {
    /**
    * Load a LUA script from the file system and return the content as string
    * @param a_sFileName filename of the LUA script
    * @return the content of the script file
    */
    std::string loadLuaScript(const std::string& a_sFileName);

    /**
    * Convert a SVector3 lua structure to an irrlicht vector
    * @param a_cIn the lua table
    * @return the Irrlicht vector
    */
    irr::core::vector3df convertVectorToIrr(const SVector3d &a_cIn);

    /**
    * Convert an Irrlicht vector to a lua SVector3
    * @param a_cIn the Irrlicht vector
    * @return the lua table
    */
    SVector3d convertVectorToLua(const irr::core::vector3df &a_cIn);

    /**
    * Convert a SColor lua structor to an irrlicht color object
    * @param a_cIn the lua table
    * @return the irrlicht color
    */
    irr::video::SColor convertColorToIrr(const SColor &a_cIn);

    /**
    * Convert an Irrlicht color to a lua SColor
    * @param a_cIn the irrlicht color
    * @return a SColor lua structure
    */
    SColor convertColorToLua(const irr::video::SColor &a_cIn);
  }
}
