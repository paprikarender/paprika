#ifndef PAPRIKA_HPP
#define PAPRIKA_HPP

#include <libpaprika_export.hpp>

namespace paprika {

namespace core {
class ParameterMap;
class Transform;
}

class LIBPAPRIKA_EXPORT PaprikaAPI
{
public:
    PaprikaAPI();
    ~PaprikaAPI();

    void parameter(const char *typedname, int val);
    void parameter(const char *typedname, float val);
    void parameter(const char *typedname, double val);
    void parameter(const char *typedname, const char *val);
    void parameter(const char *typedname, const int *val);
    void parameter(const char *typedname, const float *val);
    void parameter(const char *typedname, const char * const *val);

    void mesh(const char* interp, int nfaces, const int* nverts, const int* verts);
    void sphere(float radius);
    void background();
    
    void camera(const char *name);

    // Shaders
    void shaderGroupBegin();
    void shader(const char *shaderusage, const char *shadername, const char *layername);
    void connectShaders(const char* srclayer, const char* srcparam,
                        const char* dstlayer, const char* dstparam);
    void shaderGroupEnd();

    // Transformations
    void pushTransform();
    void popTransform();
    void setTransform(const float* M);
    void appendTransform(const float* M);
    void translate(float x, float y, float z);
    void rotate(float angle, float x, float y, float z);
    void scale(float x, float y, float z);
    void lookAt(float ex, float ey, float ez, 
                float lx, float ly, float lz,
                float ux, float uy, float uz);

    void world();
    void render();
    void output(const char* name, const char* format, const char* dataname);

    void input(const char *filename);

private:
    struct PaprikaData;
    PaprikaData *d_;
};

}		// paprika
#endif	// PAPRIKA_HPP