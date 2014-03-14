#ifndef CORE_RENDERER_H
#define CORE_RENDERER_H

#include <OSL/oslexec.h>

namespace paprika {
namespace core {

class Scene;
class Camera;
class Renderer;

class RendererService : public OSL::RendererServices
{
public:
    RendererService()
    {
        renderer_ = NULL;
    }

    void setRenderer(Renderer *renderer)
    {
        renderer_ = renderer;
    }

    virtual bool get_matrix(OSL::Matrix44 &result, OSL::TransformationPtr xform, float time);
    virtual bool get_inverse_matrix(OSL::Matrix44 &result, OSL::TransformationPtr xform, float time);
    virtual bool get_matrix(OSL::Matrix44 &result, OSL::TransformationPtr xform);
    virtual bool get_inverse_matrix(OSL::Matrix44 &result, OSL::TransformationPtr xform);

    virtual bool get_matrix(OSL::Matrix44 &result, OSL::ustring from, float time);
    virtual bool get_inverse_matrix(OSL::Matrix44 &result, OSL::ustring to, float time);
    virtual bool get_matrix(OSL::Matrix44 &result, OSL::ustring from);
    virtual bool get_inverse_matrix(OSL::Matrix44 &result, OSL::ustring to);

    virtual bool get_attribute(void *renderstate, bool derivatives, OSL::ustring object, OSL::TypeDesc type, OSL::ustring name, void *val);
    virtual bool get_array_attribute(void *renderstate, bool derivatives, OSL::ustring object, OSL::TypeDesc type, OSL::ustring name, int index, void *val);
    virtual bool get_userdata(bool derivatives, OSL::ustring name, OSL::TypeDesc type, void *renderstate, void *val);
    virtual bool has_userdata(OSL::ustring name, OSL::TypeDesc type, void *renderstate);

private:
    Renderer *renderer_;
};

class Renderer
{
public:
    Renderer(core::Scene *scene, core::Camera *camera, OSL::ShadingAttribStateRef backgroundShaderState, OSL::ShadingSystem *shadingSystem);
    virtual ~Renderer();

    bool get_matrix(OSL::Matrix44 &result, OSL::ustring from, float time);
    bool get_inverse_matrix(OSL::Matrix44 &result, OSL::ustring to, float time);
    bool get_matrix(OSL::Matrix44 &result, OSL::ustring from);
    bool get_inverse_matrix(OSL::Matrix44 &result, OSL::ustring to);

    bool get_attribute(void *renderstate, bool derivatives, OSL::ustring object, OSL::TypeDesc type, OSL::ustring name, void *val) { return false; }
    bool get_array_attribute(void *renderstate, bool derivatives, OSL::ustring object, OSL::TypeDesc type, OSL::ustring name, int index, void *val) { return false; }
    bool get_userdata(bool derivatives, OSL::ustring name, OSL::TypeDesc type, void *renderstate, void *val);
    bool has_userdata(OSL::ustring name, OSL::TypeDesc type, void *renderstate);

    virtual void render() = 0;

protected:
    core::Scene *scene_;
    core::Camera *camera_;
    OSL::ShadingAttribStateRef backgroundShaderState_;
    OSL::ShadingSystem *shadingSystem_;
};

}
}

#endif