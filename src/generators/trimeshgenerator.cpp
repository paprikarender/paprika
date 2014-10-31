#include <generators/trimeshgenerator.hpp>
#include <api/paprikaapi.hpp>
#include <core/debug.hpp>
#include <vector>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

namespace paprika {
namespace generator {

struct MemoryStream
{
    size_t offset;
    unsigned char *buffer;
};

template <typename T>
static T read(MemoryStream *stream)
{
    T t = *(T*)(stream->buffer + stream->offset);
    stream->offset += sizeof(T);
    return t;
}

template <typename T>
static T *readArray(MemoryStream *stream, size_t size)
{
    T *t = (T*)(stream->buffer + stream->offset);
    stream->offset += sizeof(T)* size;
    return t;
}
    
void TriMeshGenerator::run(PaprikaAPI *p, const char *params)
{
    const char *fileName = params;

    FILE *stream = fopen(fileName, "rb");

    if (stream == NULL)
    {
        core::Error("Cannot open: %s", fileName);
        return;
    }

    fseek(stream, 0, SEEK_END);
    long streamSize = ftell(stream);
    fseek(stream, 0, SEEK_SET);

    MemoryStream m;
    m.offset = 0;
    m.buffer = (unsigned char*)malloc(streamSize);
    fread(m.buffer, 1, streamSize, stream);

    uint32_t flags = read<uint32_t>(&m);
    uint64_t nverts = read<uint64_t>(&m);
    uint64_t ntris = read<uint64_t>(&m);

    float *verts = readArray<float>(&m, nverts * 3);
    float *normals = readArray<float>(&m, nverts * 3);
    float *uvs = readArray<float>(&m, nverts * 2);
    uint32_t *tris = readArray<uint32_t>(&m, ntris * 3);

    std::vector<float> us(nverts);
    std::vector<float> vs(nverts);
    for (int i = 0; i < nverts; ++i)
    {
        us[i] = uvs[i * 2];
        vs[i] = uvs[i * 2 + 1];
    }

    p->parameter("vertex point P", verts);
    p->parameter("vertex normal N", normals);
    p->parameter("vertex float u", &us[0]);
    p->parameter("vertex float v", &vs[0]);

    std::vector<int> nfaces(ntris, 3);

    p->mesh("linear", ntris, &nfaces[0], (int*)tris);

    free(m.buffer);

    fclose(stream);
}

}
}
