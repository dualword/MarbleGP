/***************************************
This file is part of LuaWrap.

Copyright © 2012 Pegasus Alpha
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

* Redistributions of source code must retain the above copyright notice,
	this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright
	notice, this list of conditions and the following disclaimer in the
	documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***************************************/

#ifndef LUA_WRAPPER_HPP
#define LUA_WRAPPER_HPP

#include <iostream>
#include <string>

extern "C" {
#ifdef _LINUX_INCLUDE_PATH
#include <lua5.3/lua.h>
#include <lua5.3/lualib.h>
#include <lua5.3/lauxlib.h>
#else
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#endif
}

/***************************************
 * Prototypes
 * (note that C++ doesn’t allow
 * templates in a separate file than
 * their prototypes)
 ***************************************/

 /**
	* This is the toplevel namespace for the LuaWrap library.
	*/
namespace LuaWrap {

	/**
	 * Version number as a string.
	 */
	inline std::string version() { return "0.0.1-dev"; }

	/**
	 * Internal functions to be called from the Lua side are defined in this
	 * namespace. This means that you are not supposed to call this methods
	 * directly as they’re bound to Lua userdata objects at runtime.
	 */
	namespace InternalLua {
		// Class methods
		int name(lua_State* p_state);
		int superclass(lua_State* p_state);

		// Instance methods
		int klass(lua_State* p_state);
		int method_lookup(lua_State* p_state);
		template<typename WrappedClass> int default_new(lua_State* p_state);
		template<typename WrappedClass> int default_gc(lua_State* p_state);
	};

	/**
	 * The methods defined inside this namespace manage the interaction between
	 * the Lua and the C++ side. For your own classes, you normally shouldn’t
	 * bother about what’s defined herein, except probably two methods,
	 * set_imethod_table(), which is a helper to be used from within your
	 * own Lua constructors, and dump_lua_stack() which can be really useful
	 * when debugging what’s wrong with your Lua stack.
	 */
	namespace InternalC {
		/// \internal Lua classname + this suffix = name of instance method table in Lua registry
		inline std::string itable_suffix() { return "__instancemethods"; }

		void dump_lua_stack(lua_State* p_state);
		void set_imethod_table(lua_State* p_state, int index);

		void create_classtable(lua_State* p_state,
			const std::string& classname,
			const luaL_Reg     cmethods[],
			lua_CFunction      fp_constructor,
			std::string        superclassname = "");

		void create_instancetable(lua_State* p_state,
			const std::string& classname,
			const luaL_Reg     imethods[],
			lua_CFunction      fp_finalizer);
	};

	template<typename WrappedClass>
	WrappedClass* check(lua_State* p_state, int index = 1);

	template<typename WrappedClass>
	void register_class(lua_State* p_state,
		std::string    classname,
		const luaL_Reg imethods[] = NULL,
		const luaL_Reg cmethods[] = NULL,
		lua_CFunction  fp_constructor = InternalLua::default_new<WrappedClass>,
		lua_CFunction  fp_finalizer = InternalLua::default_gc<WrappedClass>);

	template<typename WrappedClass>
	void register_subclass(lua_State* p_state,
		std::string    classname,
		std::string    superclassname,
		const luaL_Reg imethods[] = NULL,
		const luaL_Reg cmethods[] = NULL,
		lua_CFunction  fp_constructor = InternalLua::default_new<WrappedClass>,
		lua_CFunction  fp_finalizer = InternalLua::default_gc<WrappedClass>);

	void register_singleton(lua_State* p_state,
		std::string classname,
		const luaL_Reg methods[]);
};

/***************************************
 * Implementation
 ***************************************/

namespace LuaWrap {

	namespace InternalLua {

		/**
		 * This is the default constructor for a registered Lua class.
		 * It just instanciates the wrapped class and stores the instance
		 * inside the Lua object, then it associates the metatable for
		 * the instance methods with the object by calling out to
		 * InternalC::set_imethod_table.
		 *
		 * \tparam WrappedClass The class to create the constructor for.
		 * \param[in]  p_state  The Lua stack to operate on. Must contain
		 *                      the class method table of the class to
		 *                      instanciate on the top.
		 *
		 * \returns 1 as this places a new "instance" on the stack.
		 *
		 * Lua Example:
		 * <pre>
		 * MyClass:new() --> An instance of this class
		 * </pre>
		 *
		 * \see InternalC::set_imethod_table() for a helper method you can use
		 * to simplify the defining of instance method tables when you don’t
		 * want to use this standard constructor. See also the Readme for an
		 * example of a custom constructor.
		 *
		 */
		template<typename WrappedClass>
		int default_new(lua_State* p_state)
		{
			// Ensure we got a class table
			if (!lua_istable(p_state, 1))
				return luaL_error(p_state, "No class table given.");

			// Let Lua allocate memory and place the new object into that memory
			WrappedClass* p_wrapped = new (lua_newuserdata(p_state, sizeof(WrappedClass))) WrappedClass;
			if (!p_wrapped)
				return luaL_error(p_state, "Failed to allocate an instance!");

			InternalC::set_imethod_table(p_state, -2);

			return 1;
		}

