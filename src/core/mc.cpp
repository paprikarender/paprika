#include <core/mc.hpp>
#include <core/geometry.hpp>

namespace paprika {
namespace core {

void concentricSampleDisk(float u1, float u2, float* dx, float* dy)
{
    float r, theta;

    // Map uniform random numbers to $[-1,1]^2$
    float sx = 2.f * u1 - 1.f;
    float sy = 2.f * u2 - 1.f;

    // Handle degeneracy at the origin
    if (sx == 0.0f && sy == 0.0f)
    {
        *dx = 0.0f;
        *dy = 0.0f;
        return;
    }

    // Map square to $(r,\theta)$
    if (sx >= -sy)
    {
        if (sx > sy)
        {
            // Handle first region of disk
            r = sx;
            if (sy > 0.0f)
                theta = sy / r;
            else
                theta = 8.0f + sy / r;
        }
        else
        {
            // Handle second region of disk
            r = sy;
            theta = 2.0f - sx / r;
        }
    }
    else
    {
        if (sx <= sy)
        {
            // Handle third region of disk
            r = -sx;
            theta = 4.0f - sy / r;
        }
        else
        {
            // Handle fourth region of disk
            r = -sy;
            theta = 6.0f + sx / r;
        }
    }

    theta *= F_PI / 4.f;

    *dx = r * cos(theta);
    *dy = r * sin(theta);
}


}
}



