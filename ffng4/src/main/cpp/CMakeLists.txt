cmake_minimum_required(VERSION 3.4.1)

set(${CMAKE_C_FLAGS}, "${CMAKE_C_FLAGS}")

add_library(native_app_glue STATIC
    ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-std=c++20 -Wall -Werror")
set(CMAKE_C_FLAGS -D_FORTIFY_SOURCE=0)

set(CMAKE_SHARED_LINKER_FLAGS
    "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

add_library(fillets SHARED main.cpp)

target_include_directories(fillets PRIVATE
    ${ANDROID_NDK}/sources/android/native_app_glue)

target_link_libraries(fillets
    android
    native_app_glue
    jnigraphics
    EGL
    GLESv1_CM
    log)