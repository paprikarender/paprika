#include <core/projectivecamera.hpp>

namespace paprika {
namespace core {

ProjectiveCamera::ProjectiveCamera(const core::Transform &worldToCamera,
                                   const core::Transform &cameraToScreen,
                                   const float screen[4],
                                   float hither, float yon,
                                   float shutterOpen, float shutterClose,
                                   float lensRadius, float focalDistance,
                                   int xres, int yres) :
    Camera(worldToCamera, hither, yon, shutterOpen, shutterClose, xres, yres),
    lensRadius_(lensRadius),
    focalDistance_(focalDistance),
    cameraToScreen_(cameraToScreen)
{
    Transform screenToNDC = core::Transform::translate(-screen[0], -screen[3], 0.f) *
                            core::Transform::scale(1.f / (screen[1] - screen[0]), 1.f / (screen[2] - screen[3]), 1.f);

    screenToRaster_ = screenToNDC * core::Transform::scale((float)xres_, (float)yres_, 1.f);

    rasterToCamera_ = screenToRaster_.inverse() * cameraToScreen_.inverse();

    NDCToCamera_ = screenToNDC.inverse() * cameraToScreen_.inverse();
}

}		// core
}		// paprika
