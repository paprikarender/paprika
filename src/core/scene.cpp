#include <core/scene.hpp>
#include <core/primitive.hpp>

namespace paprika {
namespace core {

Scene::Scene(const std::vector<core::Primitive*> &primitives)
{
    primitives_ = primitives;
    for (std::size_t i = 0; i < primitives_.size(); ++i)
        primitives_[i]->ref();

	scene_ = rtcNewScene(RTC_SCENE_STATIC, RTC_INTERSECT1);

    for (std::size_t i = 0; i < primitives_.size(); ++i)
    {
        RTCScene scene = primitives_[i]->shape()->rtcScene();
        unsigned int instID = rtcNewInstance(scene_, scene);
        rtcSetTransform(scene_, instID, RTC_MATRIX_COLUMN_MAJOR_ALIGNED16, primitives_[i]->objectToWorld().matrix().getValue());
    }

    rtcCommit(scene_);
}

Scene::~Scene()
{
    for (std::size_t i = 0; i < primitives_.size(); ++i)
        primitives_[i]->unref();
	rtcDeleteScene(scene_);
}


core::Primitive *Scene::intersect(const core::Ray &ray, core::InterpolationInfo *interp, OSL::ShaderGlobals *sg) const
{
	RTCRay ray2;
	ray2.org[0] = ray.o.val().x;
	ray2.org[1] = ray.o.val().y;
	ray2.org[2] = ray.o.val().z;

	ray2.dir[0] = ray.d.val().x;
	ray2.dir[1] = ray.d.val().y;
	ray2.dir[2] = ray.d.val().z;

    ray2.tnear = ray.tnear;
    ray2.tfar = ray.tfar;
	ray2.geomID = RTC_INVALID_GEOMETRY_ID;
	ray2.primID = RTC_INVALID_GEOMETRY_ID;
	ray2.instID = RTC_INVALID_GEOMETRY_ID;
	ray2.mask = 0xFFFFFFFF;
	ray2.time = 0.0f;

	rtcIntersect(scene_, ray2);

	if (ray2.geomID == RTC_INVALID_GEOMETRY_ID)
		return NULL;
 
    core::Primitive *primitive = primitives_[ray2.instID];      // TODO bunu arastir

    primitive->fillIntersectionInfo(ray, ray2.primID, interp, sg);

    return primitive;
}

bool Scene::isVisible(const core::Ray &ray) const
{
    RTCRay ray2;
    ray2.org[0] = ray.o.val().x;
    ray2.org[1] = ray.o.val().y;
    ray2.org[2] = ray.o.val().z;

    ray2.dir[0] = ray.d.val().x;
    ray2.dir[1] = ray.d.val().y;
    ray2.dir[2] = ray.d.val().z;

    ray2.tnear = ray.tnear;
    ray2.tfar = ray.tfar;
    ray2.geomID = RTC_INVALID_GEOMETRY_ID;
    ray2.primID = RTC_INVALID_GEOMETRY_ID;
    ray2.instID = RTC_INVALID_GEOMETRY_ID;
    ray2.mask = 0xFFFFFFFF;
    ray2.time = 0.0f;

    rtcOccluded(scene_, ray2);

    return ray2.geomID == RTC_INVALID_GEOMETRY_ID;
}

bool Scene::isVisible(const core::Vec3 &p1, const core::Vec3 &p2) const
{
    core::Ray ray(p1, p2 - p1, 1e-3f, 1 - 1e-3f);
    return isVisible(ray);
}


#if 0
class CScene : public core::CObject
{
public:
    CScene(const std::vector<Primitive>& primitives) : primitives_(primitives)
    {

    }

    virtual core::Type type() const
    {
        return Scene::type();
    }

    int intersect(const core::Ray& ray, InterpolationInfo* interp, DifferentialGeometry* dgeom) const
    {
        InterpolationInfo newinterp;
        DifferentialGeometry newdgeom;
        int index = -1;
        for (std::size_t i = 0; i < primitives_.size(); ++i)
        {
            if (primitives_[i].intersect(ray, &newinterp, &newdgeom))
            {
                *interp = newinterp;
                *dgeom = newdgeom;
                index = (int)i;
            }
        }
        
        return index;
    }

    bool intersectP(const core::Ray& ray) const
    {
        for (std::size_t i = 0; i < primitives_.size(); ++i)
            if (primitives_[i].intersectP(ray))
                return true;

        return false;
    }

    const Primitive& primitive(int index) const
    {
        return primitives_[index];
    }

    int primitiveCount() const
    {
        return (int)primitives_.size();
    }

    core::BBox bounds() const
    {
        core::BBox result;

        for (int i = 0; i < primitiveCount(); ++i)
            result += primitives_[i].bounds();

        return result;
    }

private:
    std::vector<Primitive> primitives_;
};

#define PTR static_cast<CScene*>(getPtr())
#define CPTR static_cast<const CScene*>(getPtr())

Scene Scene::create(const std::vector<Primitive>& primitives)
{
    return Scene(new CScene(primitives));
}

core::Type Scene::type()
{
    static core::TypeInfo typeinfo(core::Object::type(), "Scene");
    static core::Type type = typeinfo.type();

    return type;
}

int Scene::intersect(const core::Ray& ray, InterpolationInfo* interp, DifferentialGeometry* dgeom) const
{
    return CPTR->intersect(ray, interp, dgeom);
}

bool Scene::intersectP(const core::Ray& ray) const
{
    return CPTR->intersectP(ray);
}

const Primitive& Scene::primitive(int index) const
{
    return CPTR->primitive(index);
}

int Scene::primitiveCount() const
{
    return CPTR->primitiveCount();
}

core::BBox Scene::bounds() const
{
    return CPTR->bounds();
}


#endif
}		// scene
}		// paprika

