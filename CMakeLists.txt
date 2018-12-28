cmake_minimum_required(VERSION 3.6)
project(glfw-occ-cmake)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_VERBOSE_MAKEFILE 1)
set(APP_VERSION_MAJOR 1)
set(APP_VERSION_MINOR 0)

set(APP_TARGET glfwocc)

include_directories(${PROJECT_SOURCE_DIR})
file(GLOB SOURCES
    *.h
    *.cpp
)
add_executable(${APP_TARGET} ${SOURCES})

# opengl
find_package(OpenGL REQUIRED)

# glfw
find_package(glfw3 REQUIRED)
include_directories(${GLFW_INCLUDE_DIRS})
link_libraries(${GLFW_LIBRARY_DIRS})

# glew
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})

# opencasacde
INCLUDE_DIRECTORIES(/usr/local/include/opencascade/)
SET(EXTRA_LIBS 
    /usr/local/Cellar/opencascade/7.3.0/lib/libTKernel.dylib
    /usr/local/Cellar/opencascade/7.3.0/lib/libTKMath.dylib
    /usr/local/Cellar/opencascade/7.3.0/lib/libTKService.dylib
    /usr/local/Cellar/opencascade/7.3.0/lib/libTKV3d.dylib
    /usr/local/Cellar/opencascade/7.3.0/lib/libTKOpenGl.dylib
    /usr/local/Cellar/opencascade/7.3.0/lib/libTKBRep.dylib
    /usr/local/Cellar/opencascade/7.3.0/lib/libTKGeomBase.dylib
    /usr/local/Cellar/opencascade/7.3.0/lib/libTKGeomAlgo.dylib
    /usr/local/Cellar/opencascade/7.3.0/lib/libTKG3d.dylib
    /usr/local/Cellar/opencascade/7.3.0/lib/libTKG2d.dylib
    /usr/local/Cellar/opencascade/7.3.0/lib/libTKTopAlgo.dylib
    /usr/local/Cellar/opencascade/7.3.0/lib/libTKPrim.dylib
    /usr/local/Cellar/opencascade/7.3.0/lib/libTKBool.dylib
    /usr/local/Cellar/opencascade/7.3.0/lib/libTKBO.dylib
    /usr/local/Cellar/opencascade/7.3.0/lib/libTKFillet.dylib
    /usr/local/Cellar/opencascade/7.3.0/lib/libTKOffset.dylib
    )

target_link_libraries(
        ${APP_TARGET}
        ${OPENGL_LIBRARIES}
        ${GLEW_LIBRARIES}
        glfw
        ${OPENSCENEGRAPH_LIBRARIES}
        ${EXTRA_LIBS}
)
