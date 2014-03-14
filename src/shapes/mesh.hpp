#ifndef MESH_HPP
#define MESH_HPP

#include <core/shape.hpp>
#include <core/parametermap.hpp>

namespace paprika {
namespace shape {

class Mesh : public core::Shape
{
public:
    Mesh(const char* interp, int nfaces, const int* nverts, const int* verts, core::ParameterMap &map);
    virtual ~Mesh();

    virtual float area() const;

    virtual void fillHitInfo(const core::Ray &ray, int primID, core::HitInfo *hitInfo) const;

    virtual void fillInterpolationInfo(const core::HitInfo &hitInfo, core::InterpolationInfo *interp) const;

    virtual void sample(float u1, float u2, float u3, int *primID, core::Vec3 *p, core::Vec3 *n) const;

private:
    std::vector<core::Vec3> P_;
    std::vector<std::vector<int> > faces_;
    std::vector<float> pdf_;

    struct Triangle
    {
        int iface;
        int v[3];		// vertex indices
        int l[3];		// linear indices
    };
    std::vector<Triangle> triangles_;

    float area_;
};

}		// shape
}		// paprika
#endif