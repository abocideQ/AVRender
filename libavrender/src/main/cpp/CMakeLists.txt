cmake_minimum_required(VERSION 3.10.2)

# set source
set(SOURCE_HEAD
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/include/glm)
set(SOURCE_CPP
        ${CMAKE_SOURCE_DIR}/render_jni.cpp
        ${CMAKE_SOURCE_DIR}/src/common.h
        ${CMAKE_SOURCE_DIR}/src/egl_context.cpp
        ${CMAKE_SOURCE_DIR}/src/gl2_plane.cpp
        ${CMAKE_SOURCE_DIR}/src/gl3_plane.cpp
        ${CMAKE_SOURCE_DIR}/src/gl3_box.cpp
        ${CMAKE_SOURCE_DIR}/src/gl3_light.cpp
        ${CMAKE_SOURCE_DIR}/src/gl3_sky_box.cpp)
include_directories(${SOURCE_HEAD})
add_library(avrender
        SHARED
        ${SOURCE_CPP})

# link libs
target_link_libraries(avrender
        EGL
        GLESv3
        android
        log)
