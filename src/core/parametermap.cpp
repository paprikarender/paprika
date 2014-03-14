#include "parametermap.hpp"
#include "debug.hpp"

namespace paprika {
namespace core {

void ParameterMap::parameter(const char *name, const ParamType &p, int val)
{
    const OIIO::TypeDesc &t = p.type;
    bool valid = t.basetype == OIIO::TypeDesc::INT && t.aggregate == OIIO::TypeDesc::SCALAR && t.arraylen == 0 && (p.interp == INTERP_CONSTANT || p.interp == INTERP_PERPIECE);
    if (!valid)
        core::Error("Parameter \"%s\" incorrect type (you passed a int) -- Skipping", name);
    else
        map_[key_type(name)] = ParamItem(p, val);
}

void ParameterMap::parameter(const char *name, const ParamType &p, float val)
{
    const OIIO::TypeDesc &t = p.type;
    bool valid = t.basetype == OIIO::TypeDesc::FLOAT && t.aggregate == OIIO::TypeDesc::SCALAR && t.arraylen == 0;
    if (!valid)
        core::Error("Parameter \"%s\" incorrect type (you passed a float) -- Skipping", name);
    else
        map_[key_type(name)] = ParamItem(p, val);
}

void ParameterMap::parameter(const char *name, const ParamType &p, double val)
{
    const OIIO::TypeDesc &t = p.type;
    bool valid = t.basetype == OIIO::TypeDesc::FLOAT && t.aggregate == OIIO::TypeDesc::SCALAR && t.arraylen == 0;
    if (!valid)
        core::Error("Parameter \"%s\" incorrect type (you passed a float) -- Skipping", name);
    else
        map_[key_type(name)] = ParamItem(p, (float)val);
}

void ParameterMap::parameter(const char *name, const ParamType &p, const char *val)
{
    const OIIO::TypeDesc &t = p.type;
    bool valid = t.basetype == OIIO::TypeDesc::STRING && t.aggregate == OIIO::TypeDesc::SCALAR && t.arraylen == 0 && (p.interp == INTERP_CONSTANT || p.interp == INTERP_PERPIECE);
    if (!valid)
        core::Error("Parameter \"%s\" incorrect type (you passed a char*) -- Skipping", name);
    else
        map_[key_type(name)] = ParamItem(p, val);
}

void ParameterMap::parameter(const char *name, const ParamType &p, const int *val)
{
    const OIIO::TypeDesc &t = p.type;
    bool valid = t.basetype == OIIO::TypeDesc::INT && (p.interp == INTERP_CONSTANT || p.interp == INTERP_PERPIECE);
    if (!valid)
        core::Error("Parameter \"%s\" incorrect type (you passed a int*) -- Skipping", name);
    else
        map_[key_type(name)] = ParamItem(p, val);
}

void ParameterMap::parameter(const char *name, const ParamType &p, const float *val)
{
    const OIIO::TypeDesc &t = p.type;
    bool valid = t.basetype == OIIO::TypeDesc::FLOAT;
    if (!valid)
        core::Error("Parameter \"%s\" incorrect type (you passed a float*) -- Skipping", name);
    else
        map_[key_type(name)] = ParamItem(p, val);
}

void ParameterMap::parameter(const char *name, const ParamType &p, const char * const *val)
{
    const OIIO::TypeDesc &t = p.type;
    bool valid = t.basetype == OIIO::TypeDesc::STRING && (p.interp == INTERP_CONSTANT || p.interp == INTERP_PERPIECE);
    if (!valid)
        core::Error("Parameter \"%s\" incorrect type (you passed a char**) -- Skipping", name);
    else
        map_[key_type(name)] = ParamItem(p, val);
}


#define PARAMETER_TYPED_NAME					\
    ParamType t;								\
    std::string name;							\
    if (t.fromstring(typedname, &name))			\
        parameter(name.c_str(), t, val);

void ParameterMap::parameter(const char *typedname, int val)
{
    PARAMETER_TYPED_NAME
}

void ParameterMap::parameter(const char *typedname, float val)
{
    PARAMETER_TYPED_NAME
}

void ParameterMap::parameter(const char *typedname, double val)
{
    PARAMETER_TYPED_NAME
}

void ParameterMap::parameter(const char *typedname, const char *val)
{
    PARAMETER_TYPED_NAME
}

void ParameterMap::parameter(const char *typedname, const int *val)
{
    PARAMETER_TYPED_NAME
}

void ParameterMap::parameter(const char *typedname, const float *val)
{
    PARAMETER_TYPED_NAME
}

void ParameterMap::parameter(const char *typedname, const char * const *val)
{
    PARAMETER_TYPED_NAME
}

int ParameterMap::find(const char *name, const ParamType &p, int def) const
{
    const OIIO::TypeDesc &t = p.type;
    bool valid = t.basetype == OIIO::TypeDesc::INT && t.aggregate == OIIO::TypeDesc::SCALAR && t.arraylen == 0 && (p.interp == INTERP_CONSTANT || p.interp == INTERP_PERPIECE);
    if (!valid)
    {
        core::Error("Parameter \"%s\" incorrect type (you passed a int) -- Skipping", name);
        return def;
    }

    const_iterator iter = find(name);
    if (iter != map_.end() && iter->second.type == p)
    {
        iter->second.lookedup = true;
        return *iter->second.ints;
    }
    
    return def;
}

float ParameterMap::find(const char *name, const ParamType &p, float def) const
{
    const OIIO::TypeDesc &t = p.type;
    bool valid = t.basetype == OIIO::TypeDesc::FLOAT && t.aggregate == OIIO::TypeDesc::SCALAR && t.arraylen == 0;
    if (!valid)
    {
        core::Error("Parameter \"%s\" incorrect type (you passed a float) -- Skipping", name);
        return def;
    }

    const_iterator iter = find(name);
    if (iter != map_.end() && iter->second.type == p)
    {
        iter->second.lookedup = true;
        return *iter->second.floats;
    }
    
    return def;
}

double ParameterMap::find(const char *name, const ParamType &p, double def) const
{
    const OIIO::TypeDesc &t = p.type;
    bool valid = t.basetype == OIIO::TypeDesc::FLOAT && t.aggregate == OIIO::TypeDesc::SCALAR && t.arraylen == 0;
    if (!valid)
    {
        core::Error("Parameter \"%s\" incorrect type (you passed a float) -- Skipping", name);
        return def;
    }

    const_iterator iter = find(name);
    if (iter != map_.end() && iter->second.type == p)
    {
        iter->second.lookedup = true;
        return *iter->second.floats;
    }
    
    return def;
}

const char *ParameterMap::find(const char *name, const ParamType &p, const char *def) const
{
    const OIIO::TypeDesc &t = p.type;
    bool valid = t.basetype == OIIO::TypeDesc::STRING && t.aggregate == OIIO::TypeDesc::SCALAR && t.arraylen == 0 && (p.interp == INTERP_CONSTANT || p.interp == INTERP_PERPIECE);
    if (!valid)
    {
        core::Error("Parameter \"%s\" incorrect type (you passed a char*) -- Skipping", name);
        return def;
    }

    const_iterator iter = find(name);
    if (iter != map_.end() && iter->second.type == p)
    {
        iter->second.lookedup = true;
        return *iter->second.strings;
    }
    
    return def;
}

const int *ParameterMap::find(const char *name, const ParamType &p, const int *def) const
{
    const OIIO::TypeDesc &t = p.type;
    bool valid = t.basetype == OIIO::TypeDesc::INT && (p.interp == INTERP_CONSTANT || p.interp == INTERP_PERPIECE);
    if (!valid)
    {
        core::Error("Parameter \"%s\" incorrect type (you passed a int*) -- Skipping", name);
        return def;
    }

    const_iterator iter = find(name);
    if (iter != map_.end() && iter->second.type == p)
    {
        iter->second.lookedup = true;
        return iter->second.ints;
    }
    
    return def;
}

const float *ParameterMap::find(const char *name, const ParamType &p, const float *def) const
{
    const OIIO::TypeDesc &t = p.type;
    bool valid = t.basetype == OIIO::TypeDesc::FLOAT;
    if (!valid)
    {
        core::Error("Parameter \"%s\" incorrect type (you passed a float*) -- Skipping", name);
        return def;
    }

    const_iterator iter = find(name);
    if (iter != map_.end() && iter->second.type == p)
    {
        iter->second.lookedup = true;
        return iter->second.floats;
    }
    
    return def;
}

const char * const *ParameterMap::find(const char *name, const ParamType &p, const char * const *def) const
{
    const OIIO::TypeDesc &t = p.type;
    bool valid = t.basetype == OIIO::TypeDesc::STRING && (p.interp == INTERP_CONSTANT || p.interp == INTERP_PERPIECE);
    if (!valid)
    {
        core::Error("Parameter \"%s\" incorrect type (you passed a char**) -- Skipping", name);
        return def;
    }

    const_iterator iter = find(name);
    if (iter != map_.end() && iter->second.type == p)
    {
        iter->second.lookedup = true;
        return iter->second.strings;
    }
    
    return def;
}

#define FIND_TYPED_NAME							\
    ParamType t;								\
    std::string name;							\
    if (t.fromstring(typedname, &name))			\
        return find(name.c_str(), t, def);		\
                                                \
    return def;						

int ParameterMap::find(const char *typedname, int def) const
{
    FIND_TYPED_NAME
}

float ParameterMap::find(const char *typedname, float def) const
{
    FIND_TYPED_NAME
}

double ParameterMap::find(const char *typedname, double def) const
{
    FIND_TYPED_NAME
}

const char *ParameterMap::find(const char *typedname, const char *def) const
{
    FIND_TYPED_NAME
}

const int *ParameterMap::find(const char *typedname, const int *def) const
{
    FIND_TYPED_NAME
}

const float *ParameterMap::find(const char *typedname, const float *def) const
{
    FIND_TYPED_NAME
}

const char * const *ParameterMap::find(const char *typedname, const char * const *def) const
{
    FIND_TYPED_NAME
}

void ParameterMap::reportUnused(const char* message) const
{
    for (const_iterator iter = begin(); iter != end(); ++iter)
        if (iter->second.lookedup == false)
            core::Warning("Parameter \"%s\" not used in \"%s\"", iter->first.c_str(), message);
}

}		// param
}		// paprika
