#include <core/renderer.hpp>
#include <core/scene.hpp>
#include <core/camera.hpp>
#include <OpenImageIO/imageio.h>
#include <core/primitive.hpp>
#include <core/shape.hpp>
#include <OSL/shading.h>
#include <core/projectivecamera.hpp>

namespace paprika {
namespace core {

static OIIO::ustring u_camera("camera");
static OIIO::ustring u_screen("screen");
static OIIO::ustring u_NDC("NDC");
static OIIO::ustring u_raster("raster");

bool RendererService::get_matrix(OSL::ShaderGlobals *sg, OSL::Matrix44 &result, OSL::TransformationPtr xform, float time)
{
    return get_matrix(sg, result, xform);
}

bool RendererService::get_inverse_matrix(OSL::ShaderGlobals *sg, OSL::Matrix44 &result, OSL::TransformationPtr xform, float time)
{
    return get_inverse_matrix(sg, result, xform);
}

bool RendererService::get_matrix(OSL::ShaderGlobals *sg, OSL::Matrix44 &result, OSL::TransformationPtr xform)
{
    const core::Transform *transform = static_cast<const core::Transform*>(xform);
    result = transform->matrix();
    return true;
}

bool RendererService::get_inverse_matrix(OSL::ShaderGlobals *sg, OSL::Matrix44 &result, OSL::TransformationPtr xform)
{
    const core::Transform *transform = static_cast<const core::Transform*>(xform);
    result = transform->inverseMatrix();
    return true;
}

bool RendererService::get_matrix(OSL::ShaderGlobals *sg, OSL::Matrix44 &result, OSL::ustring from, float time)
{
    return renderer_->get_matrix(result, from, time);
}

bool RendererService::get_inverse_matrix(OSL::ShaderGlobals *sg, OSL::Matrix44 &result, OSL::ustring to, float time)
{
    return renderer_->get_inverse_matrix(result, to, time);
}

bool RendererService::get_matrix(OSL::ShaderGlobals *sg, OSL::Matrix44 &result, OSL::ustring from)
{
    return renderer_->get_matrix(result, from);
}

bool RendererService::get_inverse_matrix(OSL::ShaderGlobals *sg, OSL::Matrix44 &result, OSL::ustring to)
{
    return renderer_->get_inverse_matrix(result, to);
}

bool RendererService::get_attribute(OSL::ShaderGlobals *sg, bool derivatives, OSL::ustring object, OSL::TypeDesc type, OSL::ustring name, void *val)
{
    return renderer_->get_attribute(sg, derivatives, object, type, name, val);
}

bool RendererService::get_array_attribute(OSL::ShaderGlobals *sg, bool derivatives, OSL::ustring object, OSL::TypeDesc type, OSL::ustring name, int index, void *val)
{
    return renderer_->get_array_attribute(sg, derivatives, object, type, name, index, val);
}

bool RendererService::get_userdata(bool derivatives, OSL::ustring name, OSL::TypeDesc type, OSL::ShaderGlobals *sg, void *val)
{
    return renderer_->get_userdata(derivatives, name, type, sg, val);
}

Renderer::Renderer(core::Scene *scene, core::Camera *camera, OSL::ShaderGroupRef backgroundShaderGroup, OSL::ShadingSystem *shadingSystem)
{
    scene_ = scene;
    scene_->ref();

    camera_ = camera;
    camera_->ref();

    backgroundShaderGroup_ = backgroundShaderGroup;
    
    shadingSystem_ = shadingSystem;
}

Renderer::~Renderer()
{
    scene_->unref();
    camera_->unref();
}

bool Renderer::get_matrix(OSL::Matrix44 &result, OSL::ustring from, float time)
{
    return get_matrix(result, from);
}

bool Renderer::get_inverse_matrix(OSL::Matrix44 &result, OSL::ustring to, float time)
{
    return get_inverse_matrix(result, to);
}

bool Renderer::get_matrix(OSL::Matrix44 &result, OSL::ustring from)
{
    if (from == u_camera)
    {
        result = camera_->cameraToWorld().matrix();
        return true;
    }
    else if (from == u_screen)
    {
        if (camera_->isProjective())
        {
            core::ProjectiveCamera *projective = static_cast<core::ProjectiveCamera*>(camera_);
            core::Transform worldToScreen = projective->worldToCamera() * projective->cameraToScreen();
            result = worldToScreen.inverseMatrix();
            return true;
        }
        else
            return false;
    }
    else if (from == u_NDC && camera_->isProjective())
    {
        if (camera_->isProjective())
        {
            core::ProjectiveCamera *projective = static_cast<core::ProjectiveCamera*>(camera_);
            core::Transform NDCToWorld = projective->NDCToCamera() * projective->cameraToWorld();
            result = NDCToWorld.matrix();
            return true;
        }
        else
            return false;
    }
    else if (from == u_raster)
    {
        if (camera_->isProjective())
        {
            core::ProjectiveCamera *projective = static_cast<core::ProjectiveCamera*>(camera_);
            core::Transform rasterToWorld = projective->rasterToCamera() * projective->cameraToWorld();
            result = rasterToWorld.matrix();
            return true;
        }
        else
            return false;
    }

    //TODO named transformations

    return false;
}

bool Renderer::get_inverse_matrix(OSL::Matrix44 &result, OSL::ustring to)
{
    if (to == u_camera)
    {
        result = camera_->worldToCamera().matrix();
        return true;
    }
    else if (to == u_screen)
    {
        if (camera_->isProjective())
        {
            core::ProjectiveCamera *projective = static_cast<core::ProjectiveCamera*>(camera_);
            core::Transform worldToScreen = projective->worldToCamera() * projective->cameraToScreen();
            result = worldToScreen.matrix();
            return true;
        }
        else
            return false;
    }
    else if (to == u_NDC)
    {
        if (camera_->isProjective())
        {
            core::ProjectiveCamera *projective = static_cast<core::ProjectiveCamera*>(camera_);
            core::Transform NDCToWorld = projective->NDCToCamera() * projective->cameraToWorld();
            result = NDCToWorld.inverseMatrix();
            return true;
        }
        else
            return false;
    }
    else if (to == u_raster)
    {
        if (camera_->isProjective())
        {
            core::ProjectiveCamera *projective = static_cast<core::ProjectiveCamera*>(camera_);
            core::Transform rasterToWorld = projective->rasterToCamera() * projective->cameraToWorld();
            result = rasterToWorld.inverseMatrix();
            return true;
        }
        else
            return false;
    }

    return false;
}

bool Renderer::get_userdata(bool derivatives, OSL::ustring name, OSL::TypeDesc type, OSL::ShaderGlobals *sg, void *val)
{
    /*
    core::InterpolationInfo* interp = static_cast<core::InterpolationInfo*>(renderstate);

    const core::Shape *shape = interp->shape;

    const core::ParamItem *paramItem = shape->getParamItem(name);

    if (paramItem == NULL)
        return false;

    if (paramItem->type.type != type)
        return false;

    shape->interpolate(*paramItem, *interp, derivatives, val);

    return true;
    */
    return false;
}

}
}