		/**
		 * This is the default finalizer for a registered Lua class.
		 * It calls the destructor of the wrapped class’ instance, but
		 * doesn’t release any memory (releasing the memory the instance
		 * lives in is up to Lua)
		 *
		 * \attention You don’t want to call this method directly, neither
		 * from C++ nor from Lua, because this is the <tt>__gc</tt> metamethod
		 * automatically invoked by Lua just prior to freeing an object.
		 *
		 * \tparam WrappedClass The class to create the finalizer for.
		 * \param[in]  p_state  The Lua stack to operate on. Must contain
		 *                      the Lua userdata object you want to finalize.
		 *
		 * \returns 0 and keeps the stack balanced.
		 *
		 * \see the Readme for an example of defining your own finalizer.
		 */
		template<typename WrappedClass>
		int default_gc(lua_State* p_state)
		{
			WrappedClass* p_wrapped = (WrappedClass*)lua_touserdata(p_state, 1);
			p_wrapped->~WrappedClass(); // Can’t call delete as Lua manages the chunk of memory
			return 0;
		}
	};


	/**
	 * Automatically checks whether the stack contains a Lua userdata
	 * object at the bottom (or at the given index) and raises a Lua
	 * error if there isn’t one. It then unwraps the original object
	 * from the Lua userdata and checks whether the pointer returned
	 * by Lua points to a valid area of memory, and if it doesn’t,
	 * throw as well (but note this doesn’t check whether the given
	 * pointer actually points to an instance of your expected class).
	 *
	 * The name was choosen to resemble Lua’s own luaL_check* function
	 * names.
	 *
	 * \tparam WrappedClass The class you want to deal with.
	 * \param[in] p_state The Lua stack to check.
	 * \param     index   The index to look up.
	 *
	 * \returns The pointer to the wrapped object. Note this method
	 * doesn’t return in case of an error.
	 *
	 * Example:
	 * \code
	 * int mycoolfunc(lua_State* p_lua)
	 * {
	 *   MyClass* p_class = LuaWrap::check<MyClass>(p_lua);
	 *   // Do something with the pointer...
	 *   return 0;
	 * }
	 * \endcode
	 */
	template<typename WrappedClass>
	WrappedClass* check(lua_State* p_state, int index /* = 1 */)
	{
		if (!lua_isuserdata(p_state, index)) {
			luaL_error(p_state, "No userdata object given.");
			return NULL;
		}

		// lua_touserdata() returns void* which cannot be chcked with
		// dynamic_cast<>.
		WrappedClass* p_wrapped = (WrappedClass*)lua_touserdata(p_state, index);
		if (!p_wrapped) {
			luaL_error(p_state, "Got a NULL pointer from Lua.");
			return NULL;
		}

		return p_wrapped;
	}

	/**
	 * This is the main method you want to interact with when registering
	 * C++ classes with Lua.
	 *
	 * \tparam    WrappedClass   The C++ class you want to wrap.
	 * \param[in] p_state        The Lua state to define the class in.
	 * \param     classname      The name the class will have inside Lua. Is not required
	 *                           to match the real C++ class name.
	 * \param     imethods       A C array of luaL_Reg structures that define the instance
	 *                           methods you may call on instances of this class. Note that
	 *                           this array must be stored _permanently_ somewhere as this
	 *                           method doesn’t copy it, i.e. ensure that it doesn’t go out
	 *                           of scope. Also, do *not* create a <tt>__gc</tt> entry,
	 *                           this is done automatically by <tt>register_class</tt>.
	 *                           Instead, use the <tt>fp_finalizer</tt> function pointer.
	 * \param     cmethods       A C array of luaL_Reg structures that define the class
	 *                           methods (aka static member functions) of the Lua class.
	 *                           As with <tt>imethods</tt>, it mustn’t go out of scope.
	 *                           Do not create a <tt>new</tt> entry, this is done automatically.
	 *                           Use <tt>fp_constructor</tt> instead.
	 * \param[in] fp_constructor If you don’t like the default Lua constructor this method
	 *                           creates for your class, pass in a pointer to a Lua C function
	 *                           here that will be used instead. Note _you_ are responsible for
	 *                           allocating the necessary memory and pushing a Lua userdata
	 *                           (either light or full) onto the stack inside this method.
	 *                           You most likely want to have a look at how InternalLua::default_new
	 *                           handles this in order to know what to do.
	 *                           If this is NULL, LuaWrap won’t define the new() method for this
	 *                           class, making it impossible to instanciate the class from the
	 *                           Lua side. This may be useful for creating abstract classes.
	 *                           Note that for C++-created objects, the finalizer (see below)
	 *                           may still be called.
	 * \param[in] fp_finalizer   When defining a custom Lua constructor, you most likely want
	 *                           a custom finalizer too. Pass a pointer to a Lua C function
	 *                           that will be called as the <tt>__gc</tt> metamethod. Please
	 *                           note that if your custom constructor allocates memory using
	 *                           Lua’s full userdata, you cannot use the <tt>delete</tt> operator
	 *                           here as Lua later tries to free this memory itself, most
	 *                           likely causing a segmentation fault.
	 *
	 * This method ensures the given Lua stack is balanced.
	 *
	 *
	 * \see The README (Main page in Doxygen) as it has several
	 * examples using this method I don’t want to repeat here.
	 */
	template<typename WrappedClass>
	void register_class(lua_State* p_state,
		std::string    classname,
		const luaL_Reg imethods[]     /* = NULL */,
		const luaL_Reg cmethods[]     /* = NULL */,
		lua_CFunction  fp_constructor /* = InternalLua::default_new<WrappedClass> */,
		lua_CFunction  fp_finalizer   /* = InternalLua::default_gc<WrappedClass> */)
	{
		// Create the class method table and register the classname for it
		InternalC::create_classtable(p_state, classname, cmethods, fp_constructor);
		// Create the instancemethods metatable
		InternalC::create_instancetable(p_state, classname, imethods, fp_finalizer);
	}

