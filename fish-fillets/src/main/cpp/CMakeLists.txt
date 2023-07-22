cmake_minimum_required(VERSION 3.20)
project(Fillets)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "-Wall -Werror -g -Wno-unused-variable")

set(FILLETS_DIR app)
set(OGG_DIR deps/ogg)
set(VORBIS_DIR deps/vorbis)
set(THEORA_DIR deps/theora)
set(LUA_DIR deps/lua)

set(fillets_sources
        "${FILLETS_DIR}/instance.cpp"
        "${FILLETS_DIR}/api/ogl/display.cpp"
        "${FILLETS_DIR}/api/ogl/framebuffer.cpp"
        "${FILLETS_DIR}/api/ogl/program.cpp"
        "${FILLETS_DIR}/api/ogl/shader.cpp"
        "${FILLETS_DIR}/api/ogl/texture.cpp"
        "${FILLETS_DIR}/api/ogg/lowlevel.cpp"
        "${FILLETS_DIR}/api/ogg/video.cpp"
        "${FILLETS_DIR}/subsystem/audio.cpp"
        "${FILLETS_DIR}/subsystem/audio/audiosource.cpp"
        "${FILLETS_DIR}/subsystem/audio/audiosourcelist.cpp"
        "${FILLETS_DIR}/subsystem/graphics.cpp"
        "${FILLETS_DIR}/subsystem/graphics/texture.cpp"
        "${FILLETS_DIR}/subsystem/graphics/image.cpp"
        "${FILLETS_DIR}/subsystem/graphics/program.cpp"
        "${FILLETS_DIR}/subsystem/graphics/shaders.cpp"
        "${FILLETS_DIR}/subsystem/graphics/drawtarget.cpp"
        "${FILLETS_DIR}/subsystem/graphics/texturetarget.cpp"
        "${FILLETS_DIR}/subsystem/graphics/graphicssystem.cpp"
        "${FILLETS_DIR}/subsystem/input/key.cpp"
        "${FILLETS_DIR}/subsystem/persist.cpp"
        "${FILLETS_DIR}/subsystem/rng.cpp"
        "${FILLETS_DIR}/subsystem/script.cpp"
        "${FILLETS_DIR}/subsystem/script/callback.cpp"
        "${FILLETS_DIR}/game/logic/shape.cpp"
        "${FILLETS_DIR}/game/logic/model.cpp"
        "${FILLETS_DIR}/game/logic/layout.cpp"
        "${FILLETS_DIR}/game/logic/level.cpp"
        "${FILLETS_DIR}/game/logic/rules.cpp"
        "${FILLETS_DIR}/game/logic/interface.cpp"
        "${FILLETS_DIR}/game/logic/timer.cpp"
        "${FILLETS_DIR}/game/graphics/modelanim.cpp"
        "${FILLETS_DIR}/game/graphics/subtitles.cpp"
        "${FILLETS_DIR}/game/graphics/hint.cpp"
        "${FILLETS_DIR}/game/graphics/zxeffect.cpp"
        "${FILLETS_DIR}/game/structure/gametree.cpp"
        "${FILLETS_DIR}/game/structure/levelrecord.cpp"
        "${FILLETS_DIR}/game/screens/screenmanager.cpp"
        "${FILLETS_DIR}/game/screens/gamescreen.cpp"
        "${FILLETS_DIR}/game/screens/worldmap.cpp"
        "${FILLETS_DIR}/game/screens/credits.cpp"
        "${FILLETS_DIR}/game/screens/intro.cpp"
        "${FILLETS_DIR}/game/screens/levelscreen.cpp"
        "${FILLETS_DIR}/game/screens/leveltitle.cpp"
        "${FILLETS_DIR}/game/screens/baseinput.cpp"
        "${FILLETS_DIR}/game/screens/levelinput.cpp"
        "${FILLETS_DIR}/game/screens/pedometer.cpp"
        "${FILLETS_DIR}/game/screens/poster.cpp"
        "${FILLETS_DIR}/game/screens/options.cpp")

if(CMAKE_SYSTEM_NAME STREQUAL "Android")
    set(fillets_android
            "${FILLETS_DIR}/platform/android/afont.cpp"
            "${FILLETS_DIR}/platform/android/ainstance.cpp"
            "${FILLETS_DIR}/platform/android/decoders.cpp"
            "${FILLETS_DIR}/platform/android/files.cpp"
            "${FILLETS_DIR}/platform/android/files/system.cpp"
            "${FILLETS_DIR}/platform/android/files/user.cpp"
            "${FILLETS_DIR}/platform/android/input.cpp"
            "${FILLETS_DIR}/platform/android/log.cpp"
            "${FILLETS_DIR}/platform/android/main.cpp"
            "${FILLETS_DIR}/platform/android/oboestream.cpp"
            "${FILLETS_DIR}/api/jni.cpp")

    add_library(fillets SHARED ${fillets_sources} ${fillets_android})
else()
    set(fillets_linux
            "${FILLETS_DIR}/platform/linux/alsasink.cpp"
            "${FILLETS_DIR}/platform/linux/files.cpp"
            "${FILLETS_DIR}/platform/linux/files/system.cpp"
            "${FILLETS_DIR}/platform/linux/files/user.cpp"
            "${FILLETS_DIR}/platform/linux/ft-font.cpp"
            "${FILLETS_DIR}/platform/linux/input.cpp"
            "${FILLETS_DIR}/platform/linux/loadogg.cpp"
            "${FILLETS_DIR}/platform/linux/loadpng.cpp"
            "${FILLETS_DIR}/platform/linux/log.cpp"
            "${FILLETS_DIR}/platform/linux/main.cpp"
            "${FILLETS_DIR}/platform/linux/xinstance.cpp")

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

    set(lua_sources
            ${LUA_DIR}/lapi.c
            ${LUA_DIR}/lauxlib.c
            ${LUA_DIR}/lbaselib.c
            ${LUA_DIR}/lcode.c
            ${LUA_DIR}/lcorolib.c
            ${LUA_DIR}/lctype.c
            ${LUA_DIR}/ldblib.c
            ${LUA_DIR}/ldebug.c
            ${LUA_DIR}/ldo.c
            ${LUA_DIR}/ldump.c
            ${LUA_DIR}/lfunc.c
            ${LUA_DIR}/lgc.c
            ${LUA_DIR}/linit.c
            ${LUA_DIR}/liolib.c
            ${LUA_DIR}/llex.c
            ${LUA_DIR}/lmathlib.c
            ${LUA_DIR}/lmem.c
            ${LUA_DIR}/loadlib.c
            ${LUA_DIR}/lobject.c
            ${LUA_DIR}/lopcodes.c
            ${LUA_DIR}/loslib.c
            ${LUA_DIR}/lparser.c
            ${LUA_DIR}/lstate.c
            ${LUA_DIR}/lstring.c
            ${LUA_DIR}/lstrlib.c
            ${LUA_DIR}/ltable.c
            ${LUA_DIR}/ltablib.c
            ${LUA_DIR}/ltests.c
            ${LUA_DIR}/ltm.c
            ${LUA_DIR}/lundump.c
            ${LUA_DIR}/lutf8lib.c
            ${LUA_DIR}/lvm.c
            ${LUA_DIR}/lzio.c)

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
