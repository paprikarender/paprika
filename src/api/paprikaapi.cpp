#include <api/paprikaapi.hpp>
#include <core/parametermap.hpp>
#include <core/geometry.hpp>
#include <embree2/rtcore.h>
#include <embree2/rtcore_ray.h>
#include <shapes/mesh.hpp>
#include <core/primitive.hpp>
#include <cameras/perspectivecamera.hpp>
#include <core/scene.hpp>
#include <core/renderer.hpp>
#include <stack>
#include <OSL/oslexec.h>
#include <OSL/shading.h>
#include <shapes/sphere.hpp>
#include <renderers/pathtracer.hpp>
#include <renderers/debugrenderer.hpp>
#include <generators/luagenerator.hpp>
#include <generators/trimeshgenerator.hpp>

namespace paprika {

enum APIState
{
    STATE_OPTIONS,
    STATE_WORLD,
    STATE_SHADER,
};

struct PaprikaAPI::PaprikaData
{	
    APIState state;
    core::ParameterMap params;
    core::Transform ctm;
    std::stack<core::Transform> transformStack;
    std::vector<core::Primitive*> primitives;
    core::Camera *camera;
    core::RendererService rendererService;
    RTCDevice rtcDevice;
    OSL::ErrorHandler errorHandler;
    OSL::ShadingSystem *shadingSystem;
    core::Transform shaderTransform;
    OSL::ShaderGroupRef shaderGroup;
    OSL::ShaderGroupRef backgroundShaderGroup;
};

PaprikaAPI::PaprikaAPI()
{
    d_ = new PaprikaData;
    d_->rtcDevice = rtcNewDevice(NULL);
    d_->shadingSystem = new OSL::ShadingSystem(&d_->rendererService, NULL, &d_->errorHandler);
    register_closures(d_->shadingSystem);
    d_->shadingSystem->attribute("lockgeom", 1);
#if 1
    const char *aovnames[1] = { "Cout" };
    d_->shadingSystem->attribute("renderer_outputs", OIIO::TypeDesc(OIIO::TypeDesc::STRING, 1), aovnames);
#endif
    d_->state = STATE_OPTIONS;
    d_->camera = NULL;
}

PaprikaAPI::~PaprikaAPI()
{
    if (d_->camera)
        d_->camera->unref();

    for (std::size_t i = 0; i < d_->primitives.size(); ++i)
        d_->primitives[i]->unref();

    d_->backgroundShaderGroup = nullptr;
    d_->shaderGroup = nullptr;
    delete d_->shadingSystem;

    rtcDeleteDevice(d_->rtcDevice);

    delete d_;
}

//valid states
//ALL
void PaprikaAPI::parameter(const char* typedname, int val)
{
    d_->params.parameter(typedname, val);
}

//valid states
//ALL
void PaprikaAPI::parameter(const char* typedname, float val)
{
    d_->params.parameter(typedname, val);
}

//valid states
//ALL
void PaprikaAPI::parameter(const char* typedname, double val)
{
    d_->params.parameter(typedname, val);
}

//valid states
//ALL
void PaprikaAPI::parameter(const char* typedname, const char* val)
{
    d_->params.parameter(typedname, val);
}

//valid states
//ALL
void PaprikaAPI::parameter(const char* typedname, const int* val)
{
    d_->params.parameter(typedname, val);
}

//valid states
//ALL
void PaprikaAPI::parameter(const char* typedname, const float* val)
{
    d_->params.parameter(typedname, val);
}

//valid states
//ALL
void PaprikaAPI::parameter(const char* typedname, const char*const* val)
{
    d_->params.parameter(typedname, val);
}


//valid states
//STATE_OPTIONS
//STATE_WORLD
void PaprikaAPI::pushTransform()
{
    if (d_->state == STATE_SHADER)
    {
        core::Error("popTransform() command cannot be inside shader block. Skipping...");
        return;
    }

    d_->transformStack.push(d_->ctm);
}

//valid states
//STATE_OPTIONS
//STATE_WORLD
void PaprikaAPI::popTransform()
{
    if (d_->state == STATE_SHADER)
    {
        core::Error("popTransform() command cannot be inside shader block. Skipping...");
        return;
    }

    //TODO: test empty stack
    d_->ctm = d_->transformStack.top();
    d_->transformStack.pop();
}


//valid states
//STATE_OPTIONS
//STATE_WORLD
void PaprikaAPI::setTransform(const float* M)
{
    if (d_->state == STATE_SHADER)
    {
        core::Error("setTransform() command cannot be inside shader block. Skipping...");
        return;
    }

    d_->ctm = core::Transform(M);
}


//valid states
//STATE_OPTIONS
//STATE_WORLD
void PaprikaAPI::appendTransform(const float* M)
{
    if (d_->state == STATE_SHADER)
    {
        core::Error("appendTransform() command cannot be inside shader block. Skipping...");
        return;
    }

    d_->ctm = core::Transform(M) * d_->ctm;
}


//valid states
//STATE_OPTIONS
//STATE_WORLD
void PaprikaAPI::translate(float x, float y, float z)
{
    if (d_->state == STATE_SHADER)
    {
        core::Error("translate() command cannot be inside shader block. Skipping...");
        return;
    }

    d_->ctm = core::Transform::translate(x, y, z) * d_->ctm;
}

//valid states
//STATE_OPTIONS
//STATE_WORLD
void PaprikaAPI::rotate(float angle, float x, float y, float z)
{
    if (d_->state == STATE_SHADER)
    {
        core::Error("rotate() command cannot be inside shader block. Skipping...");
        return;
    }

    d_->ctm = core::Transform::rotate(angle, x, y, z) * d_->ctm;
}

//valid states
//STATE_OPTIONS
//STATE_WORLD
void PaprikaAPI::scale(float x, float y, float z)
{
    if (d_->state == STATE_SHADER)
    {
        core::Error("scale() command cannot be inside shader block. Skipping...");
        return;
    }

    d_->ctm = core::Transform::scale(x, y, z) * d_->ctm;
}

//valid states
//STATE_OPTIONS
//STATE_WORLD
void PaprikaAPI::lookAt(float ex, float ey, float ez,
                     float lx, float ly, float lz,
                     float ux, float uy, float uz)
{
    if (d_->state == STATE_SHADER)
    {
        core::Error("lookAt() command cannot be inside shader block. Skipping...");
        return;
    }

    d_->ctm = core::Transform::lookAt(ex, ey, ez, lx, ly, lz, ux, uy, uz) * d_->ctm;
}

//valid states
//STATE_WORLD
void PaprikaAPI::mesh(const char* interp, int nfaces, const int* nverts, const int* verts)
{
    if (d_->state != STATE_WORLD)
    {
        core::Error("mesh() command must be inside world block. Skipping...");
        return;
    }

    bool isEmissive = d_->params.find("emissive", OIIO::TypeDesc::INT, 0);

    shape::Mesh *mesh = new shape::Mesh(d_->rtcDevice, interp, nfaces, nverts, verts, d_->params);
    core::Primitive *primitive = new core::Primitive(mesh, d_->ctm, d_->shaderGroup, d_->shaderTransform, isEmissive);
    mesh->unref();

    d_->primitives.push_back(primitive);

    d_->params.reportUnused("mesh");
    d_->params.clear();
}


//valid states
//STATE_WORLD
void PaprikaAPI::sphere(float radius)
{
    if (d_->state != STATE_WORLD)
    {
        core::Error("sphere() command must be inside world block. Skipping...");
        return;
    }

    bool isEmissive = d_->params.find("emissive", OIIO::TypeDesc::INT, 0);

    shape::Sphere *sphere = new shape::Sphere(d_->rtcDevice, radius, d_->params);
    core::Primitive *primitive = new core::Primitive(sphere, d_->ctm, d_->shaderGroup, d_->shaderTransform, isEmissive);
    sphere->unref();

    d_->primitives.push_back(primitive);

    d_->params.reportUnused("sphere");
    d_->params.clear();
}

void PaprikaAPI::background()
{
    if (d_->state != STATE_WORLD)
    {
        core::Error("background() command must be inside world block. Skipping...");
        return;
    }

    d_->backgroundShaderGroup = d_->shaderGroup;

    d_->params.reportUnused("background");
    d_->params.clear();
}

//valid states
//STATE_OPTIONS
void PaprikaAPI::camera(const char *name)
{
    if (d_->camera != NULL)
        d_->camera->unref();

    d_->camera = camera::PerspectiveCamera::create(d_->ctm, d_->params);

    d_->params.reportUnused("camera");
    d_->params.clear();
}

//valid states
//STATE_OPTIONS
void PaprikaAPI::world()
{
    if (d_->state != STATE_OPTIONS)
    {
        core::Error("world() command must be inside options block. Skipping...");
        return;
    }

    d_->ctm = core::Transform();
    d_->state = STATE_WORLD;
}

//valid states
//STATE_WORLD
void PaprikaAPI::render()
{
    if (d_->state != STATE_WORLD)
    {
        core::Error("render() command must be inside world block. Skipping...");
        return;
    }

    core::Scene *scene = new core::Scene(d_->rtcDevice, d_->primitives);
    core::Renderer *renderer = new renderer::PathTracer(scene, d_->camera, d_->backgroundShaderGroup, d_->shadingSystem);
    // core::Renderer *renderer = new renderer::DebugRenderer(scene, d_->camera, d_->backgroundShaderGroup, d_->shadingSystem);
    d_->rendererService.setRenderer(renderer);
    renderer->render();
    d_->rendererService.setRenderer(NULL);
    delete renderer;
    scene->unref();

    //output_.save("hebe");
}

void PaprikaAPI::shaderGroupBegin()
{
    if (d_->state != STATE_WORLD)
    {
        core::Error("shaderGroupBegin() command must be inside world block. Skipping...");
        return;
    }

    d_->shaderGroup = d_->shadingSystem->ShaderGroupBegin();
    d_->state = STATE_SHADER;
}

void PaprikaAPI::shader(const char *shaderusage, const char *shadername, const char *layername)
{
    if (d_->state != STATE_SHADER)
    {
        core::Error("shader() command must be inside shader block. Skipping...");
        return;
    }

    for (core::ParameterMap::iterator iter = d_->params.begin(); iter != d_->params.end(); ++iter)
    {
        const OIIO::ustring& name = iter->first;
        const core::ParamItem &param = iter->second;
        d_->shadingSystem->Parameter(name.c_str(), param.type.type, param.ptr);
        param.lookedup = true;      // TODO: don't lookedup ununsed parameters
    }

    d_->shadingSystem->Shader(shaderusage, shadername, layername);

    d_->shaderTransform = d_->ctm;

    d_->params.reportUnused(layername);
    d_->params.clear();
}

void PaprikaAPI::connectShaders(const char* srclayer, const char* srcparam,
                             const char* dstlayer, const char* dstparam)
{
    if (d_->state != STATE_SHADER)
    {
        core::Error("connectShaders() command must be inside shader block. Skipping...");
        return;
    }
        
    d_->shadingSystem->ConnectShaders(srclayer, srcparam, dstlayer, dstparam);
}

void PaprikaAPI::shaderGroupEnd()
{
    if (d_->state != STATE_SHADER)
    {
        core::Error("shaderGroupEnd() command must be inside shader block. Skipping...");
        return;
    }

    d_->shadingSystem->ShaderGroupEnd();
    d_->state = STATE_WORLD;
}


void PaprikaAPI::input(const char *fileName)
{
    const char *dot = strrchr(fileName, '.');
    
    if (dot == NULL)
    {
        core::Error("Unrecognized generator.");
        return;
    }

    std::string ext = dot + 1;

    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext == "lua")
    {
        generator::LuaGenerator g;
        g.run(this, fileName);
    }
    else if (ext == "trimesh")
    {
        generator::TriMeshGenerator g;
        g.run(this, fileName);
    }
    else
    {
        core::Error("Unrecognized generator.");
        return;
    }
}

}
