/* Generated by the Dustbin::Games LuaBind Python Script (w) 2020 by Christian Keimel / Dustbin::Games */
#pragma once

#include <irrlicht/irrlicht.h>
#include <_generated/lua/lua_tables.h>
#include <luawrap.hpp>
#include <lua/ILuaClass.h>
#include <irrlicht/irrlicht.h>

/**
 * @class CCLuaSceneObject
 * @author Dustbin::Games LuaBind Python Script
 * LUA object for mesh objects in 3d scenes
 */
class CLuaSceneObject : public ILuaClass {
  protected:
    irr::scene::ISceneNode * m_node;  /**< Pointer to the Irrlicht scene node */

    void *getObjectFromLuaStack(const std::string &a_sIdentifier);

  public:
    CLuaSceneObject();
    virtual ~CLuaSceneObject();

    /**
     * Replace a texture of the scene node by a texture containing some text
     * @param The material of which the texture is replaced
     * @param The text to be used for the text texture
     * @param Width of the generated texture
     * @param Height of the generated texture
     * @param Generate the texture with transparent background
     * @param Center the text horizontally?
     * @param Color of the text to render
     * @param Background color of the texture
     */
    void settexttexture(int a_material, const std::string &a_text, int a_width, int a_height, bool a_alpha, bool a_center, SColor a_textcolor, SColor a_background);
    /**
     * Replace a texture of the scene node by an image
     * @param The material of which the texture is replaced
     * @param The image to be used for the texture
     */
    void setimagetexture(int a_material, const std::string &a_image);
    /**
     * Set the scale of the scene object
     * @param The new scale
     */
    void setscale(const SVector3d &a_scale);
    /**
     * Initialize the object with a child of the passed scene object
     * @param The parent object which has a named child that is used for initialization
     * @param Name of the child to use for initialization of the object
     */
    void initchild(CLuaSceneObject *a_object, const std::string &a_child);
    /**
     * Initialize the object by cloning the passed node
     * @param The scene object to clone
     */
    void initclone(CLuaSceneObject *a_object);
    /**
     * Find out whether or not the object is currently visible
     */
    bool isvisible();
    /**
     * Initialize the object with a node from the scene
     * @param Name of the node in the scene
     */
    void initialize(const std::string &a_nodename);
    /**
     * Set the rotation of the scene object in Euler angles
     * @param The new rotation.
     */
    void setrotation(const SVector3d &a_rotation);
    /**
     * Set the position of the scene object
     * @param The new position
     */
    void setposition(const SVector3d &a_position);
    /**
     * Set the visibility of the scene node
     * @param The new visibility flag
     */
    void setvisible(bool a_visible);
    /**
     * Retrieve the position of the scene node
     */
    SVector3d getposition();
    /**
     * Search for a child of this scene node
     * @param The parent node to search for the child in
     * @param The name of the requested child node
     */
    irr::scene::ISceneNode * findChild(irr::scene::ISceneNode * a_parent, const std::string &a_name);
    /**
     * Get the Irrlicht scene node attached to the LUA scene node instance
     */
    irr::scene::ISceneNode * getNode();

    static void registerMethods(lua_State *a_pState);
};
