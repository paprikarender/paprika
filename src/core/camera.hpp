#ifndef CCAMERA_HPP
#define CCAMERA_HPP

#include <core/geometry.hpp>
#include <core/referenced.hpp>

namespace paprika {
namespace core {

struct CameraSample
{
    float imagex, imagey;
    float lensu, lensv;
    float time;
};

class Camera : public core::Referenced
{
public:
    Camera(const core::Transform &cameraToWorld,
           float hither, float yon,
           float shutterOpen, float shutterClose,
           int xres, int yres);

    virtual float generateRay(const core::CameraSample &sample, core::Ray *ray) const = 0;

    int xres() const
    {
        return xres_;
    }

    int yres() const
    {
        return yres_;
    }

    const core::Transform &cameraToWorld() const
    {
        return cameraToWorld_;
    }
    
    const core::Transform &worldToCamera() const
    {
        return worldToCamera_;
    }

    virtual bool isProjective() const
    {
        return false;
    }

protected:
    core::Transform cameraToWorld_;
    core::Transform worldToCamera_;
    float hither_, yon_;
    float shutterOpen_, shutterClose_;
    int xres_, yres_;
};

}		// core
}		// paprika

#endif
