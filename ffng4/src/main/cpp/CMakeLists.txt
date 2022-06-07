cmake_minimum_required(VERSION 3.20)
project(Fillets)

set(${CMAKE_C_FLAGS}, "${CMAKE_C_FLAGS}")

add_library(native_app_glue STATIC
    ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

find_package (oboe REQUIRED CONFIG)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "-Wall -Werror -g")
set(CMAKE_C_FLAGS -D_FORTIFY_SOURCE=0)

set(CMAKE_SHARED_LINKER_FLAGS
    "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

file(GLOB fillets_sources
    "*.cpp"
    "api/*.cpp"
    "api/*/*.cpp"
    "subsystem/*.cpp"
    "subsystem/*/*.cpp"
    "game/*.cpp"
    "game/*/*.cpp")

add_library(fillets SHARED ${fillets_sources})

file(GLOB lua_sources "lua/*.c")

add_library(lua
        STATIC
        ${lua_sources} )

target_compile_definitions(lua PRIVATE -DLUA_USE_POSIX)

target_include_directories(fillets PRIVATE . lua
        ${ANDROID_NDK}/sources/android/native_app_glue)

target_link_libraries(fillets
    android
    native_app_glue
    jnigraphics
    mediandk
    EGL
    GLESv2
    oboe::oboe
    lua
    log)
