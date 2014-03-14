#ifndef GENERATOR_LUAGENERATOR_HPP
#define GENERATOR_LUAGENERATOR_HPP

#include <core/generator.hpp>

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <vector>

namespace paprika {
namespace generator {

class LuaGenerator : public core::Generator
{
public:
    virtual void run(PaprikaAPI *renderer, const char *params);

private:
    static LuaGenerator *self(lua_State *L);

    void *alloc(size_t size);
    void clear();
    char *tokenize(const char *str);
    int *tokenize(const std::vector<int> &ints);
    float *tokenize(const std::vector<float> &floats);
    char **tokenize(const std::vector<const char*> &strs);

    static int parameter_s(lua_State *L);
    static int world_s(lua_State *L);
    static int render_s(lua_State *L);
    static int camera_s(lua_State *L);
    static int mesh_s(lua_State *L);
    static int sphere_s(lua_State *L);
    static int background_s(lua_State *L);
    static int input_s(lua_State *L);
    static int pushTransform_s(lua_State *L);
    static int popTransform_s(lua_State *L);
    static int setTransform_s(lua_State *L);
    static int appendTransform_s(lua_State *L);
    static int translate_s(lua_State *L);
    static int rotate_s(lua_State *L);
    static int scale_s(lua_State *L);
    static int lookAt_s(lua_State *L);
    static int shaderGroupBegin_s(lua_State *L);
    static int shader_s(lua_State *L);
    static int connectShaders_s(lua_State *L);
    static int shaderGroupEnd_s(lua_State *L);

    int parameter(lua_State *L, int index);
    int parameter(lua_State *L);
    int world(lua_State *L);
    int render(lua_State *L);
    int camera(lua_State *L);
    int mesh(lua_State *L);
    int sphere(lua_State *L);
    int background(lua_State *L);
    int input(lua_State *L);
    int pushTransform(lua_State *L);
    int popTransform(lua_State *L);
    int setTransform(lua_State *L);
    int appendTransform(lua_State *L);
    int translate(lua_State *L);
    int rotate(lua_State *L);
    int scale(lua_State *L);
    int lookAt(lua_State *L);
    int shaderGroupBegin(lua_State *L);
    int shader(lua_State *L);
    int connectShaders(lua_State *L);
    int shaderGroupEnd(lua_State *L);

    std::vector<void*> freeList_;

    PaprikaAPI *api_;
};

}
}
#endif
