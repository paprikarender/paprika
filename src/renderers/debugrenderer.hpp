#ifndef DEBUG_RENDERER_HPP
#define DEBUG_RENDERER_HPP

#include <core/renderer.hpp>

namespace paprika {
namespace renderer {

class DebugRenderer : public core::Renderer
{
public:
    DebugRenderer(core::Scene *scene, core::Camera *camera, OSL::ShaderGroupRef backgroundShaderGroup, OSL::ShadingSystem *shadingSystem);

    virtual void render();
};

}
}
#endif