#ifndef CORE_SCENE_H
#define CORE_SCENE_H

#include <vector>
#include <core/geometry.hpp>
#include <core/shape.hpp>
#include <core/referenced.hpp>
#include <embree2/rtcore.h>
#include <embree2/rtcore_ray.h>

namespace paprika {
namespace core {

class Primitive;

class Scene : public core::Referenced
{
public:
    Scene(const std::vector<core::Primitive*> &primitives);
    ~Scene();

    core::Primitive *intersect(const core::Ray &ray, core::InterpolationInfo *interp, OSL::ShaderGlobals *sg) const;

    bool isVisible(const core::Ray &ray) const;
    bool isVisible(const core::Vec3 &p1, const core::Vec3 &p2) const;

    const std::vector<core::Primitive*> &primitives() const
    {
        return primitives_;
    }

private:
    std::vector<core::Primitive*> primitives_;
    RTCDevice device_;
    RTCScene scene_;
};

}
}

#endif
