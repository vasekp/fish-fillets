cmake_minimum_required(VERSION 3.20)
project(Fillets)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "-Wall -Werror -g -Wno-unused-variable")
set(CMAKE_C_FLAGS "-D_FORTIFY_SOURCE=0 -Wno-incompatible-library-redeclaration")

file(GLOB fillets_sources
        "*.cpp"
        "api/lua/*.cpp"
        "api/ogl/*.cpp"
        "api/ogg/*.cpp"
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

if(CMAKE_SYSTEM_NAME STREQUAL "Android")
    add_library(native_app_glue STATIC
            ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

    set(CMAKE_SHARED_LINKER_FLAGS
            "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

    target_include_directories(fillets PRIVATE
            ${ANDROID_NDK}/sources/android/native_app_glue
            deps/lua
            deps/ogg/include
            deps/vorbis/include
            deps/theora/include)

    file(GLOB lua_sources "deps/lua/*.c")
    list(FILTER lua_sources EXCLUDE REGEX ".*lua/(lua|luac|script|onelua).c")

    add_library(lua SHARED ${lua_sources})

    target_compile_definitions(lua PRIVATE -DLUA_USE_POSIX)

    find_package(oboe REQUIRED CONFIG)

    add_subdirectory(deps/ogg)
    add_subdirectory(deps/vorbis)

    add_library(theoradec
            deps/theora/lib/apiwrapper.c
            deps/theora/lib/bitpack.c
            deps/theora/lib/decapiwrapper.c
            deps/theora/lib/decinfo.c
            deps/theora/lib/decode.c
            deps/theora/lib/dequant.c
            deps/theora/lib/fragment.c
            deps/theora/lib/huffdec.c
            deps/theora/lib/idct.c
            deps/theora/lib/info.c
            deps/theora/lib/internal.c
            deps/theora/lib/quant.c
            deps/theora/lib/state.c)

    target_include_directories(theoradec PRIVATE
            deps/ogg/include
            deps/theora/include
            ${CMAKE_CURRENT_BINARY_DIR}/deps/ogg/include)

    target_compile_options(theoradec PRIVATE -Wno-shift-negative-value
            -Wno-shift-op-parentheses)

    target_link_libraries(fillets
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
            asound
            Freetype::Freetype)
endif()

target_include_directories(fillets PRIVATE .)

target_link_libraries(fillets
        EGL
        GLESv2
        lua
        ogg
        vorbis
        theoradec)
