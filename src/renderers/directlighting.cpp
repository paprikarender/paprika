#include <renderers/directlighting.hpp>

namespace paprika {
namespace renderer {

DirectLighting::DirectLighting(core::Scene *scene, core::Camera *camera, OSL::ShaderGroupRef backgroundShaderGroup, OSL::ShadingSystem *shadingSystem) :
    Renderer(scene, camera, backgroundShaderGroup, shadingSystem)
{

}

DirectLighting::~DirectLighting()
{

}


}
}
