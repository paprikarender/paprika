#include <generators/luagenerator.hpp>
#include <core/paramtype.hpp>
#include <api/paprikaapi.hpp>
#include <core/debug.hpp>
#include <string.h>

namespace paprika {
namespace generator {

static char self_key = ' ';

void LuaGenerator::run(PaprikaAPI *renderer, const char *params)
{
    api_ = renderer;

    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    lua_pushlightuserdata(L, &self_key);
    lua_pushlightuserdata(L, this);
    lua_settable(L, LUA_REGISTRYINDEX);

    lua_register(L, "parameter", parameter_s);
    lua_register(L, "world", world_s);
    lua_register(L, "render", render_s);
    lua_register(L, "camera", camera_s);
    lua_register(L, "mesh", mesh_s);
    lua_register(L, "background", background_s);
    lua_register(L, "input", input_s);
    lua_register(L, "pushTransform", pushTransform_s);
    lua_register(L, "popTransform", popTransform_s);
    lua_register(L, "setTransform", setTransform_s);
    lua_register(L, "appendTransform", appendTransform_s);
    lua_register(L, "translate", translate_s);
    lua_register(L, "rotate", rotate_s);
    lua_register(L, "scale", scale_s);
    lua_register(L, "lookAt", lookAt_s);
    lua_register(L, "shaderGroupBegin", shaderGroupBegin_s);
    lua_register(L, "shader", shader_s);
    lua_register(L, "connectShaders", connectShaders_s);
    lua_register(L, "shaderGroupEnd", shaderGroupEnd_s);

    if (luaL_loadfile(L, params) || lua_pcall(L, 0, 0, 0))
        core::Error("%s", lua_tostring(L, -1));

    lua_close(L);

    clear();
}

int LuaGenerator::parameter_s(lua_State *L)	            { return self(L)->parameter(L); }
int LuaGenerator::world_s(lua_State *L)                 { return self(L)->world(L); }
int LuaGenerator::render_s(lua_State *L)                { return self(L)->render(L); }
int LuaGenerator::camera_s(lua_State *L)                { return self(L)->camera(L); }
int LuaGenerator::mesh_s(lua_State *L)                  { return self(L)->mesh(L); }
int LuaGenerator::background_s(lua_State *L)            { return self(L)->background(L); }
int LuaGenerator::input_s(lua_State *L)                 { return self(L)->input(L); }
int LuaGenerator::pushTransform_s(lua_State *L)         { return self(L)->pushTransform(L); }
int LuaGenerator::popTransform_s(lua_State *L)          { return self(L)->popTransform(L); }
int LuaGenerator::setTransform_s(lua_State *L)          { return self(L)->setTransform(L); }
int LuaGenerator::appendTransform_s(lua_State *L)       { return self(L)->appendTransform(L); }
int LuaGenerator::translate_s(lua_State *L)             { return self(L)->translate(L); }
int LuaGenerator::rotate_s(lua_State *L)                { return self(L)->rotate(L); }
int LuaGenerator::scale_s(lua_State *L)                 { return self(L)->scale(L); }
int LuaGenerator::lookAt_s(lua_State *L)                { return self(L)->lookAt(L); }
int LuaGenerator::shaderGroupBegin_s(lua_State *L)      { return self(L)->shaderGroupBegin(L); }
int LuaGenerator::shader_s(lua_State *L)                { return self(L)->shader(L); }
int LuaGenerator::connectShaders_s(lua_State *L)        { return self(L)->connectShaders(L); }
int LuaGenerator::shaderGroupEnd_s(lua_State *L)        { return self(L)->shaderGroupEnd(L); }

void LuaGenerator::clear()
{
    for (std::size_t i = 0; i < freeList_.size(); ++i)
        free(freeList_[i]);
    freeList_.clear();
}

void *LuaGenerator::alloc(size_t size)
{
    void *ptr = malloc(size);
    freeList_.push_back(ptr);
    return ptr;
}

char *LuaGenerator::tokenize(const char *str)
{
    char* newstr = (char*)alloc(strlen(str) + 1);
    strcpy(newstr, str);
    return newstr;
}

int *LuaGenerator::tokenize(const std::vector<int> &ints)
{
    int* newints = (int*)alloc(sizeof(int) * ints.size());
    memcpy(newints, &ints[0], sizeof(int) * ints.size());
    return newints;
}

float *LuaGenerator::tokenize(const std::vector<float> &floats)
{
    float* newfloats = (float*)alloc(sizeof(float) * floats.size());
    memcpy(newfloats, &floats[0], sizeof(float) * floats.size());
    return newfloats;
}

char **LuaGenerator::tokenize(const std::vector<const char*> &strs)
{
    char **newstrs = (char**)alloc(sizeof(char*) * strs.size());
    for (std::size_t i = 0; i < strs.size(); ++i)
        newstrs[i] = tokenize(strs[i]);
    return newstrs;
}

LuaGenerator *LuaGenerator::self(lua_State *L)
{
    lua_pushlightuserdata(L, &self_key);
    lua_gettable(L, LUA_REGISTRYINDEX);
    LuaGenerator* self = (LuaGenerator*)lua_touserdata(L, -1);
    lua_pop(L, 1);
    return self;
}

int LuaGenerator::parameter(lua_State *L, int index)
{
    const char *typedname = luaL_checkstring(L, index);

    core::ParamType type;
    if (type.fromstring(typedname, NULL) == false)
        return luaL_error(L, "TypeError: Cannot parse parameter type.");

    bool istable = lua_istable(L, index + 1);

    int totalElements = type.type.numelements() * type.type.aggregate;

    if (totalElements > 1 && !istable)
        return luaL_error(L, "TypeError: Cannot pass single value to an array/vector/matrix type.");

    switch (type.type.basetype)
    {
    case OIIO::TypeDesc::FLOAT:
    {
        if (istable)
        {
            int n = luaL_getn(L, index + 1);
            std::vector<float> floats(n);
            for (int i = 0; i < n; ++i)
            {
                lua_rawgeti(L, index + 1, i + 1);
                floats[i] = luaL_checknumber(L, -1);
                lua_pop(L, 1);
            }
            api_->parameter(typedname, tokenize(floats));
        }
        else
        {
            api_->parameter(typedname, luaL_checknumber(L, index + 1));
        }
        break;
    }
    case OIIO::TypeDesc::INT:
    {
        if (istable)
        {
            int n = luaL_getn(L, index + 1);
            std::vector<int> ints(n);
            for (int i = 0; i < n; ++i)
            {
                lua_rawgeti(L, index + 1, i + 1);
                ints[i] = luaL_checkint(L, -1);
                lua_pop(L, 1);
            }
            api_->parameter(typedname, tokenize(ints));
        }
        else
        {
            api_->parameter(typedname, luaL_checkint(L, index + 1));
        }
        break;
    }
    case OIIO::TypeDesc::STRING:
    {
        if (istable)
        {
            int n = luaL_getn(L, index + 1);
            std::vector<const char*> strs(n);
            for (int i = 0; i < n; ++i)
            {
                lua_rawgeti(L, index + 1, i + 1);
                strs[i] = luaL_checkstring(L, -1);
                lua_pop(L, 1);
            }
            api_->parameter(typedname, tokenize(strs));
        }
        else
        {
            api_->parameter(typedname, tokenize(luaL_checkstring(L, index + 1)));
        }
        break;
    }
    default:
        return luaL_error(L, "TypeError: Unsupported base type.");
    }

    return 0;
}


int LuaGenerator::parameter(lua_State *L)
{
    return parameter(L, 1);
}

int LuaGenerator::world(lua_State *L)
{
    api_->world();
    return 0;
}

int LuaGenerator::mesh(lua_State *L)
{
    const char* interp = luaL_checkstring(L, 1);

    luaL_checktype(L, 2, LUA_TTABLE);
    luaL_checktype(L, 3, LUA_TTABLE);

    int nfaces = luaL_getn(L, 2);
    std::vector<int> nverts(nfaces);
    for (int i = 0; i < nfaces; ++i)
    {
        lua_rawgeti(L, 2, i + 1);
        nverts[i] = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
    }

    std::vector<int> verts(luaL_getn(L, 3));
    for (int i = 0; i < (int)verts.size(); ++i)
    {
        lua_rawgeti(L, 3, i + 1);
        verts[i] = luaL_checkinteger(L, -1);
        lua_pop(L, 1);
    }

    for (int i = 4; i < lua_gettop(L); i += 2)
        parameter(L, i);

    api_->mesh(interp, nfaces, &nverts[0], &verts[0]);

    clear();

    return 0;
}

int LuaGenerator::background(lua_State *L)
{
    api_->background();
    return 0;
}

int LuaGenerator::input(lua_State *L)
{
    api_->input(luaL_checkstring(L, 1));
    return 0;
}

int LuaGenerator::render(lua_State *L)
{
    api_->render();
    return 0;
}

int LuaGenerator::pushTransform(lua_State *L)
{
    api_->pushTransform();
    return 0;
}

int LuaGenerator::popTransform(lua_State *L)
{
    api_->popTransform();
    return 0;
}

int LuaGenerator::setTransform(lua_State *L)
{
    float M[16];
    for (int i = 0; i < 16; ++i)
        M[i] = (float)luaL_checknumber(L, i + 1);

    api_->setTransform(M);

    return 0;
}

int LuaGenerator::appendTransform(lua_State *L)
{
    float M[16];
    for (int i = 0; i < 16; ++i)
        M[i] = (float)luaL_checknumber(L, i + 1);

    api_->appendTransform(M);

    return 0;
}

int LuaGenerator::translate(lua_State* L)
{
	float x = (float)luaL_checknumber(L, 1);
	float y = (float)luaL_checknumber(L, 2);
	float z = (float)luaL_checknumber(L, 3);

	api_->translate(x, y, z);

	return 0;
}

int LuaGenerator::rotate(lua_State* L)
{
	float angle = (float)luaL_checknumber(L, 1);
	float x = (float)luaL_checknumber(L, 2);
	float y = (float)luaL_checknumber(L, 3);
	float z = (float)luaL_checknumber(L, 4);

	api_->rotate(angle, x, y, z);

	return 0;
}

int LuaGenerator::scale(lua_State* L)
{
	float x = (float)luaL_checknumber(L, 1);
	float y = (float)luaL_checknumber(L, 2);
	float z = (float)luaL_checknumber(L, 3);

	api_->scale(x, y, z);

	return 0;
}

int LuaGenerator::lookAt(lua_State *L)
{
	float ex = (float)luaL_checknumber(L, 1);
	float ey = (float)luaL_checknumber(L, 2);
	float ez = (float)luaL_checknumber(L, 3);
	float lx = (float)luaL_checknumber(L, 4);
	float ly = (float)luaL_checknumber(L, 5);
	float lz = (float)luaL_checknumber(L, 6);
	float ux = (float)luaL_checknumber(L, 7);
	float uy = (float)luaL_checknumber(L, 8);
	float uz = (float)luaL_checknumber(L, 9);

	api_->lookAt(ex, ey, ez,
                 lx, ly, lz,
                 ux, uy, uz);

	return 0;
}

int LuaGenerator::camera(lua_State *L)
{
    const char *name = luaL_checkstring(L, 1);

    for (int i = 2; i < lua_gettop(L); i += 2)
        parameter(L, i);

    api_->camera(name);

    clear();

    return 0;
}

int LuaGenerator::shaderGroupBegin(lua_State *L)
{
    api_->shaderGroupBegin();
    return 0;
}

int LuaGenerator::shader(lua_State *L)
{
    const char *shaderusage = luaL_checkstring(L, 1);
    const char *shadername = luaL_checkstring(L, 2);
    const char *layername = luaL_checkstring(L, 3);

    for (int i = 4; i < lua_gettop(L); i += 2)
        parameter(L, i);

    api_->shader(shaderusage, shadername, layername);

    clear();

    return 0;
}

int LuaGenerator::connectShaders(lua_State *L)
{
    const char *srclayer = luaL_checkstring(L, 1);
    const char *srcparam = luaL_checkstring(L, 2);
    const char *dstlayer = luaL_checkstring(L, 3);
    const char *dstparam = luaL_checkstring(L, 4);

    api_->connectShaders(srclayer, srcparam, dstlayer, dstparam);

    return 0;
}

int LuaGenerator::shaderGroupEnd(lua_State *L)
{
    api_->shaderGroupEnd();
    return 0;
}

}
}
