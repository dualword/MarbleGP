
# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.22.1)

# Declares and names the project.

project("marblegp")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
file(GLOB_RECURSE src
        ../../../../../_source/source/*.cpp
    )

list(REMOVE_ITEM src ../../../../../_source/source/main_windows.cpp)
list(REMOVE_ITEM src ../../../../../_source/source/platform/CPlatformLinux.cpp)
list(REMOVE_ITEM src ../../../../../_source/source/platform/CPlatformWindows.cpp)
list(REMOVE_ITEM src ../../../../../_source/source/sound/CSoundData.cpp)
list(REMOVE_ITEM src ../../../../../_source/source/sound/CSoundInterface.cpp)
list(REMOVE_ITEM src ../../../../../_source/source/menu/CMenuStandings.cpp)

message(NOTICE "VCPKG_HOME=$ENV{VCPKG_ROOT}")
message(NOTICE "ANDROID_NDK=${ANDROID_NDK}")
message(NOTICE "IRRLICHT_ES_FOLDER=$ENV{IRRLICHT_ES_FOLDER}")

add_definitions(-D_ANDROID -DNO_XEFFECT -D_IRR_STATIC_LIB_ -ferror-limit=0)


add_library(native_app_glue STATIC
        ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
# add_library(native_app_glue IMPORTED)
target_include_directories(native_app_glue PUBLIC
        ${ANDROID_NDK}/sources/android/native_app_glue)


# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library(enet STATIC IMPORTED)
add_library(lua STATIC IMPORTED)
add_library(ode STATIC IMPORTED)
add_library(Irrlicht STATIC IMPORTED)

if (${ANDROID_ABI} STREQUAL "arm64-v8a")
  set_target_properties(enet PROPERTIES IMPORTED_LOCATION $ENV{VCPKG_ROOT}/installed/arm64-android/lib/libenet.a)
  set_target_properties(lua PROPERTIES IMPORTED_LOCATION $ENV{VCPKG_ROOT}/installed/arm64-android/lib/liblua.a)
  set_target_properties(ode PROPERTIES IMPORTED_LOCATION $ENV{VCPKG_ROOT}/installed/arm64-android/lib/libode.a)
  set_target_properties(Irrlicht PROPERTIES IMPORTED_LOCATION $ENV{VCPKG_ROOT}/installed/arm64-android/lib/libIrrlicht.a)
elseif(${ANDROID_ABI} STREQUAL "armeabi-v7a")
  set_target_properties(enet PROPERTIES IMPORTED_LOCATION $ENV{VCPKG_ROOT}/installed/arm-android/lib/libenet.a)
  set_target_properties(lua PROPERTIES IMPORTED_LOCATION $ENV{VCPKG_ROOT}/installed/arm-android/lib/liblua.a)
  set_target_properties(ode PROPERTIES IMPORTED_LOCATION $ENV{VCPKG_ROOT}/installed/arm-android/lib/libode.a)
  set_target_properties(Irrlicht PROPERTIES IMPORTED_LOCATION $ENV{VCPKG_ROOT}/installed/arm-android/lib/libIrrlicht.a)
elseif(${ANDROID_ABI} STREQUAL "x86")
  set_target_properties(enet PROPERTIES IMPORTED_LOCATION $ENV{VCPKG_ROOT}/installed/arm-android/lib/libenet.a)
  set_target_properties(lua PROPERTIES IMPORTED_LOCATION $ENV{VCPKG_ROOT}/installed/arm-android/lib/liblua.a)
  set_target_properties(ode PROPERTIES IMPORTED_LOCATION $ENV{VCPKG_ROOT}/installed/arm-android/lib/libode.a)
  set_target_properties(Irrlicht PROPERTIES IMPORTED_LOCATION $ENV{VCPKG_ROOT}/installed/x86-android/lib/libIrrlicht.a)
else()
  set_target_properties(enet PROPERTIES IMPORTED_LOCATION $ENV{VCPKG_ROOT}/installed/${ANDROID_ABI}/lib/libenet.a)
  set_target_properties(lua PROPERTIES IMPORTED_LOCATION $ENV{VCPKG_ROOT}/installed/${ANDROID_ABI}/lib/liblua.a)
  set_target_properties(ode PROPERTIES IMPORTED_LOCATION $ENV{VCPKG_ROOT}/installed/${ANDROID_ABI}/lib/libode.a)
  set_target_properties(irrlicht PROPERTIES IMPORTED_LOCATION $ENV{IRRLICHT_ES_FOLDER}/Source/Irrlicht/Android/obj/local/{$ANDROID_ABI}/libirrlicht.a)
endif()

add_library( # Sets the name of the library.
             marblegp

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             ${src}
        )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

find_package (oboe REQUIRED CONFIG)

target_include_directories(marblegp PUBLIC
            ../../../../../_source/include
        $ENV{IRRLICHT_ES_FOLDER}/include
        $ENV{IRRLICHT_ES_FOLDER}/source/Irrlicht
        )

if (${ANDROID_ABI} STREQUAL "arm64-v8a")
  target_include_directories(marblegp PUBLIC $ENV{VCPKG_ROOT}/installed/arm64-android/include)
elseif (${ANDROID_ABI} STREQUAL "armeabi-v7a")
  target_include_directories(marblegp PUBLIC $ENV{VCPKG_ROOT}/installed/arm-android/include)
elseif (${ANDROID_ABI} STREQUAL "x86")
  target_include_directories(marblegp PUBLIC $ENV{VCPKG_ROOT}/installed/x86-android/include)
else()
  target_include_directories(marblegp PUBLIC $ENV{VCPKG_ROOT}/installed/${ANDROID_ABI}/include)
endif()

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       marblegp
                       android
                       native_app_glue
                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib}
                       oboe::oboe
                       EGL
                       GLESv1_CM
                       z
                       m
                       GLESv2
                       Irrlicht
                       # $ENV{IRRLICHT_ES_FOLDER}/Source/Irrlicht/Android/obj/local/arm64-v8a/libirrlicht.a
                       ode
                       enet
                       lua
                       mediandk
        )
