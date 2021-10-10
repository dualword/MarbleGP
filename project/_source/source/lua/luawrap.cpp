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
#include "luawrap.hpp"

namespace LuaWrap {

	/**
	 * Instance method available for any object created via this library.
	 * It returns the the table for the class this object is an instance
	 * of.
	 *
	 * \param[in] p_state The Lua stack to operate on. Must contain the
	 *                    class’ name as an upvalue, so don’t call
	 *                    this from the C++ side.
	 *
	 * \returns 1, this method places the class table on the stack.
	 *
	 * Lua example:
	 * <pre>
	 * MyClass:new():class():new()
	 * </pre>
	 */
	int InternalLua::klass(lua_State* p_state)
	{
		lua_getglobal(p_state, lua_tostring(p_state, lua_upvalueindex(1)));
		return 1;
	}

	/**
	 * Class method for Lua "classes" created with this library.
	 *
	 * \param[in] p_state The Lua stack to operate on. Must contain the class table
	 *                    and the upvalue needed for this function, so you’re better
	 *                    off not calling it from the C++ side.
	 *
	 * \returns 1, this method places a Lua string on the stack that contains
	 * the name of this class.
	 *
	 * Lua Example:
	 * <pre>
	 *   MyClass:name() --> "MyClass"
	 * </pre>
	 */
	int InternalLua::name(lua_State* p_state)
	{
		// Retrieve the class name from the closure upvalues.
		lua_pushvalue(p_state, lua_upvalueindex(1));
		return 1;
	}

	/**
	 * Class method for Lua "classes" created with this library.
	 *
	 * \param[in] p_state The Lua stack to operate on. Must contain the class table
	 *                    and the upvalue needed for this fucntions, so you’re better
	 *                    off not calling it from the C++ side.
	 *
	 * \returns 1 and places the parent class’ classtable on the stack, or,
	 * if there isn’t a superclass, pushes nil.
	 *
	 * Lua Example:
	 * \code
	 * Foo:superclass():name() --> "MySuperclass"
	 * \endcode
	 */
	int InternalLua::superclass(lua_State* p_state)
	{
		std::string sclassname = std::string(lua_tostring(p_state, lua_upvalueindex(1)));
		if (sclassname.empty())
			lua_pushnil(p_state);
		else
			lua_getglobal(p_state, sclassname.c_str());
		return 1;
	}

	/**
	 * \internal
	 *
	 * The "__index" metamethod for all instances of classes created
	 * with this library. It looks up the given method name inside
	 * this class’ instance method table, and if it is found, returns
	 * the function object. If not, it loops through all superclasses
	 * until either the function is found (again, returning the
	 * function object) or no superclass is found, in which case
	 * it raises a Lua error.
	 *
	 * This function expects the original classname as an upvalue,
	 * so don’t call this from C++. And don’t call it from Lua either,
	 * this is done automatically.
	 *
	 * \param[in] p_state The Lua stack to operate on. Expects the
	 *                    userdata object and the method name on the
	 *                    stack (methodname at the top, userdata below).
	 */
	int InternalLua::method_lookup(lua_State* p_state)
	{
		if (!lua_isuserdata(p_state, 1))
			return luaL_error(p_state, "No receiver (userdata) given.");

		// Get the "method name", i.e. the key used by the caller.
		// Only strings are allowed, otherwise this isn’t a "method call".
		std::string methodname = luaL_checkstring(p_state, 2);
		std::string classname = std::string(lua_tostring(p_state, lua_upvalueindex(1)));

		// Get the instance method table for this class and look
		// the given method name up.
		luaL_getmetatable(p_state, (classname + InternalC::itable_suffix()).c_str());
		lua_pushstring(p_state, methodname.c_str());
		lua_rawget(p_state, -2); // Pushes nil if not found

		/* If the method name is not found, loop through the
		 * superclasses’ instance tables until we find the
		 * function or error out when no superclasses exist
		 * anymore. This if{} leaves the stack balanced except
		 * that it pushes the found function onto the top if
		 * any (otherwise error out as described). */
		if (!lua_isfunction(p_state, -1)) {
			// Remove the nil function value, it’s useless
			lua_pop(p_state, 1);
			// Original class table onto the top--this will not be
			// on the stack anymore after the while{} has finished
			// TODO: When we can define classes outside the global
			// namespace, obejcts need to have a class() method!
			lua_getglobal(p_state, classname.c_str());
			while (true) {
				// Get the superclass() method
				lua_pushstring(p_state, "superclass");
				lua_gettable(p_state, -2);

				// Call the superclass() method (returns nil if there is no superclass)
				lua_call(p_state, 0, 1);

				// If there is no superclass, error out as nobody defined
				// this method.
				if (lua_isnil(p_state, -1))
					return luaL_error(p_state, "Undefined method `%s' for an instance of %s.", methodname.c_str(), classname.c_str());

				// Get the superclass’ name
				lua_pushstring(p_state, "name");
				lua_gettable(p_state, -2);
				lua_call(p_state, 0, 1);

				std::string superclassname = std::string(lua_tostring(p_state, -1));
				lua_pop(p_state, 1); // Remove the superclass’ name, we don’t need it anymore

				// Same lookup as before, but now for the superclass instance method table
				luaL_getmetatable(p_state, (superclassname + InternalC::itable_suffix()).c_str());
				lua_pushstring(p_state, methodname.c_str());
				lua_rawget(p_state, -2); // Pushes nil if not found

				// Put the instance method table onto the top and remove
				// it as we don’t need it anymore
				lua_insert(p_state, -2);
				lua_pop(p_state, 1);

				// If we found a function, leave it on the top and break,
				// otherwise rety with next superclass
				if (lua_isfunction(p_state, -1)) {
					// Remove the current and the previous superclass
					// tables, leave the function on the top, break
					lua_insert(p_state, -3);
					lua_pop(p_state, 2);
					break;
				}
				else {
					// Remove the nil function value, it’s useless
					lua_pop(p_state, 1);
					// Remove the class table from the previous run,
					// we don’t need it anymore (for the first run,
					// this removes the original class table)
					lua_insert(p_state, -2);
					lua_pop(p_state, 1);
					// Note that now the current superclass table is on
					// the top of the stack.
				}
			}
		}

		// Move the original instance method table onto the top of
		// the stack and delete it in order to leave the stack balanced.
		lua_insert(p_state, -2);
		lua_pop(p_state, 1);

		return 1;
	}

