#include <core/paramtype.hpp>
#include<boost/algorithm/string/split.hpp>                                      
#include<boost/algorithm/string.hpp>     
#include <vector>

namespace paprika {
namespace core {

bool ParamType::fromstring(const char *typestring, std::string *shortname)
{
    std::string s = typestring;
    std::vector<std::string> v;
    boost::trim(s);
    boost::algorithm::split(v, s, boost::is_space(), boost::token_compress_on);

    interp = INTERP_CONSTANT;
    if (v.size() == 3)
    {
        if (v[0] == "constant")
            interp = INTERP_CONSTANT;
        else if (v[0] == "perpiece")
            interp = INTERP_PERPIECE;
        else if (v[0] == "linear")
            interp = INTERP_LINEAR;
        else if (v[0] == "vertex")
            interp = INTERP_VERTEX;
        else
            return false;

        v.erase(v.begin());
    }

    if (v.size() != 2)
        return false;

    type = OIIO::TypeDesc(v[0].c_str());

    if (type.basetype == OIIO::TypeDesc::UNKNOWN)
        return false;

    if (shortname)
        *shortname = v[1];

    return true;
}

}
}