	/**
	 * Acts the same as register_class(), but allows you to specify
	 * a superclass for your class from which all methods are
	 * inherited.
	 *
	 * \tparam    WrappedClass   The C++ class you want to wrap.
	 * \param[in] p_state        The Lua state to define the class in.
	 * \param     classname      The name the class will have inside Lua. Is not required
	 *                           to match the real C++ class name.
	 * \param     superclassname The *Lua* name of the class you want to subclass.
	 *                           Note the class is not required to exist yet, but you have
	 *                           to create it before calling any method on objects of the
	 *                           subclass, otherwise you’ll get strange errors.
	 * \param     imethods       A C array of luaL_Reg structures that define the instance
	 *                           methods you may call on instances of this class. Note that
	 *                           this array must be stored _permanently_ somewhere as this
	 *                           method doesn’t copy it, i.e. ensure that it doesn’t go out
	 *                           of scope. Also, do *not* create a <tt>__gc</tt> entry,
	 *                           this is done automatically by <tt>register_class</tt>.
	 *                           Instead, use the <tt>fp_finalizer</tt> function pointer.
	 * \param     cmethods       A C array of luaL_Reg structures that define the class
	 *                           methods (aka static member functions) of the Lua class.
	 *                           As with <tt>imethods</tt>, it mustn’t go out of scope.
	 *                           Do not create a <tt>new</tt> entry, this is done automatically.
	 *                           Use <tt>fp_constructor</tt> instead.
	 * \param[in] fp_constructor If you don’t like the default Lua constructor this method
	 *                           creates for your class, pass in a pointer to a Lua C function
	 *                           here that will be used instead. Note _you_ are responsible for
	 *                           allocating the necessary memory and pushing a Lua userdata
	 *                           (either light or full) onto the stack inside this method.
	 *                           You most likely want to have a look at how InternalLua::default_new
	 *                           handles this in order to know what to do.
	 *                           If this is NULL, LuaWrap won’t define the new() method for this
	 *                           class, making it impossible to instanciate the class from the
	 *                           Lua side. This may be useful for creating abstract classes.
	 *                           Note that for C++-created objects, the finalizer (see below)
	 *                           may still be called.
	 * \param[in] fp_finalizer   When defining a custom Lua constructor, you most likely want
	 *                           a custom finalizer too. Pass a pointer to a Lua C function
	 *                           that will be called as the <tt>__gc</tt> metamethod. Please
	 *                           note that if your custom constructor allocates memory using
	 *                           Lua’s full userdata, you cannot use the <tt>delete</tt> operator
	 *                           here as Lua later tries to free this memory itself, most
	 *                           likely causing a segmentation fault.
	 *
	 * This method keeps the stack balanced.
	 */
	template<typename WrappedClass>
	void register_subclass(lua_State* p_state,
		std::string    classname,
		std::string    superclassname,
		const luaL_Reg imethods[]     /* = NULL */,
		const luaL_Reg cmethods[]     /* = NULL */,
		lua_CFunction  fp_constructor /* = InternalLua::default_new<WrappedClass> */,
		lua_CFunction  fp_finalizer   /* = InternalLua::default_gc<WrappedClass> */)
	{
		InternalC::create_classtable(p_state, classname, cmethods, fp_constructor, superclassname);
		InternalC::create_instancetable(p_state, classname, imethods, fp_finalizer);
	}


};

#endif