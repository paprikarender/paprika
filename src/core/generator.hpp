#ifndef CORE_GENERATOR_HPP
#define CORE_GENERATOR_HPP

namespace paprika {

class PaprikaAPI;
    
namespace core {

class Generator
{
public:
    virtual ~Generator() { }

    virtual void run(PaprikaAPI *renderer, const char *params) = 0;
};

}
}
#endif
