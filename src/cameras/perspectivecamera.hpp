#ifndef CORE_PERSPECTIVECAMERA_HPP
#define CORE_PERSPECTIVECAMERA_HPP

#include <core/projectivecamera.hpp>
#include <core/parametermap.hpp>

namespace paprika {
namespace camera {

class PerspectiveCamera : public core::ProjectiveCamera
{
public:
	PerspectiveCamera(	const core::Transform &worldToCamera,
						const float screen[4],
						float hither, float yon,
						float shutterOpen, float shutterClose,
						float lensRadius, float focalDistance,
						float fov,
						int xres, int yres);

    static PerspectiveCamera *create(const core::Transform &worldToCamera, core::ParameterMap &map);

	virtual float generateRay(const core::CameraSample &sample, core::Ray *ray) const;

};


}		// core
}		// paprika
#endif
