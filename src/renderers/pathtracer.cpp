#include <renderers/pathtracer.hpp>
#include <core/camera.hpp>
#include <core/primitive.hpp>
#include <core/scene.hpp>
#include <OSL/shading.h>
#include <OSL/sampling.h>

namespace paprika {
namespace renderer {

static OSL::Rng rng(time(NULL));

struct EvalBackgroundData
{
    OSL::ShadingSystem *shadingSystem;
    OSL::ShadingContext *ctx;
    OSL::ShadingAttribStateRef shaderState;
};

static core::Vec3 eval_background(const OSL::Dual2<core::Vec3> &dir, EvalBackgroundData *data)
{
    OSL::ShaderGlobals sg;
    memset(&sg, 0, sizeof(OSL::ShaderGlobals));
    sg.I = dir.val();
    sg.dIdx = dir.dx();
    sg.dIdy = dir.dy();
    data->shadingSystem->execute(*data->ctx, *data->shaderState, sg);
    return OSL::process_background_closure(sg.Ci);
}

PathTracer::PathTracer(core::Scene *scene, core::Camera *camera, OSL::ShadingAttribStateRef backgroundShaderState, OSL::ShadingSystem *shadingSystem) : 
    Renderer(scene, camera, backgroundShaderState, shadingSystem)
{
    for (std::size_t i = 0; i < scene_->primitives().size(); ++i)
    {
        core::Primitive *primitive = scene_->primitives()[i];
        if (primitive->isEmissive())
            lights_.push_back(primitive);
    }

    if (backgroundShaderState_)
    {
        background_ = new OSL::Background;

        OSL::PerThreadInfo *threadInfo = shadingSystem_->create_thread_info();
        OSL::ShadingContext *ctx = shadingSystem_->get_context(threadInfo);

        EvalBackgroundData data;
        data.shadingSystem = shadingSystem_;
        data.ctx = ctx;
        data.shaderState = backgroundShaderState_;

        background_->prepare(128, eval_background, &data);

        shadingSystem_->release_context(ctx);
        shadingSystem_->destroy_thread_info(threadInfo);
    }
    else
        background_ = NULL;
}

PathTracer::~PathTracer()
{
    delete background_;
}

static float powerHeuristic(float a, float b)
{
    return (a * a) / (a * a + b * b);
}

core::Color3 PathTracer::estimateDirect(OSL::ShadingContext *ctx,
                                        const OSL::ShaderGlobals &sg,
                                        OSL::CompositeBSDF &bsdf)
{
    int nLights = lights_.size();

    if (background_)
        nLights++;

    if (nLights == 0)
        return core::Color3(0.f, 0.f, 0.f);

    int lightNum = (int)(rng * nLights);
    lightNum = std::min(lightNum, nLights - 1);

    core::Primitive *light;
    if (background_ && lightNum == nLights - 1)
        light = NULL;
    else
        light = lights_[lightNum];

    float pdfLightSelect = 1.f / nLights;

    core::Color3 Ld(0.f, 0.f, 0.f);

    if (light == NULL)
    {
        // sample background
        do
        {
            OSL::Dual2<core::Vec3> wi;
            float invpdf;
            core::Color3 Le = background_->sample(rng, rng, wi, invpdf);

            if (invpdf == 0 || Le == core::Color3(0, 0, 0))
                break;

            float pdfLight = pdfLightSelect / invpdf;

            float pdfBsdf;
            core::Color3 f = bsdf.eval(sg, wi.val(), pdfBsdf);

            if (f == core::Color3(0, 0, 0))
                break;

            if (!scene_->isVisible(core::Ray(sg.P, wi)))
                break;

            float weight = powerHeuristic(pdfLight, pdfBsdf);
            Ld += (f * Le) * (weight / pdfLight);
        } while (0);
    }
    else
    {
        // sample light
        do
        {
            int primIDLight;
            core::Vec3 pLight, nLight;
            light->sample(sg.P, rng, rng, rng, &primIDLight, &pLight, &nLight);
            float pdfLight = light->pdf(sg.P, pLight, nLight);

            if (pdfLight == 0)
                break;

            pdfLight *= pdfLightSelect;

            core::InterpolationInfo interpLight;
            OSL::ShaderGlobals sgLight;
            light->fillIntersectionInfo(pLight, nLight, primIDLight, &interpLight, &sgLight);

            core::Vec3 wi = (sgLight.P - sg.P).normalized();

            if (wi.dot(sgLight.Ng) >= 0)
                break;

            shadingSystem_->execute(*ctx, *light->shaderState(), sgLight);
            OSL::ShadingResult resultLight;
            OSL::process_closure(resultLight, sgLight.Ci, true);

            core::Color3 Le = resultLight.Le;

            if (Le == core::Color3(0, 0, 0))
                break;

            float pdfBsdf;
            core::Color3 f = bsdf.eval(sg, wi, pdfBsdf);

            if (f == core::Color3(0, 0, 0))
                break;

            if (!scene_->isVisible(sg.P, sgLight.P))
                break;

            float weight = powerHeuristic(pdfLight, pdfBsdf);
            Ld += (f * Le) * (weight / pdfLight);
        } while (0);
    }

    // sample bsdf
    do
    {
        OSL::Dual2<core::Vec3> wi;
        float invpdf;
        bsdf.sample(sg, rng, rng, wi, invpdf);

        if (invpdf == 0)
            break;

        float pdfBsdf;
        core::Color3 f = bsdf.eval(sg, wi.val(), pdfBsdf);

        if (pdfBsdf == 0 || f == core::Color3(0, 0, 0))
            break;

        core::InterpolationInfo interpLight;
        OSL::ShaderGlobals sgLight;
        core::Primitive *primitive = scene_->intersect(core::Ray(sg.P, wi), &interpLight, &sgLight);
        
        if (primitive == NULL)
        {
            // evaluate background
            if (!background_)
                break;

            float pdfLight;
            core::Color3 Le = background_->eval(wi.val(), pdfLight);

            if (pdfLight == 0)
                break;

            pdfLight *= pdfLightSelect;

            float weight = powerHeuristic(pdfBsdf, pdfLight);
            Ld += (f * Le) * (weight / pdfBsdf);
        }
        else
        {
            // evaluate light
            if (!primitive->isEmissive())
                break;

            if (sgLight.backfacing)
                break;

            float pdfLight = light->pdf(sg.P, sgLight.P, sgLight.Ng);

            if (pdfLight == 0)
                break;

            pdfLight *= pdfLightSelect;

            shadingSystem_->execute(*ctx, *light->shaderState(), sgLight);
            OSL::ShadingResult resultLight;
            OSL::process_closure(resultLight, sgLight.Ci, true);

            float weight = powerHeuristic(pdfBsdf, pdfLight);
            Ld += (f * resultLight.Le) * (weight / pdfBsdf);
        }
    } while (0);

    return Ld;
}

core::Color3 PathTracer::Li(OSL::ShadingContext *ctx, float x, float y)
{
    core::Color3 pathThroughput(1.f, 1.f, 1.f);
    core::Color3 L(0.f, 0.f, 0.f);

    core::CameraSample sample = { x + rng, y + rng, 0.f, 0.f, 0.f };
    core::Ray ray;
    camera_->generateRay(sample, &ray);

    bool specular = false;

    for (int bounces = 0; ; ++bounces)
    {
        core::InterpolationInfo interp;
        OSL::ShaderGlobals sg;
        memset(&sg, 0, sizeof(sg));
        core::Primitive *primitive = scene_->intersect(ray, &interp, &sg);

        // evaluate background
        if (primitive == NULL)
        {
            if (bounces == 0 || specular)
            {
                if (backgroundShaderState_)
                {
                    OSL::ShaderGlobals sg;
                    memset(&sg, 0, sizeof(OSL::ShaderGlobals));
                    sg.I = ray.d.val();
                    sg.dIdx = ray.d.dx();
                    sg.dIdy = ray.d.dy();
                    shadingSystem_->execute(*ctx, *backgroundShaderState_, sg);
                    L += pathThroughput * OSL::process_background_closure(sg.Ci);
                }
            }

            break;
        }

        // execute shader and process the resulting list of closures
        shadingSystem_->execute(*ctx, *primitive->shaderState(), sg);
        OSL::ShadingResult result;
        OSL::process_closure(result, sg.Ci, false);

        // build internal pdf for sampling between bsdf closures
        result.bsdf.prepare(sg, core::Color3(1, 1, 1), false);

        if (bounces == 0 || specular)
            L += pathThroughput * result.Le;

        OSL::CompositeBSDF &bsdf = result.bsdf;

        // sample illumination from lights to find path contribution
        L += pathThroughput * estimateDirect(ctx, sg, bsdf);

        // sample BSDF to get new path direction
        OSL::Dual2<core::Vec3> wi;
        float invpdf;
        pathThroughput *= bsdf.sample(sg, rng, rng, wi, invpdf);

        if (!(pathThroughput.x > 0) && !(pathThroughput.y > 0) && !(pathThroughput.z > 0))
            break;

        specular = (invpdf == 0);

        ray = core::Ray(OSL::Dual2<core::Vec3>(sg.P, sg.dPdx, sg.dPdy), wi);

        // possibly terminate the path
        if (bounces > 3)
        {
            float continueProbability = std::min(0.5f, pathThroughput.length());
            if (rng > continueProbability)
                break;
            pathThroughput /= continueProbability;
        }
    }

    return L;
}

void PathTracer::render()
{
    OSL::PerThreadInfo *threadInfo = shadingSystem_->create_thread_info();
    OSL::ShadingContext *ctx = shadingSystem_->get_context(threadInfo);

    int xres = camera_->xres();
    int yres = camera_->yres();
    std::vector<float> pixels(xres * yres * 3);

    int perc = -1;

    for (int y = 0; y < yres; ++y)
    {
        for (int x = 0; x < xres; ++x)
        {
            core::Color3 c(0.f, 0.f, 0.f);
            for (int i = 0; i < 64; ++i)
                c += Li(ctx, x, y) / 64.f;

            int index = x + y * xres;

            pixels[index * 3] = pow(c.x, 1.f / 2.2f);
            pixels[index * 3 + 1] = pow(c.y, 1.f / 2.2f);
            pixels[index * 3 + 2] = pow(c.z, 1.f / 2.2f);
        }

        int newPerc = (100 * (y + 1)) / yres;
        if (perc != newPerc)
        {
            printf("%d\n", newPerc);
            perc = newPerc;
        }
    }

    shadingSystem_->release_context(ctx);
    shadingSystem_->destroy_thread_info(threadInfo);

    const char *imagefile = "out.png";
    OIIO::ImageOutput *out = OIIO::ImageOutput::create(imagefile);
    OIIO::ImageSpec spec(xres, yres, 3, OIIO::TypeDesc::FLOAT);
    if (out && out->open(imagefile, spec))
        out->write_image(OIIO::TypeDesc::TypeFloat, &pixels[0]);
    delete out;
}


}
}
