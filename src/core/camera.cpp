#include <core/camera.hpp>

namespace paprika {
namespace core {

Camera::Camera(const core::Transform &cameraToWorld,
               float hither, float yon,
               float shutterOpen, float shutterClose,
               int xres, int yres) :
    cameraToWorld_(cameraToWorld),
    worldToCamera_(cameraToWorld.inverse()),
    hither_(hither), yon_(yon),
    shutterOpen_(shutterOpen), shutterClose_(shutterClose),
    xres_(xres), yres_(yres)
{

}

}		// core
}		// paprika

