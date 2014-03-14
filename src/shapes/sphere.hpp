#ifndef SHAPES_SPHERE_HPP
#define SHAPES_SPHERE_HPP

#include <core/shape.hpp>

namespace paprika {
namespace shape {
        
class Sphere : public core::Shape
{
public:
    Sphere(float radius, core::ParameterMap &map);
    virtual ~Sphere();

    virtual float area() const;

    virtual void fillHitInfo(const core::Ray &ray, int primID, core::HitInfo *hitInfo) const;

    virtual void fillInterpolationInfo(const core::HitInfo &hitInfo, core::InterpolationInfo *interp) const;

    virtual void sample(float u1, float u2, float u3, int *primID, core::Vec3 *p, core::Vec3 *n) const;

private:
    float radius_;

private:
    static void bounds_s(void *that, size_t item, RTCBounds &bounds_o)
    {
        static_cast<Sphere*>(that)->bounds(item, bounds_o);
    }
    static void intersect_s(void *that, RTCRay &ray, size_t item)
    {
        static_cast<Sphere*>(that)->intersect(ray, item);
    }
    static void occluded_s(void *that, RTCRay &ray, size_t item)
    {
        static_cast<Sphere*>(that)->occluded(ray, item);
    }

    void bounds(size_t item, RTCBounds &bounds_o);
    void intersect(RTCRay &ray, size_t item);
    void occluded(RTCRay &ray, size_t item);
};

}		// shape
}		// paprika
#endif
