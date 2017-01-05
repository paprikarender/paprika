#ifndef IPRIMITIVE_HPP
#define IPRIMITIVE_HPP

#include <core/referenced.hpp>
#include <core/shape.hpp>
#include <OSL/oslexec.h>

namespace paprika {
namespace core {

class MemoryPool;

class Primitive : public core::Referenced
{
public:
    Primitive(Shape* shape, const core::Transform &objectToWorld, OSL::ShaderGroupRef shaderGroup, const core::Transform &shaderToWorld, bool isEmissive);
    ~Primitive();

    Shape *shape() const
    {
        return shape_;
    }

    const core::Transform &objectToWorld() const
    {
        return objectToWorld_;
    }

    const core::Transform &worldToObject() const
    {
        return worldToObject_;
    }

    void interpolate(const core::ParamItem &paramitem, const InterpolationInfo &interp, bool derivatives, void *paramarea) const;

    OSL::ShaderGroupRef shaderGroup() const
    {
        return shaderGroup_;
    }
    
    const core::Transform &shaderToWorld() const
    {
        return shaderToWorld_;
    }

    void fillIntersectionInfo(const core::Ray &ray, int primID, core::InterpolationInfo *interp, OSL::ShaderGlobals *sg);

    void fillIntersectionInfo(const core::Vec3 &p, const core::Vec3 &n, int primID, core::InterpolationInfo *interp, OSL::ShaderGlobals *sg);

    bool isEmissive() const
    {
        return isEmissive_;
    }


    void sample(float u1, float u2, float u3, int *primID, core::Vec3 *p, core::Vec3 *n) const;
    float pdf(const core::Vec3 &p) const;

    void sample(const core::Vec3 &ps, float u1, float u2, float u3, int *primID, core::Vec3 *p, core::Vec3 *n) const;
    float pdf(const core::Vec3 &ps, const core::Vec3 &p, const core::Vec3 &n) const;


#if 0
    void sample(float u1, float u2, float u3, InterpolationInfo* interp, DifferentialGeometry* dgeom) const
    {
        shape_.sample(u1, u2, u3, interp, dgeom);

        //transform dgeom
        dgeom->P = objectToWorld_(dgeom->P);
        dgeom->Ng = normalize(objectToWorld_(dgeom->Ng));
        dgeom->dPdu = objectToWorld_(dgeom->dPdu);
        dgeom->dPdv = objectToWorld_(dgeom->dPdv);
    }
#endif

private:
    core::Shape *shape_;
    core::Transform objectToWorld_;
    core::Transform worldToObject_;
    OSL::ShaderGroupRef shaderGroup_;
    core::Transform shaderToWorld_;
    bool isEmissive_;
};


}		// scene
}		// paprika
#endif