	/**
	 * Prints out the Lua stack on standard output. This is done in
	 * reverse order to simplify imagening the processes, i.e.
	 * you’re going to find the element you’d get when popping from
	 * the stack right at the top. It also prints out the absolute
	 * indices of the elements next to the content.
	 *
	 * \param[in] p_state The Lua stack to print out.
	 *
	 * The following function prints out the complete stack it receives
	 * on the standard output. To call it from Lua, you of course
	 * have to bind it via an instance or class method table:
	 *
	 * \code
	 * int mycoolfunction(lua_State* p_state)
	 * {
	 *   dump_lua_stack(p_state);
	 *   return 0;
	 * }
	 * \endcode
	 *
	 * The output will look something like this:
	 *
	 * \code
	 * === LUA STACK DUMP ===
	 * Total elements in the Lua stack: 3
	 *
	 * [3] Other: function
	 * ----------------------
	 * [2] String: 'name'
	 * ----------------------
	 * [1] Other: table
	 * ----------------------
	 * ==== END OF STACK ====
	 * \endcode
	 */
	void InternalC::dump_lua_stack(lua_State* p_state)
	{
		using namespace std;

		int total = lua_gettop(p_state);
		cout << "=== LUA STACK DUMP ===" << endl;
		cout << "Total elements in the Lua stack: " << total << endl << endl;

		for (int i = total; i > 0; i--) {
			cout << "[" << i << "] ";
			int t = lua_type(p_state, i);
			switch (t) {
			case LUA_TSTRING:
				cout << "String: '" << lua_tostring(p_state, i) << "'" << endl;
				break;
			case LUA_TBOOLEAN:
				cout << "Boolean: " << (lua_toboolean(p_state, i) ? "true" : "false") << endl;
				break;
			case LUA_TNUMBER:
				cout << "Numeric: " << lua_tonumber(p_state, i) << endl;
				break;
			default: // Rest
				cout << "Other: " << lua_typename(p_state, t) << endl;
				break;
			}
			cout << "----------------------" << endl; // Separator
		}
		cout << "==== END OF STACK ====" << endl;
	}

