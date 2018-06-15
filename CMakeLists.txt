cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_CXX_STANDARD 14)

include_directories(
            src/main/jni/
            src/main/jni/lua/
            src/main/jni/ff/game/
            src/main/jni/ff/gengine/
            src/main/jni/ff/plan/
            src/main/jni/ff/effect/
            src/main/jni/ff/menu/
            src/main/jni/ff/level/
            src/main/jni/ff/option/
            src/main/jni/ff/state/
            src/main/jni/ff/widget/ )

file( GLOB ffng_sources "src/main/jni/*.cpp" "src/main/jni/ff/*/*.cpp" )

add_library( ffng
             SHARED
             ${ffng_sources} )

file( GLOB lua_sources "src/main/jni/lua/*.c" )

add_library( lua
             STATIC
             ${lua_sources} )

target_link_libraries( ffng
                       lua
                       android
                       log
                       jnigraphics
                       EGL
                       GLESv2 )