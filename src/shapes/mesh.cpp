#include "mesh.hpp"
#include <algorithm>
#include <numeric>
#include "triangulate.hpp"

namespace paprika {
namespace shape {

Mesh::Mesh(RTCDevice device, const char* interp, int nfaces, const int* nverts, const int* verts, core::ParameterMap& map)
{
    int nConstant = 1;
    int nPerPiece = nfaces;
    int nLinear = std::accumulate(nverts, nverts + nfaces, 0);
    int nVertex = *std::max_element(verts, verts + nLinear) + 1;

    const float* p = map.find("P", core::ParamType(OIIO::TypeDesc(OIIO::TypeDesc::FLOAT, OIIO::TypeDesc::VEC3, OIIO::TypeDesc::POINT), core::INTERP_VERTEX), (float*)NULL);

    if (p == NULL)
    {
        core::Error("Cannot construct mesh without parameter \"P\"");
        return;
    }

    // transfer points
    P_.resize(nVertex);
    for (int i = 0; i < nVertex; ++i)
    {
        float x = p[i * 3 + 0];
        float y = p[i * 3 + 1];
        float z = p[i * 3 + 2];

        P_[i] = core::Vec3(x, y, z);
    }

    // transfer faces
    const int* v = verts;
    faces_.resize(nfaces);
    for (int i = 0; i < nfaces; ++i)
    {
        faces_[i].resize(nverts[i]);
        std::copy(v, v + nverts[i], faces_[i].begin());
        v += nverts[i];
    }

    transferParameters(map, nConstant, nPerPiece, nLinear, nVertex);

    // triangulate the mesh
    int ivert = 0;
    for (std::size_t i = 0; i < faces_.size(); ++i)
    {
        std::vector<core::Vec3> P(faces_[i].size());
        for (std::size_t j = 0; j < faces_[i].size(); ++j)
            P[j] = P_[faces_[i][j]];

        core::Triangulate triangulate(&P[0], faces_[i].size());	

        for (std::size_t j = 0; j < triangulate.triangles.size() / 3; ++j)
        {
            Triangle t;

            t.iface = (int)i;
            for (int k = 0; k < 3; ++k)
            {
                t.l[k] = triangulate.triangles[j * 3 + k] + ivert;
                t.v[k] = verts[t.l[k]];
            }

            triangles_.push_back(t);
        }

        ivert += faces_[i].size();
    }

    // calculate mesh area and pdf
    pdf_.resize(triangles_.size() + 1);
    area_ = 0.f;
    for (std::size_t i = 0; i < triangles_.size(); ++i)
    {
        const core::Vec3& p0 = P_[triangles_[i].v[0]];
        const core::Vec3& p1 = P_[triangles_[i].v[1]];
        const core::Vec3& p2 = P_[triangles_[i].v[2]];
        pdf_[i] = area_;
        area_ += 0.5f * ((p1 - p0).cross(p2 - p0)).length();
    }
    pdf_[triangles_.size()] = area_;

    for (std::size_t i = 0; i < pdf_.size(); ++i)
        pdf_[i] /= area_;

    scene_ = rtcDeviceNewScene(device, RTC_SCENE_STATIC, RTC_INTERSECT1);

    geomID_ = rtcNewTriangleMesh(scene_, RTC_GEOMETRY_STATIC, triangles_.size(), nVertex, 1);

    float *vertices = (float*)rtcMapBuffer(scene_, geomID_, RTC_VERTEX_BUFFER);
    for (std::size_t i = 0; i < P_.size(); ++i)
    {
        vertices[i * 4] = P_[i].x;
        vertices[i * 4 + 1] = P_[i].y;
        vertices[i * 4 + 2] = P_[i].z;
    }
    rtcUnmapBuffer(scene_, geomID_, RTC_VERTEX_BUFFER);

    int *triangles = (int*)rtcMapBuffer(scene_, geomID_, RTC_INDEX_BUFFER);
    for (std::size_t i = 0; i < triangles_.size(); ++i)
    {
        triangles[i * 3] = triangles_[i].v[0];
        triangles[i * 3 + 1] = triangles_[i].v[1];
        triangles[i * 3 + 2] = triangles_[i].v[2];
    }
    rtcUnmapBuffer(scene_, geomID_, RTC_INDEX_BUFFER);

    rtcCommit(scene_);
}

Mesh::~Mesh()
{
    rtcDeleteScene(scene_);
}

void Mesh::fillHitInfo(const core::Ray &ray, int primID, core::HitInfo *hitInfo) const
{
    hitInfo->primID = primID;

    const Triangle &t = triangles_[primID];

    const core::Vec3 &v0 = P_[t.v[0]];
    const core::Vec3 &v1 = P_[t.v[1]];
    const core::Vec3 &v2 = P_[t.v[2]];
    core::Vec3 e1 = v1 - v0;
    core::Vec3 e2 = v2 - v0;
    OSL::Dual2<core::Vec3> pvec = cross(ray.d, e2);
    OSL::Dual2<float> det = dot(e1, pvec);
    OSL::Dual2<float> invDet = 1.f / det;
    OSL::Dual2<core::Vec3> tvec = ray.o - v0;
    hitInfo->u = dot(tvec, pvec) * invDet;
    OSL::Dual2<core::Vec3> qvec = cross(tvec, e1);
    hitInfo->v = dot(ray.d, qvec) * invDet;
    hitInfo->t = dot(e2, qvec) * invDet;

    hitInfo->Ng = e1.cross(e2).normalized();

    hitInfo->dPdu = e1;
    hitInfo->dPdv = e2;
}

void Mesh::fillInterpolationInfo(const core::HitInfo &hitInfo, core::InterpolationInfo *interp) const
{
    const Triangle &t = triangles_[hitInfo.primID];

    interp->ipiece = t.iface;

    interp->weights.resize(3);
    interp->weights[0] = core::InterpolationInfo::Weight(t.v[0], t.l[0], 1.f - hitInfo.u - hitInfo.v);
    interp->weights[1] = core::InterpolationInfo::Weight(t.v[1], t.l[1], hitInfo.u);
    interp->weights[2] = core::InterpolationInfo::Weight(t.v[2], t.l[2], hitInfo.v);

    interp->shape = this;
}

float Mesh::area() const
{
    return area_;
}

void Mesh::sample(float u1, float u2, float u3, int *primID, core::Vec3 *p, core::Vec3 *n) const
{
    int index = std::lower_bound(pdf_.begin() + 1, pdf_.end() - 1, u3) - pdf_.begin() - 1;

    float su1 = sqrt(u1);
    float u = 1 - su1;
    float v = u2 * su1;

    *primID = index;

    const Triangle &t = triangles_[index];

    const core::Vec3 &v0 = P_[t.v[0]];
    const core::Vec3 &v1 = P_[t.v[1]];
    const core::Vec3 &v2 = P_[t.v[2]];

    *p = (1 - u - v) * v0 + u * v1 + v * v2;

    *n = (v1 - v0).cross(v2 - v0).normalized();
}

}		// shape
}		// paprika
