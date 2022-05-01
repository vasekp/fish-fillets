cmake_minimum_required(VERSION 3.4.1)

set(${CMAKE_C_FLAGS}, "${CMAKE_C_FLAGS}")

add_library(native_app_glue STATIC
    ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

find_package (oboe REQUIRED CONFIG)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Werror -g")
set(CMAKE_C_FLAGS -D_FORTIFY_SOURCE=0)

set(CMAKE_SHARED_LINKER_FLAGS
    "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

file(GLOB fillets_sources
    "*.cpp"
    "platform/*.cpp"
    "platform/*/*.cpp"
    "subsystem/*.cpp"
    "subsystem/*/*.cpp")

add_library(fillets SHARED ${fillets_sources})

target_include_directories(fillets PRIVATE .
    ${ANDROID_NDK}/sources/android/native_app_glue)

target_link_libraries(fillets
    android
    native_app_glue
    jnigraphics
    mediandk
    EGL
    GLESv2
    oboe::oboe
    log)