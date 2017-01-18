#ifndef DIRECTLIGHTING_HPP
#define DIRECTLIGHTING_HPP

#include <core/renderer.hpp>
#include <core/geometry.hpp>
#include <vector>
#include <OSL/shading.h>
#include <OSL/background.h>

namespace paprika {

namespace core {
class Primitive;
}

namespace renderer {

class DirectLighting : public core::Renderer
{
public:
    DirectLighting(core::Scene *scene, core::Camera *camera, OSL::ShaderGroupRef backgroundShaderGroup, OSL::ShadingSystem *shadingSystem);
    ~DirectLighting();

    virtual void render();

private:
    // core::Color3 Li(OSL::ShadingContext *ctx, float x, float y);
    // core::Color3 estimateDirect(OSL::ShadingContext *ctx,
    //                             const OSL::ShaderGlobals &sg,
    //                             OSL::CompositeBSDF &bsdf);

    // std::vector<core::Primitive*> lights_;

    // OSL::Background *background_;
};

}
}

#endif