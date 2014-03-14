#ifndef GENERATOR_TRIMESHGENERATOR_HPP
#define GENERATOR_TRIMESHGENERATOR_HPP

#include <core/generator.hpp>

namespace paprika {
namespace generator {

class TriMeshGenerator : public core::Generator
{
public:
    virtual void run(PaprikaAPI *renderer, const char *params);
};

}
}
#endif
