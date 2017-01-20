#include <renderers/directlighting.hpp>
#include <core/primitive.hpp>
#include <core/scene.hpp>

namespace paprika {
namespace renderer {

DirectLighting::DirectLighting(core::Scene *scene, core::Camera *camera, OSL::ShaderGroupRef backgroundShaderGroup, OSL::ShadingSystem *shadingSystem) :
    Renderer(scene, camera, backgroundShaderGroup, shadingSystem)
{
    for (std::size_t i = 0; i < scene_->primitives().size(); ++i)
    {
        core::Primitive *primitive = scene_->primitives()[i];
        if (primitive->isEmissive())
            lights_.push_back(primitive);
    }
}

DirectLighting::~DirectLighting()
{

}

void DirectLighting::render()
{

}

core::Color3 DirectLighting::Li(OSL::ShadingContext *ctx, const core::Ray& ray)
{
    core::InterpolationInfo interp;
    OSL::ShaderGlobals sg;
    memset(&sg, 0, sizeof(sg));
    core::Primitive *primitive = scene_->intersect(ray, &interp, &sg);

    // evaluate background
    if (primitive == NULL)
    {
        return core::Color3();
    }

    // execute shader and process the resulting list of closures
    shadingSystem_->execute(ctx, *primitive->shaderGroup(), sg);
    OSL::ShadingResult result;
    OSL::process_closure(result, sg.Ci, false);

    // build internal pdf for sampling between bsdf closures
    result.bsdf.prepare(sg, core::Color3(1, 1, 1), false);



    // sample 1 light randomly
    int nLights = lights_.size();

    if (nLights == 0)
        return core::Color3(0.f, 0.f, 0.f);


    return core::Color3();
}

}
}