	/**
	 * Helper method to define the instance method table for a given
	 * object.
	 *
	 * \attention
	 * This method should only be called from a Lua constructor. See
	 * the source of InternalLua::default_new() for an example.
	 *
	 * \param[in] p_state
	 *   The Lua stack to operate on, whose top element is the userdata
	 *   to which you want to attach the instance methods.
	 * \param[in] index
	 *   The valid index for the class table your userdata is intended
	 *   to be an instance of.
	 *
	 * This method keeps the given stack balanced.
	 *
	 * The following example defines a custom allocator and registers it for
	 * the <tt>MyClass</tt> class.
	 * \code
	 * int mycustomallocator(lua_State* p_state)
	 * {
	 *   MyClass* p_class = new lua_newuserdata(p_state, sizeof(MyClass)) MyClass("someparametervalue");
	 *   // Tell LuaWrap to define the proper instance table on the object.
	 *   // The constructor gets passed the class table as the first Lua argument,
	 *   // and as we already pushed something onto the stack (the userdata), it’s
	 *   // now at index -2.
	 *   LuaWrap::InternalC::set_imethod_table(p_state, -2);
	 *   return 1;
	 * }
	 *
	 * // ...
	 *
	 * void luaopen_mylibrary()
	 * {
	 *   // ... (defining gp_lua as the Lua interpeter instance and the method tables)
	 *   LuaWrap::register_class<MyClass>(gp_lua, "MyClass", instance_method_table, class_method_table, mycustomallocator);
	 * }
	 * \endcode
	 */
	void InternalC::set_imethod_table(lua_State* p_state, int index)
	{
		// Ensure we got a (class) table
		if (!lua_istable(p_state, index)) {
			luaL_error(p_state, "Stack element %d is not a table.", index);
			return;
		}
		// Ensure we got a raw userdata
		if (!lua_isuserdata(p_state, -1)) {
			luaL_error(p_state, "No userdata at the top of the stack.");
			return;
		}

		// Find out the class name
		lua_pushstring(p_state, "name");
		if (index < 0)
			lua_gettable(p_state, index - 1); // -1, b/c we pushed something onto the stack, pushing the table downwards
		else
			lua_gettable(p_state, index); // Absolute indices don’t change when pushing onto the stack
		lua_call(p_state, 0, 1);
		std::string classname(lua_tostring(p_state, -1));

		lua_pop(p_state, 1); // Remove the class name from the stack, we don’t need it anymore

		luaL_getmetatable(p_state, (classname + itable_suffix()).c_str());
		lua_setmetatable(p_state, -2);
	}

	/**
	 * \internal
	 *
	 * Creates the class method table for the given class,
	 * registers the class and adds the new(), name() and
	 * superclass() methods to the class object.
	 *
	 * \param[in] p_state        The Lua stack to operate on. Should be empty.
	 * \param[in] classname      The Lua class name for the class to register.
	 *                           See LuaWrap::register_class.
	 * \param[in] cmethods       The class methods to register for this class.
	 *                           See LuaWrap::register_class.
	 * \param[in] fp_constructor The pointer to the Lua constructor to
	 *                           register for this object. See LuaWrap::register_class.
	 *                           If this is NULL, the class cannot be instanciated,
	 *                           making it abstract.
	 * \param     superclassname Name of the superclass, needed for the superclass()
	 *                           Lua method.
	 *
	 * The stack is kept balanced by this method.
	 */
	void InternalC::create_classtable(lua_State* p_state,
		const std::string& classname,
		const luaL_Reg     cmethods[],
		lua_CFunction      fp_constructor,
		std::string        superclassname)
	{
		// Create a table for the class methods and if we got some
		// method definitions, store them in the class method table
		lua_newtable(p_state);
		if (cmethods)
			luaL_setfuncs(p_state, cmethods, 0);

		// Add an entry for the new() method into the class method table
		if (fp_constructor) {
			lua_pushstring(p_state, "new");
			lua_pushcfunction(p_state, fp_constructor);
			lua_settable(p_state, -3);
		}

		// Add an entry for the name() method
		lua_pushstring(p_state, "name");
		lua_pushstring(p_state, classname.c_str());
		lua_pushcclosure(p_state, InternalLua::name, 1); // Removes the class’ name from the stack
		lua_settable(p_state, -3);

		// Add an entry for the superclass() method
		lua_pushstring(p_state, "superclass");
		lua_pushstring(p_state, superclassname.c_str()); // May push an empty string
		lua_pushcclosure(p_state, InternalLua::superclass, 1); // Removes the superclass’ name from the stack
		lua_settable(p_state, -3);

		// Name the class method table after the class, effectively
		// registering the class.
		lua_setglobal(p_state, classname.c_str());
	}

