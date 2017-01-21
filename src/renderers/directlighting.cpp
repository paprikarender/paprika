#include <renderers/directlighting.hpp>
#include <core/primitive.hpp>
#include <core/scene.hpp>

namespace paprika {
namespace renderer {

static OSL::Rng rng(time(NULL));

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

    int lightNum = (int)(rng * nLights);
    lightNum = std::min(lightNum, nLights - 1);

    core::Primitive *light = lights_[lightNum];

    float pdfLightSelect = 1.f / nLights;

 
    int primIDLight;
    core::Vec3 pLight, nLight;
    light->sample(sg.P, rng, rng, rng, &primIDLight, &pLight, &nLight);
    float pdfLight = light->pdf(sg.P, pLight, nLight);

    if (pdfLight == 0)
        return core::Color3();

    pdfLight *= pdfLightSelect;
 
 
 
 
 
    return core::Color3();
}

}
}
