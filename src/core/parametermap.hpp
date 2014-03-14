#ifndef CPARAMETERMAP_HPP
#define CPARAMETERMAP_HPP

#include <core/paramitem.hpp>
#include <OpenImageIO/ustring.h>
#include <core/referenced.hpp>
#include <map>
#include <string>

namespace paprika {
namespace core {

class ParameterMap : public core::Referenced
{
public:
    typedef OIIO::ustring key_type;
    typedef ParamItem value_type;
    typedef std::map<key_type, value_type> map_type;
    typedef map_type::iterator iterator;
    typedef map_type::const_iterator const_iterator;

    void parameter(const char *name, const ParamType &p, int val);
    void parameter(const char *name, const ParamType &p, float val);
    void parameter(const char *name, const ParamType &p, double val);
    void parameter(const char *name, const ParamType &p, const char *val);
    void parameter(const char *name, const ParamType &p, const int *val);
    void parameter(const char *name, const ParamType &p, const float *val);
    void parameter(const char *name, const ParamType &p, const char * const *val);

    void parameter(const char *typedname, int val);
    void parameter(const char *typedname, float val);
    void parameter(const char *typedname, double val);
    void parameter(const char *typedname, const char *val);
    void parameter(const char *typedname, const int *val);
    void parameter(const char *typedname, const float *val);
    void parameter(const char *typedname, const char * const *val);

    const_iterator begin() const					{ return map_.begin(); }
    const_iterator find(const char* key) const		{ return map_.find(key_type(key)); }
    const_iterator end() const						{ return map_.end(); }

    iterator begin()								{ return map_.begin(); }
    iterator end()									{ return map_.end(); }
    iterator find(const char* key)					{ return map_.find(key_type(key)); }

    void clear()									{ map_.clear(); }

    int					find(const char *name, const ParamType &p, int def) const;
    float				find(const char *name, const ParamType &p, float def) const;
    double				find(const char *name, const ParamType &p, double def) const;
    const char         *find(const char *name, const ParamType &p, const char *def) const;
    const int          *find(const char *name, const ParamType &p, const int *def) const;
    const float        *find(const char *name, const ParamType &p, const float *def) const;
    const char * const *find(const char *name, const ParamType &p, const char * const *def) const;

    int					find(const char *typedname, int def) const;
    float				find(const char *typedname, float def) const;
    double				find(const char *typedname, double def) const;
    const char         *find(const char *typedname, const char *def) const;
    const int          *find(const char *typedname, const int *def) const;
    const float        *find(const char *typedname, const float *def) const;
    const char * const *find(const char *typedname, const char * const *def) const;

    void reportUnused(const char* message) const;

private:
    map_type map_;
};

}		// param
}		// paprika
#endif	// CPARAMETERMAP_HPP
