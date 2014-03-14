#include <renderers/debugrenderer.hpp>
#include <core/camera.hpp>
#include <core/primitive.hpp>
#include <core/scene.hpp>

namespace paprika {
namespace renderer {

DebugRenderer::DebugRenderer(core::Scene *scene, core::Camera *camera, OSL::ShadingAttribStateRef backgroundShaderState, OSL::ShadingSystem *shadingSystem) : 
    Renderer(scene, camera, backgroundShaderState, shadingSystem)
{
}

void DebugRenderer::render()
{
    static OIIO::ustring u_Cout("Cout");

    OSL::PerThreadInfo *threadInfo = shadingSystem_->create_thread_info();
    OSL::ShadingContext *ctx = shadingSystem_->get_context(threadInfo);

    int xres = camera_->xres();
    int yres = camera_->yres();
    std::vector<float> pixels(xres * yres * 3);

    for (int y = 0; y < yres; ++y)
    {
        for (int x = 0; x < xres; ++x)
        {
            int index = x + y * xres;

            core::CameraSample sample = { x + 0.5f, y + 0.5f, 0.f, 0.f, 0.f };
            core::Ray ray;
            camera_->generateRay(sample, &ray);

            core::InterpolationInfo interp;
            OSL::ShaderGlobals sg;
            memset(&sg, 0, sizeof(sg));
            core::Primitive *primitive = scene_->intersect(ray, &interp, &sg);

            if (primitive)
            {
                shadingSystem_->execute(*ctx, *primitive->shaderState(), sg);

                OIIO::TypeDesc t;
                const float *data = (const float *)shadingSystem_->get_symbol(*ctx, u_Cout, t);

                pixels[index * 3] = data[0];
                pixels[index * 3 + 1] = data[1];
                pixels[index * 3 + 2] = data[2];
            }
            else
            {
                pixels[index * 3] = 0.f;
                pixels[index * 3 + 1] = 0.f;
                pixels[index * 3 + 2] = 0.f;
            }
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
