
cmake_minimum_required(VERSION 3.22.1)

# Declares and names the project.

project("marblegp")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
file(GLOB_RECURSE src
        ../_source/source/*.cpp
        )
file(APPEND ../_source/source/helpers/CMedalHelpers.cpp)
file(APPEND ../_source/source/gamecloud/CGameCloud_None.cpp)
file(APPEND ../_source/source/data/CTrackPackManager.cpp)
file(APPEND ../_source/source/main_raspberry.cpp)

list(FILTER src EXCLUDE REGEX ".*main_windows.cpp$")
list(FILTER src EXCLUDE REGEX ".*main_android.cpp$")
list(FILTER src EXCLUDE REGEX ".*CPlatformAndroid.cpp$")
list(FILTER src EXCLUDE REGEX ".*CPlatformWindows.cpp$")
list(FILTER src EXCLUDE REGEX ".*CSoundData.cpp$")
list(FILTER src EXCLUDE REGEX ".*CSoundInterface.cpp$")
list(FILTER src EXCLUDE REGEX ".*CSoundInterfaceAndroid.cpp$")

add_definitions(-DNO_XEFFECT -D_IRR_STATIC_LIB_ -D_OPENGL_ES -D_RASPI)
add_compile_definitions(_GLIBCXX_USE_CXX11_ABI=0)

add_library(enet STATIC IMPORTED)
add_library(lua STATIC IMPORTED)
add_library(ode STATIC IMPORTED)
add_library(Irrlicht STATIC IMPORTED)
add_library(freetype STATIC IMPORTED)

set_target_properties(enet PROPERTIES IMPORTED_LOCATION /usr/local/lib/libenet.a)
set_target_properties(lua PROPERTIES IMPORTED_LOCATION /usr/local/lib/liblua.a)
set_target_properties(ode PROPERTIES IMPORTED_LOCATION /usr/local/lib/libode.a)
set_target_properties(Irrlicht PROPERTIES IMPORTED_LOCATION ../../../libs/Irrlicht/lib/Linux/libIrrlicht.a)
set_target_properties(freetype PROPERTIES IMPORTED_LOCATION /usr/local/lib/libfreetype.a)

add_compile_options(-Os -fwrapv -fomit-frame-pointer -ffunction-sections -fdata-sections -fno-asynchronous-unwind-tables)
add_executable(
        marblegp

        # Provides a relative path to your source file(s).
        ${src}
        )

target_include_directories(marblegp PUBLIC
        ../_source/include
        $ENV{IRRLICHT_ES_FOLDER}/include
        $ENV{IRRLICHT_ES_FOLDER}/source/Irrlicht
        $ENV{VCPKG_INCLUDE_ARM64}/include
	"../../../libs/Irrlicht/include/"
	"../../../libs/enet/include/"
	"../../../libs/ode/include/"
	"../../../libs/freetype/freetype-2.13.3/include/"
        )

target_link_libraries(
        marblegp
        EGL
        GLESv1_CM
        z
        m
        GLESv2
        Irrlicht
        ode
        enet
        lua
        brotlienc
	brotlidec
	brotlicommon
        freetype
	OpenGL
	EGL
	GLX
	X11
	Xext
	Xxf86vm
        )