	/**
	 * \internal
	 *
	 * Creates the metatable for the instances of the given class
	 * and adds the __index() and __gc() metamethods to it.
	 * __index() returns a copy of the metatable itself, effectively
	 * making all its contents methods of the object.
	 *
	 * \param[in] p_state      The Lua stack to operate on. Should be empty.
	 * \param[in] classname    The Lua class name for the class to register,
	 *                         used to store the instance method metatable for
	 *                         this class in Lua’s registry.
	 *                         See LuaWrap::register_class.
	 * \param[in] cmethods     The class methods to register for this class.
	 *                         See LuaWrap::register_class.
	 * \param[in] fp_finalizer The pointer to the Lua finalizer (<tt>__gc</tt>) to
	 *                         register for this object. See LuaWrap::register_class.
	 *
	 * This method keeps the stack balanced.
	 */
	void InternalC::create_instancetable(lua_State* p_state,
		const std::string& classname,
		const luaL_Reg      imethods[],
		lua_CFunction       fp_finalizer)
	{
		// Register a metatable that will be used for the instance methods
		luaL_newmetatable(p_state, (classname + itable_suffix()).c_str());

		// If we got some instance methods, register them into
		// the metatable created above (the table is associated
		// with an object later in the new() class method).
		if (imethods)
			luaL_setfuncs(p_state, imethods, 0);

		// Add a class() method for every instance of any class.
		lua_pushstring(p_state, "class");
		lua_pushstring(p_state, classname.c_str());
		lua_pushcclosure(p_state, InternalLua::klass, 1);
		lua_settable(p_state, -3);

		// Give a handler function for non-existing keys. This
		// will return functions for "method calls".
		lua_pushstring(p_state, "__index");
		lua_pushstring(p_state, classname.c_str()); // Make classname directly acessible
		lua_pushcclosure(p_state, InternalLua::method_lookup, 1);
		//lua_pushvalue(p_state, -2); // Copy metatable to the top of the stack
		lua_settable(p_state, -3);

		// Create the metamethod called when an object gets gc’ed
		// (the user can explictely disable this by passing NULL).
		if (fp_finalizer) {
			lua_pushstring(p_state, "__gc");
			lua_pushcfunction(p_state, fp_finalizer);
			lua_settable(p_state, -3);
		}

		// The metatable is already stored in the registry, we don’t
		// need it anymore. Empty the stack.
		lua_pop(p_state, 1);
	}

	/**
	 * Registers a singleton, i.e. a class that has only one instance.
	 * The class exposed to Lua will not have a new() method, you can
	 * directly call the methods on the class object.
	 *
	 * As the underlying C++ object is completely managed by you, this
	 * method doesn’t require any allocators or finalizers, nor does it
	 * expose userdata objects to Lua. The registered singleton is a
	 * plain old Lua table from the Lua point of view.
	 *
	 * \param[in] p_state   The Lua state to register the singleton in.
	 * \param     classname The name to register the singleton with in Lua.
	 * \param[in] methods   The list of methods to define on the singleton.
	 *
	 * Example:
	 * \code
	 * class Foo
	 * {
	 * public:
	 *   void bar(){std::cout << "This is bar" << std::endl;}
	 * };
	 *
	 * Foo* gp_the_foo; //A global pointer to the only instance
	 * //...setting gp_the_foo somewhere...
	 *
	 * static int lua_bar(lua_State* p_state)
	 * {
	 *   gp_the_foo->bar();
	 *   return 0;
	 * }
	 *
	 * static luaL_Reg foo_methods[] = {
	 *   {"bar", lua_bar},
	 *   {NULL, NULL}
	 * };
	 *
	 * void luaopen_foo(lua_State* p_state)
	 * {
	 *   LuaWrap::register_singleton(p_state, "Foo", foo_methods);
	 * }
	 * \endcode
	 *
	 * From Lua, you’d call it like this:
	 *
	 * \code
	 * Foo:bar()
	 * \endcode
	 *
	 * It also also possible to use the normal dot notation, i.e. <tt>Foo.bar()</tt>,
	 * but note that this is discouraged as it doesn’t pass the receiver to the
	 * underlying Lua C function which may cause errors with other functions.
	 */
	void register_singleton(lua_State* p_state,
		std::string classname,
		const luaL_Reg methods[])
	{
		lua_newtable(p_state);
		luaL_setfuncs(p_state, methods, 0);
		lua_setglobal(p_state, classname.c_str());
	}

};