#ifndef CORE_PARAMTYPE_HPP
#define CORE_PARAMTYPE_HPP

#include <string>
#include <OpenImageIO/typedesc.h>

namespace paprika {
namespace core {

enum ParamInterp
{
    INTERP_CONSTANT = 0,       // Constant for all pieces/faces
    INTERP_PERPIECE = 1,       // Piecewise constant per piece/face
    INTERP_LINEAR = 2,         // Linearly interpolated across each piece/face
    INTERP_VERTEX = 3          // Interpolated like vertices
};

class ParamType
{
public:
    ParamType() : interp(INTERP_CONSTANT) { }

    ParamType(OIIO::TypeDesc::BASETYPE type, ParamInterp interp = INTERP_CONSTANT) : type(type), interp(interp)
    {
    }

    ParamType(const OIIO::TypeDesc &type, ParamInterp interp = INTERP_CONSTANT) : type(type), interp(interp)
    {
    }

    ParamType(const char *typestring)
    {
        fromstring(typestring);
    }

    bool fromstring(const char *typestring, std::string *shortname = NULL);

    friend bool operator == (const ParamType &p1, const ParamType &p2)
    {
        return p1.type == p2.type && p1.interp == p2.interp;
    }

    OIIO::TypeDesc type;
    ParamInterp interp;
};


}       // core
}       // paprika
#endif	// PARAMTYPE_HPP
