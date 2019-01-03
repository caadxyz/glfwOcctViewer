cmake_minimum_required(VERSION 3.6)
project(glfw-occt-demo)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_VERBOSE_MAKEFILE 1)
set(APP_VERSION_MAJOR 1)
set(APP_VERSION_MINOR 0)
set(APP_TARGET glfwocct)

include_directories(${PROJECT_SOURCE_DIR})
file(GLOB SOURCES
    *.h
    *.cpp
)

# opencasacde 
INCLUDE_DIRECTORIES(/usr/local/include/opencascade/)
SET(OpenCASCADE_LIBS 
    TKernel
    TKService 
    TKV3d 
    TKOpenGl
    TKBRep
    TKGeomBase
    TKGeomAlgo
    TKG3d
    TKG2d
    TKTopAlgo
    TKPrim
    TKBool
    TKBO
    TKFillet
    TKOffset 
   )

# opengl
find_package(OpenGL REQUIRED)

# glfw
find_package(glfw3 REQUIRED)
include_directories(${GLFW_INCLUDE_DIRS})
link_libraries(${GLFW_LIBRARY_DIRS})

# glew
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})

add_executable(${APP_TARGET} ${SOURCES})
target_link_libraries(
        ${APP_TARGET}
        ${OPENGL_LIBRARIES}
        ${GLEW_LIBRARIES}
        glfw
        ${OpenCASCADE_LIBS}
)
