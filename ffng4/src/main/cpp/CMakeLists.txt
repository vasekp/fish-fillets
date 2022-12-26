cmake_minimum_required(VERSION 3.20)
project(Fillets)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "-Wall -Werror -g")
set(CMAKE_C_FLAGS -D_FORTIFY_SOURCE=0)

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

target_include_directories(fillets PRIVATE .)

target_link_libraries(fillets
        EGL
        GLESv2)

if(CMAKE_SYSTEM_NAME STREQUAL "Android")
    add_library(native_app_glue STATIC
            ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

    set(CMAKE_SHARED_LINKER_FLAGS
            "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

    target_include_directories(fillets PRIVATE
            ${ANDROID_NDK}/sources/android/native_app_glue)

    file(GLOB lua_sources "lua/*.c")
    list(FILTER lua_sources EXCLUDE REGEX ".*lua/(lua|luac|script).c")

    add_library(lua SHARED ${lua_sources})

    target_compile_definitions(lua PRIVATE -DLUA_USE_POSIX)

    find_package(oboe REQUIRED CONFIG)

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
    find_package(ALSA REQUIRED)
    find_package(Freetype REQUIRED)

    target_include_directories(fillets PRIVATE ${FREETYPE_INCLUDE_DIRS})

    target_link_libraries(fillets
            X11
            png
            vorbisfile
            lua
            asound
            Freetype::Freetype)
endif()
