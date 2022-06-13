cmake_minimum_required(VERSION 3.20)
project(Fillets)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "-Wall -Werror -g")
set(CMAKE_C_FLAGS -D_FORTIFY_SOURCE=0)

if(CMAKE_SYSTEM_NAME STREQUAL "Android")
    add_library(native_app_glue STATIC
        ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

    find_package (oboe REQUIRED CONFIG)

    set(CMAKE_SHARED_LINKER_FLAGS
        "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")
endif()

file(GLOB fillets_sources
    "*.cpp"
    "api/lua/*.cpp"
    "api/ogl/*.cpp"
    "subsystem/*.cpp"
    "subsystem/*/*.cpp"
    "game/*.cpp"
    "game/*/*.cpp")

if(CMAKE_SYSTEM_NAME STREQUAL "Android")
    file(GLOB fillets_android
        "platform/android/*.cpp"
        "platform/android/*/*.cpp"
        "api/jni.cpp")
    add_library(fillets SHARED ${fillets_sources} ${fillets_android})
else()
    file(GLOB fillets_linux
        "platform/linux/*.cpp"
        "platform/linux/*/*.cpp")
    add_executable(fillets ${fillets_sources} ${fillets_linux})
endif()

target_precompile_headers(fillets PRIVATE
    "common.h"
    "subsystem/graphics.h"
    "subsystem/audio.h"
    "subsystem/graphics.h"
    "subsystem/files.h"
    "subsystem/input.h"
    "subsystem/rng.h"
    "subsystem/script.h")

target_include_directories(fillets PRIVATE .)

if(CMAKE_SYSTEM_NAME STREQUAL "Android")
    file(GLOB lua_sources "lua/*.c")
    list(FILTER lua_sources EXCLUDE REGEX ".*lua/(lua|luac|script).c")

    add_library(lua SHARED ${lua_sources})

    target_compile_definitions(lua PRIVATE -DLUA_USE_POSIX)

    target_include_directories(fillets PRIVATE lua)

    target_include_directories(fillets PRIVATE
        ${ANDROID_NDK}/sources/android/native_app_glue)
endif()

target_link_libraries(fillets
    EGL
    GLESv2)

if(CMAKE_SYSTEM_NAME STREQUAL "Android")
    target_link_libraries(fillets
        lua
        android
        native_app_glue
        jnigraphics
        mediandk
        oboe::oboe
        log)
else()
    find_package(Lua REQUIRED)
    find_package(X11 REQUIRED)
    find_package(PNG REQUIRED)
    find_package(Ogg REQUIRED)
    target_link_libraries(fillets X11 png vorbisfile lua)
endif()
