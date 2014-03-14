#include <cameras/perspectivecamera.hpp>
#include <core/mc.hpp>

namespace paprika {
namespace camera {

PerspectiveCamera::PerspectiveCamera(const core::Transform &worldToCamera,
                                     const float screen[4],
                                     float hither, float yon,
                                     float shutterOpen, float shutterClose,
                                     float lensRadius, float focalDistance,
                                     float fov,
                                     int xres, int yres) :
    ProjectiveCamera(worldToCamera, core::Transform::perspective(fov, hither, yon), screen, hither, yon, shutterOpen, shutterClose, lensRadius, focalDistance, xres, yres)
{

}

PerspectiveCamera *PerspectiveCamera::create(const core::Transform &worldToCamera, core::ParameterMap &map)
{
	const int resolution_def[] = {640, 480};

	float hither = std::max(1e-4f, map.find("hither", OIIO::TypeDesc::FLOAT, 1e-3f));
	float yon = std::min(map.find("yon", OIIO::TypeDesc::FLOAT, 1e30f), 1e30f);
	float shutteropen = map.find("shutteropen", OIIO::TypeDesc::FLOAT, 0.f);
	float shutterclose = map.find("shutterclose", OIIO::TypeDesc::FLOAT, 1.f);
	float lensradius = map.find("lensradius", OIIO::TypeDesc::FLOAT, 0.f);
	float focaldistance = map.find("focaldistance", OIIO::TypeDesc::FLOAT, 1e30f);
    const int* resolution = map.find("resolution", OIIO::TypeDesc(OIIO::TypeDesc::INT, 2), resolution_def);
	float frame = map.find("frameaspectratio", OIIO::TypeDesc::FLOAT, (float)resolution[0] / (float)resolution[1]);

	float screen_def[4];
	if (frame > 1.f)
	{
		screen_def[0] = -frame;
		screen_def[1] =  frame;
		screen_def[2] = -1.f;
		screen_def[3] =  1.f;
	}
	else
	{
		screen_def[0] = -1.f;
		screen_def[1] =  1.f;
		screen_def[2] = -1.f / frame;
		screen_def[3] =  1.f / frame;
	}
    const float* screen = map.find("screen", OIIO::TypeDesc(OIIO::TypeDesc::FLOAT, 4), screen_def);

	float fov = map.find("fov", OIIO::TypeDesc::FLOAT, 90.f);

    return new PerspectiveCamera(worldToCamera, screen, hither, yon, shutteropen, shutterclose, lensradius, focaldistance, fov, resolution[0], resolution[1]); 
}


float PerspectiveCamera::generateRay(const core::CameraSample &sample, core::Ray *ray) const
{
    OSL::Dual2<core::Vec3> pras(core::Vec3(sample.imagex, sample.imagey, 0.f), core::Vec3(1, 0, 0), core::Vec3(0, 1, 0));
    OSL::Dual2<core::Vec3> pcamera = rasterToCamera_.transformPoint(pras);

    ray->o = core::Vec3(0.f, 0.f, 0.f);
    ray->d = pcamera;

    // set ray time value
    //ray->time = core::lerp(sample.time, shutterOpen_, shutterClose_);             // TODO

#if 0
    // modify ray for depth of field
    if (lensRadius_ > 0.f)
    {
        // sample point on lens
        float lensu, lensv;
        core::concentricSampleDisk(sample.lensu, sample.lensv, &lensu, &lensv);
        lensu *= lensRadius_;
        lensv *= lensRadius_;

        // compute point on plane of focus
        float ft = (focalDistance_ - hither_) / ray->d.z;
        core::Point Pfocus = (*ray)(ft);

        // update ray for effect of lens
        ray->o.x += lensu;
        ray->o.y += lensv;
        ray->d = Pfocus - ray->o;
    }
#endif

    ray->d = normalize(ray->d);
    //ray->mint = 0.;                               // TODO
    //ray->maxt = (yon_ - hither_) / ray->d.z;      // TODO
    *ray = cameraToWorld_.transformRay(*ray);
    return 1.f;
}

}		// camera
}		// paprika
