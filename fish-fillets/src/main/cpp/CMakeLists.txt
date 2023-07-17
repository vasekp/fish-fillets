cmake_minimum_required(VERSION 3.20)
project(Fillets)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "-Wall -Werror -g -Wno-unused-variable")

set(FILLETS_DIR app)
set(OGG_DIR deps/ogg)
set(VORBIS_DIR deps/vorbis)
set(THEORA_DIR deps/theora)
set(LUA_DIR deps/lua)

file(GLOB fillets_sources
        "${FILLETS_DIR}/*.cpp"
        "${FILLETS_DIR}/api/lua/*.cpp"
        "${FILLETS_DIR}/api/ogl/*.cpp"
        "${FILLETS_DIR}/api/ogg/*.cpp"
        "${FILLETS_DIR}/subsystem/*.cpp"
        "${FILLETS_DIR}/subsystem/*/*.cpp"
        "${FILLETS_DIR}/game/*.cpp"
        "${FILLETS_DIR}/game/*/*.cpp")

if(CMAKE_SYSTEM_NAME STREQUAL "Android")
    file(GLOB fillets_android
            "${FILLETS_DIR}/platform/android/*.cpp"
            "${FILLETS_DIR}/platform/android/*/*.cpp"
            "${FILLETS_DIR}/api/jni.cpp")

    add_library(fillets SHARED ${fillets_sources} ${fillets_android})
else()
    file(GLOB fillets_linux
            "${FILLETS_DIR}/platform/linux/*.cpp"
            "${FILLETS_DIR}/platform/linux/*/*.cpp")

    add_executable(fillets ${fillets_sources} ${fillets_linux})
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Android")
    add_library(native_app_glue STATIC
            ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

    set(CMAKE_SHARED_LINKER_FLAGS
            "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

    find_package(oboe REQUIRED CONFIG)

    target_include_directories(fillets PRIVATE
            ${ANDROID_NDK}/sources/android/native_app_glue
            ${LUA_DIR}
            ${OGG_DIR}/include
            ${VORBIS_DIR}/include
            ${THEORA_DIR}/include)

    file(GLOB lua_sources "${LUA_DIR}/*.c")
    list(FILTER lua_sources EXCLUDE REGEX ".*lua/(lua|luac|script|onelua).c")

    add_library(lua SHARED ${lua_sources})

    target_compile_definitions(lua PRIVATE "-DLUA_USE_POSIX -D_FORTIFY_SOURCE=0")

    target_compile_options(lua PRIVATE -Wno-incompatible-library-redeclaration)

    add_subdirectory(${OGG_DIR})
    add_subdirectory(${VORBIS_DIR})

    add_library(theoradec
            ${THEORA_DIR}/lib/apiwrapper.c
            ${THEORA_DIR}/lib/bitpack.c
            ${THEORA_DIR}/lib/decapiwrapper.c
            ${THEORA_DIR}/lib/decinfo.c
            ${THEORA_DIR}/lib/decode.c
            ${THEORA_DIR}/lib/dequant.c
            ${THEORA_DIR}/lib/fragment.c
            ${THEORA_DIR}/lib/huffdec.c
            ${THEORA_DIR}/lib/idct.c
            ${THEORA_DIR}/lib/info.c
            ${THEORA_DIR}/lib/internal.c
            ${THEORA_DIR}/lib/quant.c
            ${THEORA_DIR}/lib/state.c)

    target_include_directories(theoradec PRIVATE
            ${OGG_DIR}/include
            ${THEORA_DIR}/include
            ${CMAKE_CURRENT_BINARY_DIR}/deps/ogg/include)

    target_compile_options(theoradec PRIVATE
            -Wno-shift-negative-value
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

target_include_directories(fillets PRIVATE ${FILLETS_DIR})

target_link_libraries(fillets
        EGL
        GLESv2
        lua
        ogg
        vorbis
        theoradec)
