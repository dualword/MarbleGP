// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#include <_generated/lua/lua_tables.h>
#include <irrlicht/irrlicht.h>

namespace dustbin {
  namespace lua {
    /**
     * Convert an Irrlicht vector to a LUA vector
     * @param a_cInput the Irrlicht vector
     * @return the LUA vector
     */
    SVector3d irrVectorToLua(const irr::core::vector3df &a_cInput);

    /**
     * Convert a LUA vector to an Irrlicht vector
     * @param a_cInput the LUA vector
     * @return the Irrlicht vector
     */
    irr::core::vector3df luaVectorToIrr(const SVector3d &a_cInput);

    /**
     * Convert Irrlicht color information to LUA
     * @param a_cInput Irrlicht color information
     * @return LUA color information
     */
    SColor irrColorToLua(const irr::video::SColor &a_cInput);

    /**
     * Convert LUA color information to Irrlicht
     * @param a_cInput Irrlicht color information
     * @return LUA color information
     */
    irr::video::SColor luaColorToIrr(const SColor a_cInput);

    /**
     * Convert a string to LUA color information
     * @param a_sInput hex color string (AARRGGBB). If the string is less than 
     * eight characters long alpha will be FF, all colors will be filled with 0
     */
    SColor colorStringToLua(const std::string &a_sColor);

    /**
     * Convert a lua color to a string (AARRGGBB)
     * @param a_cInput the lua color to encode
     * @return the color encoded as a string
     */
    std::string luaColorToString(const SColor &a_cInput);

    /**
     * Load a LUA script from the file system and return the content as string
     * @param a_sFileName filename of the LUA script
     * @return the content of the script file
     */
    std::string loadLuaScript(const std::string &a_sFileName);
  }
}
