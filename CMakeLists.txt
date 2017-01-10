cmake_minimum_required(VERSION 3.0)
project(paprika)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")

include_directories(src src/api /home/atilim/oiio-Release-1.7.9/dist/linux64/include /home/atilim/OpenShadingLanguage-Release-1.7.5/dist/linux64/include /home/atilim/embree-2.13.0.x86_64.linux/include /usr/include/lua5.1)

find_package(Boost COMPONENTS system thread REQUIRED)

find_package(PkgConfig)
pkg_search_module(OpenEXR REQUIRED OpenEXR)

find_library(OIIO_LIBRARIES
            NAMES OpenImageIO
            PATHS /home/atilim/oiio-Release-1.7.9/dist/linux64/lib)

find_library(OSL_LIBRARIES
            NAMES oslexec oslcomp oslquery
            PATHS /home/atilim/OpenShadingLanguage-Release-1.7.5/dist/linux64/lib)

find_library(EMBREE_LIBRARIES
            NAMES embree
            PATHS /home/atilim/embree-2.13.0.x86_64.linux/lib)

set(SOURCE_FILES
    src/api/paprikaapi.cpp
    src/core/camera.cpp
    src/core/debug.cpp
    src/core/generator.cpp
    src/core/geometry.cpp
    src/core/mc.cpp
    src/core/parametermap.cpp
    src/core/paramitem.cpp
    src/core/paramtype.cpp
    src/core/primitive.cpp
    src/core/projectivecamera.cpp
    src/core/renderer.cpp
    src/core/scene.cpp
    src/core/shape.cpp
    src/generators/luagenerator.cpp
    src/generators/trimeshgenerator.cpp
    src/OSL/shading.cpp
    src/renderers/debugrenderer.cpp
    src/renderers/pathtracer.cpp
    src/shapes/mesh.cpp
    src/shapes/sphere.cpp
    src/shapes/triangulate.cpp
    src/paprika.cpp
    src/cameras/perspectivecamera.cpp
)
add_executable(paprika ${SOURCE_FILES})

target_link_libraries(paprika ${OIIO_LIBRARIES} ${OSL_LIBRARIES} ${EMBREE_LIBRARIES} ${OpenEXR_LIBRARIES} lua5.1 ${Boost_LIBRARIES})
