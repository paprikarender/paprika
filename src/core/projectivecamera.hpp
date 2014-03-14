#ifndef CORE_PROJECTIVECAMERA_HPP
#define CORE_PROJECTIVECAMERA_HPP

#include <core/camera.hpp>

namespace paprika {
namespace core {

class ProjectiveCamera : public core::Camera
{
public:
    ProjectiveCamera(const core::Transform &worldToCamera,
                     const core::Transform &cameraToScreen,
                     const float screen[4],
                     float hither, float yon,
                     float shutterOpen, float shutterClose,
                     float lensRadius, float focalDistance,
                     int xres, int yres);

    const core::Transform &cameraToScreen() const
    {
        return cameraToScreen_;
    }

    const core::Transform &screenToRaster() const
    {
        return screenToRaster_;
    }

    const core::Transform &rasterToCamera() const
    {
        return rasterToCamera_;
    }

    const core::Transform &NDCToCamera() const
    {
        return NDCToCamera_;
    }

    virtual bool isProjective() const
    {
        return true;
    }

protected:
    float lensRadius_;
    float focalDistance_;
    core::Transform cameraToScreen_;
    core::Transform rasterToCamera_;
    core::Transform screenToRaster_;
    core::Transform NDCToCamera_;
};

}		// core
}		// paprika

#endif
