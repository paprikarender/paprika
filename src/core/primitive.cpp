#include <core/primitive.hpp>

namespace paprika {
namespace core {

Primitive::Primitive(Shape *shape, const core::Transform &objectToWorld, OSL::ShaderGroupRef shaderGroup, const core::Transform &shaderToWorld, bool isEmissive)
{	
    shape_ = shape;
    shape_->ref();
    objectToWorld_ = objectToWorld;
    worldToObject_ = objectToWorld.inverse();
    shaderGroup_ = shaderGroup;
    shaderToWorld_ = shaderToWorld;
    isEmissive_ = isEmissive;
}

Primitive::~Primitive()
{
    shape_->unref();
}

void Primitive::fillIntersectionInfo(const core::Ray &ray, int primID, core::InterpolationInfo *interp, OSL::ShaderGlobals *sg)
{
    core::Ray rayo = worldToObject_.transformRay(ray);

    core::HitInfo hitInfo;
    shape_->fillHitInfo(rayo, primID, &hitInfo);

    shape_->fillInterpolationInfo(hitInfo, interp);

    {
        OSL::Dual2<core::Vec3> P = ray.point(hitInfo.t);
        sg->P = P.val();
        sg->dPdx = P.dx();
        sg->dPdy = P.dy();

        const core::ParamItem *paramItemU = shape_->getParamItemU();
        if (paramItemU != NULL)
        {
            interpolate(*paramItemU, *interp, false, &sg->u);   // TODO: derivatives
        }
        else
        {
            sg->u = hitInfo.u.val();
            sg->dudx = hitInfo.u.dx();
            sg->dudy = hitInfo.u.dy();
        }

        const core::ParamItem *paramItemV = shape_->getParamItemV();
        if (paramItemV != NULL)
        {
            interpolate(*paramItemV, *interp, false, &sg->v);   // TODO: derivatives
        }
        else
        {
            sg->v = hitInfo.v.val();
            sg->dvdx = hitInfo.v.dx();
            sg->dvdy = hitInfo.v.dy();
        }

        sg->dPdu = objectToWorld_.transformVector(hitInfo.dPdu);
        sg->dPdv = objectToWorld_.transformVector(hitInfo.dPdv);

        sg->I = ray.d.val();
        sg->dIdx = ray.d.dx();
        sg->dIdy = ray.d.dy();

        sg->Ng = objectToWorld_.transformNormal(hitInfo.Ng).normalized();

        const core::ParamItem *paramItemN = shape_->getParamItemN();
        if (paramItemN != NULL)
            interpolate(*paramItemN, *interp, false, &sg->N);
        else
            sg->N = sg->Ng;

        if (sg->N.dot(sg->Ng) < 0)
            sg->N = -sg->N;

        sg->backfacing = sg->Ng.dot(sg->I) > 0;
        if (sg->backfacing)
        {
            sg->N = -sg->N;
            sg->Ng = -sg->Ng;
        }

        sg->flipHandedness = false;     // TODO

        sg->surfacearea = shape_->area();

#if 0
        // TODO
        // refine intersection point (Carsten Alexander W�chter's (toxie) thesis, page 29)
        float t = dot(dgeom->Ng, (p0 - dgeom->P)) / dot(dgeom->Ng, ray.d);
        dgeom->P = dgeom->P + ray.d * t;
#endif
    }

    sg->object2common = &objectToWorld_;
    sg->shader2common = &shaderToWorld_;
    sg->renderstate = interp;
}

void Primitive::fillIntersectionInfo(const core::Vec3 &p, const core::Vec3 &n, int primID, core::InterpolationInfo *interp, OSL::ShaderGlobals *sg)
{
    core::Ray ray = core::Ray(p + n, -n);
    fillIntersectionInfo(ray, primID, interp, sg);
}


#if 0
bool Primitive::intersect(const core::Ray& ray, core::InterpolationInfo *interp, core::DifferentialGeometry *dgeom) const
{
    core::Ray tray = worldToObject_.transformRay(ray);

    if (!shape_->intersect(tray, interp, dgeom))
        return false;

    //transform dgeom
    dgeom->P = objectToWorld_.transformPoint(dgeom->P);
    dgeom->Ng = objectToWorld_.transformNormal(dgeom->Ng);	// TODO normalize!

    return true;
}
#endif

void Primitive::interpolate(const core::ParamItem &paramitem, const InterpolationInfo &interp, bool derivatives, void *paramarea) const
{
    shape_->interpolate(paramitem, interp, derivatives, paramarea);

    int arraylen = paramitem.type.type.numelements();

    if (paramitem.type.type == OIIO::TypeDesc(OIIO::TypeDesc::FLOAT, OIIO::TypeDesc::VEC3, OIIO::TypeDesc::POINT))
    {
        core::Vec3 *points = static_cast<core::Vec3*>(paramarea);
        for (int i = 0; i < arraylen; ++i)
            points[i] = objectToWorld_.transformPoint(points[i]);
    }
    else if (paramitem.type.type == OIIO::TypeDesc(OIIO::TypeDesc::FLOAT, OIIO::TypeDesc::VEC3, OIIO::TypeDesc::VECTOR))
    {
        core::Vec3 *vectors = static_cast<core::Vec3*>(paramarea);
        for (int i = 0; i < arraylen; ++i)
            vectors[i] = objectToWorld_.transformVector(vectors[i]);
    }
    else if (paramitem.type.type == OIIO::TypeDesc(OIIO::TypeDesc::FLOAT, OIIO::TypeDesc::VEC3, OIIO::TypeDesc::NORMAL))
    {
        core::Vec3 *normals = static_cast<core::Vec3*>(paramarea);
        for (int i = 0; i < arraylen; ++i)
            normals[i] = objectToWorld_.transformNormal(normals[i]).normalized();
    }
    // TODO: derivatives, hpoint, matrix
}


void Primitive::sample(float u1, float u2, float u3, int *primID, core::Vec3 *p, core::Vec3 *n) const
{
    shape_->sample(u1, u2, u3, primID, p, n);
    *p = objectToWorld_.transformPoint(*p);
    *n = objectToWorld_.transformNormal(*n);
}

float Primitive::pdf(const core::Vec3 &p) const
{
    return shape_->pdf(worldToObject_.transformPoint(p));
}

void Primitive::sample(const core::Vec3 &ps, float u1, float u2, float u3, int *primID, core::Vec3 *p, core::Vec3 *n) const
{
    shape_->sample(worldToObject_.transformPoint(ps), u1, u2, u3, primID, p, n);
    *p = objectToWorld_.transformPoint(*p);
    *n = objectToWorld_.transformNormal(*n);
}

float Primitive::pdf(const core::Vec3 &ps, const core::Vec3 &p, const core::Vec3 &n) const
{
    return shape_->pdf(worldToObject_.transformPoint(ps), worldToObject_.transformPoint(p), worldToObject_.transformNormal(n));
}

}		// core
}		// paprika

