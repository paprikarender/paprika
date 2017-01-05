cmake_minimum_required(VERSION 2.8.4)
project(paprika)

find_package(Boost COMPONENTS system REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(src src/api /root/oiio-Release-1.7.9/dist/linux64/include /root/OpenShadingLanguage-Release-1.7.5/dist/linux64/include /root/embree-2.13.0.x86_64.linux/include /usr/include/lua5.1)

find_library(OIIO_LIBRARIES
            NAMES OpenImageIO
            PATHS /root/oiio-Release-1.7.9/dist/linux64/lib)

find_library(OSL_LIBRARIES
            NAMES oslexec oslcomp oslquery
            PATHS /root/OpenShadingLanguage-Release-1.7.5/dist/linux64/lib)

find_library(EMBREE_LIBRARIES
            NAMES embree
            PATHS /root/embree-2.13.0.x86_64.linux/lib)

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
)
add_executable(paprika ${SOURCE_FILES})

target_link_libraries(paprika ${OIIO_LIBRARIES} ${OSL_LIBRARIES} ${EMBREE_LIBRARIES} lua5.1 ${Boost_LIBRARIES})
