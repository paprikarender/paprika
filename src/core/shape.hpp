#ifndef CORE_SHAPE_HPP
#define CORE_SHAPE_HPP

#include <string>
#include <vector>
#include <core/paramitem.hpp>
#include <core/referenced.hpp>
#include <core/parametermap.hpp>
#include <core/geometry.hpp>
#include <embree2/rtcore.h>
#include <embree2/rtcore_ray.h>
#include <OSL/oslexec.h>

namespace paprika {
namespace core {

class Shape;

struct InterpolationInfo
{
	int ipiece;

	struct Weight
	{
		Weight() {}
        Weight(int ivertex, int ilinear, const OSL::Dual2<float> &weight) : ivertex(ivertex), ilinear(ilinear), weight(weight) {}

		int ivertex;
		int ilinear;
        OSL::Dual2<float> weight;
	};
	std::vector<Weight> weights; // indices and values of non-zero weights

    const Shape *shape;
};

struct HitInfo
{
    int primID;
    OSL::Dual2<float> t, u, v;
    OSL::Vec3 dPdu, dPdv;
    core::Vec3 Ng;
};

class Shape : public core::Referenced
{
public:
    Shape();
    virtual ~Shape();

    virtual void fillHitInfo(const core::Ray &ray, int primID, core::HitInfo *hitInfo) const = 0;

    virtual void fillInterpolationInfo(const core::HitInfo &hitInfo, core::InterpolationInfo *interp) const = 0;

    const core::ParamItem *getParamItem(const OIIO::ustring &name) const
    {
        std::map<OIIO::ustring, core::ParamItem>::const_iterator iter = parameters_.find(name);
        if (iter == parameters_.end())
            return NULL;
        return &iter->second;
    }

	void interpolate(const core::ParamItem &paramitem, const InterpolationInfo &interp, bool derivatives, void *paramarea) const;

	virtual float area() const = 0;

    // area measure
    virtual void sample(float u1, float u2, float u3, int *primID, core::Vec3 *p, core::Vec3 *n) const = 0;
    virtual float pdf(const core::Vec3 &p) const;

    // solid angle measure
    virtual void sample(const core::Vec3 &ps, float u1, float u2, float u3, int *primID, core::Vec3 *p, core::Vec3 *n) const;
    virtual float pdf(const core::Vec3 &ps, const core::Vec3 &p, const core::Vec3 &n) const;

    RTCScene rtcScene() const
    {
        return scene_;
    }

    const core::ParamItem *getParamItemN() const
    {
        return paramItemN_;
    }

    const core::ParamItem *getParamItemU() const
    {
        return paramItemU_;
    }

    const core::ParamItem *getParamItemV() const
    {
        return paramItemV_;
    }

protected:
    struct ustring_less
    {
        bool operator()(const OIIO::ustring &s1, const OIIO::ustring &s2) const
        {
            return s1.c_str() < s2.c_str();
        }
    };

	void transferParameters(core::ParameterMap &map, int nConstant, int nPerPiece, int nLinear, int nVertex);
    std::map<OIIO::ustring, core::ParamItem, ustring_less> parameters_;

    void *alloc(size_t sz);
    std::vector<void*> freeList_;

    RTCScene scene_;
    unsigned int geomID_;

    const core::ParamItem *paramItemN_, *paramItemU_, *paramItemV_;
};


}       // core
}		// paprika
#endif
