#include <shapes/sphere.hpp>

namespace paprika {
namespace shape {

Sphere::Sphere(float radius, core::ParameterMap &map) : radius_(radius)
{
    // TODO: transfer parameters but not vertex

    scene_ = rtcNewScene(RTC_SCENE_STATIC, RTC_INTERSECT1);
    geomID_ = rtcNewUserGeometry(scene_, 1);

    rtcSetUserData(scene_, geomID_, this);
    rtcSetBoundsFunction(scene_, geomID_, bounds_s);
    rtcSetIntersectFunction(scene_, geomID_, intersect_s);
    rtcSetOccludedFunction(scene_, geomID_, occluded_s);

    rtcCommit(scene_);
}

Sphere::~Sphere()
{
    rtcDeleteScene(scene_);
}

void Sphere::bounds(size_t item, RTCBounds &bounds_o)
{
    bounds_o.lower_x = -radius_;
    bounds_o.lower_y = -radius_;
    bounds_o.lower_z = -radius_;
    bounds_o.upper_x = radius_;
    bounds_o.upper_y = radius_;
    bounds_o.upper_z = radius_;
}

static bool intersectHelper(const RTCRay &ray, float radius, float *tnear, float *tfar)
{
    core::Vec3 o(ray.org[0], ray.org[1], ray.org[2]);
    core::Vec3 d(ray.dir[0], ray.dir[1], ray.dir[2]);
    float A = d.dot(d);
    float B = 2 * d.dot(o);
    float C = o.dot(o) - radius * radius;
    float det = (B * B) - (4.f * A * C);
    if (det < 0)
        return false;
    det = sqrt(det);
    *tnear = (-B - det) / (2.f * A);
    *tfar = (-B + det) / (2.f * A);
    return true;
}

void Sphere::intersect(RTCRay &ray, size_t item)
{
    float tnear, tfar;
    if (intersectHelper(ray, radius_, &tnear, &tfar) == false)
        return;

    if (!(tnear <= ray.tfar && tfar >= ray.tnear))
        return;

    if (tnear < ray.tnear && tfar > ray.tfar)
        return;

    float t = tnear < ray.tnear ? tfar : tnear;

    ray.tfar = t;
    ray.geomID = geomID_;
    ray.primID = item;

#if 0
    // we're not using embree's u, v and Ng => no need to calculate
    core::Vec3 P = o + d * t;

    float theta = atan2(P.y, P.x);
    if (theta < 0)
        theta += 2 * F_PI;
    ray.u = theta * (0.5f * F_INV_PI);

    float phi = asin(std::min(std::max(P.z / radius_, -1.f), 1.f));
    ray.v = (phi + (F_PI * 0.5f)) * F_INV_PI;

    core::Vec3 Ng = P.normalized();
    ray.Ng[0] = Ng.x;
    ray.Ng[1] = Ng.y;
    ray.Ng[2] = Ng.z;
#endif
}

void Sphere::occluded(RTCRay &ray, size_t item)
{
    float tnear, tfar;
    if (intersectHelper(ray, radius_, &tnear, &tfar) == false)
        return;

    if (!(tnear <= ray.tfar && tfar >= ray.tnear))
        return;

    if (tnear < ray.tnear && tfar > ray.tfar)
        return;

    ray.geomID = 0;
}

float Sphere::area() const
{
    return 4 * F_PI * radius_ * radius_;
}

void Sphere::fillHitInfo(const core::Ray &ray, int primID, core::HitInfo *hitInfo) const
{
    hitInfo->primID = primID;

    OSL::Dual2<float> A = OSL::dot(ray.d, ray.d);
    OSL::Dual2<float> B = 2.f * OSL::dot(ray.d, ray.o);
    OSL::Dual2<float> C = OSL::dot(ray.o, ray.o) - radius_ * radius_;
    OSL::Dual2<float> det = (B * B) - (4.f * A * C);
    if (det.val() < 0)
        return;
    det = sqrt(det);
    OSL::Dual2<float> tnear = (-B - det) / (2.f * A);
    OSL::Dual2<float> tfar = (-B + det) / (2.f * A);

    hitInfo->t = tnear.val() < ray.tnear ? tfar : tnear;

    OSL::Dual2<core::Vec3> P = ray.point(hitInfo->t);

    OSL::Dual2<float> Px(P.val().x, P.dx().x, P.dy().x);
    OSL::Dual2<float> Py(P.val().y, P.dx().y, P.dy().y);
    OSL::Dual2<float> Pz(P.val().z, P.dx().z, P.dy().z);

    OSL::Dual2<float> theta = OSL::atan2(Py, Px);
    if (theta.val() < 0)
        theta += 2 * F_PI;
    hitInfo->u = theta * (0.5f * F_INV_PI);

    OSL::Dual2<float> phi = OSL::asin(Pz / radius_);
    hitInfo->v = (phi + (F_PI * 0.5f)) * F_INV_PI;

    hitInfo->Ng = P.val().normalized();

    float x = P.val().x;
    float y = P.val().y;
    float z = P.val().z;
    float cosTheta = cos(theta.val());
    float sinTheta = sin(theta.val());
    float cosPhi = cos(phi.val());
    hitInfo->dPdu = core::Vec3(-y, x, 0) * (2 * F_PI);
    hitInfo->dPdv = core::Vec3(-z * cosTheta, -z * sinTheta, radius_ * cosPhi) * F_PI;
}

void Sphere::fillInterpolationInfo(const core::HitInfo &hitInfo, core::InterpolationInfo *interp) const
{
    interp->shape = this;
}

void Sphere::sample(float u1, float u2, float u3, int *primID, core::Vec3 *p, core::Vec3 *n) const
{
    float z = 1 - 2 * u1;
    float r = sqrt(std::max(0.f, 1 - z * z));
    float phi = 2 * F_PI * u2;
    float x = r * cos(phi);
    float y = r * sin(phi);

    *primID = 0;
    *p = core::Vec3(x, y, z) * radius_;
    *n = core::Vec3(x, y, z);
}

}		// shape
}		// paprika
