#include <core/shape.hpp>
#include <core/paramtype.hpp>

namespace paprika {
namespace core {

Shape::Shape()
{
    scene_ = NULL;
    geomID_ = RTC_INVALID_GEOMETRY_ID;
}

Shape::~Shape()
{
    for (std::size_t i = 0; i < freeList_.size(); ++i)
        free(freeList_[i]);
}

float Shape::pdf(const core::Vec3 &p) const
{
    return 1.f / area();
}

void Shape::sample(const core::Vec3 &ps, float u1, float u2, float u3, int *primID, core::Vec3 *p, core::Vec3 *n) const
{
    return sample(u1, u2, u3, primID, p, n);
}

float Shape::pdf(const core::Vec3 &ps, const core::Vec3 &p, const core::Vec3 &n) const
{
    float pdfArea = pdf(p);

    core::Vec3 dir = ps - p;

    float dot = fabs(dir.dot(n));

    float length2 = dir.length2();

    dot /= sqrt(length2);

    return pdfArea * length2 / dot;
}

void Shape::transferParameters(core::ParameterMap& map, int nConstant, int nPerPiece, int nLinear, int nVertex)
{
	for (ParameterMap::iterator iter = map.begin(); iter != map.end(); ++iter)
	{
		if (iter->second.lookedup)
			continue;
		iter->second.lookedup = true;

		const OIIO::ustring &name = iter->first;
		const core::ParamItem &paramitem = iter->second;
        const OIIO::TypeDesc &type = paramitem.type.type;

		int nitems = 0;
		switch (paramitem.type.interp)
		{
			case core::INTERP_CONSTANT:
				nitems = type.numelements() * nConstant;
				break;
			case core::INTERP_PERPIECE:
				nitems = type.numelements() * nPerPiece;
				break;
			case core::INTERP_LINEAR:
				nitems = type.numelements() * nLinear;
				break;
			case core::INTERP_VERTEX:
				nitems = type.numelements() * nVertex;
				break;
		}

        if (type.basetype == OIIO::TypeDesc::STRING)
		{
			// do a full copy for strings
			char** strs = (char**)alloc(sizeof(char*) * nitems);
			for (int i = 0; i < nitems; ++i)
			{
				const char* str = paramitem.strings[i];
				strs[i] = (char*)alloc(strlen(str));
				strcpy(strs[i], str);
			}

			parameters_[name] = core::ParamItem(paramitem.type, strs);
		}
		else
		{
			if (paramitem.isOne())
				parameters_[name] = paramitem;
			else
			{
				int datasize = nitems * type.elementsize();
				void* newptr = alloc(datasize);
				memcpy(newptr, paramitem.ptr, datasize);
				parameters_[name] = core::ParamItem(paramitem.type, newptr);
			}
		}
	}

    paramItemN_ = getParamItem(OIIO::ustring("N"));
    if (paramItemN_ != NULL)
    {
        if (paramItemN_->type.type != OIIO::TypeDesc(OIIO::TypeDesc::FLOAT, OIIO::TypeDesc::VEC3, OIIO::TypeDesc::NORMAL))
        {
            core::Warning("The type of parameter \"N\" should be \"normal\". Ignoring.");
            paramItemN_ = NULL;
        }
    }

    paramItemU_ = getParamItem(OIIO::ustring("u"));
    if (paramItemU_ != NULL)
    {
        if (paramItemU_->type.type != OIIO::TypeDesc::FLOAT)
        {
            core::Warning("The type of parameter \"u\" should be \"float\". Ignoring.");
            paramItemU_ = NULL;
        }
    }
        
    paramItemV_ = getParamItem(OIIO::ustring("v"));
    if (paramItemV_ != NULL)
    {
        if (paramItemV_->type.type != OIIO::TypeDesc::FLOAT)
        {
            core::Warning("The type of parameter \"v\" should be \"float\". Ignoring.");
            paramItemV_ = NULL;
        }
    }
}

void Shape::interpolate(const core::ParamItem &paramitem, const InterpolationInfo& interp, bool derivatives, void* paramarea) const
{
    const OIIO::TypeDesc &type = paramitem.type.type;

    int datasize = type.size();

	switch (paramitem.type.interp)
	{
		case core::INTERP_CONSTANT:
			memcpy(paramarea, paramitem.data, datasize);
			break;
		case core::INTERP_PERPIECE:
			memcpy(paramarea, paramitem.data + interp.ipiece * datasize, datasize);
			break;
		case core::INTERP_LINEAR:
		case core::INTERP_VERTEX:
		{
			float* farea = static_cast<float*>(paramarea);
            int nfloats = type.numelements() * type.aggregate;
			
			std::fill(farea, farea + nfloats, 0.f);

			bool isvertex = paramitem.type.interp == core::INTERP_VERTEX;

			for (std::size_t j = 0; j < interp.weights.size(); ++j)
			{
				const InterpolationInfo::Weight& w = interp.weights[j];

				const float* floats = paramitem.floats + (isvertex ? w.ivertex : w.ilinear) * nfloats;

				for (int k = 0; k < nfloats; ++k)
					farea[k] += floats[k] * w.weight.val();
			}

			break;
		}
	}
}

void *Shape::alloc(size_t sz)
{
    void *result = malloc(sz);
    freeList_.push_back(result);
    return result;
}


}
}
