#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include <core/referenced.hpp>
#include <iostream>
#include <cmath>
#include <limits>
#include <OSL/dual.h>
#include <OSL/dual_vec.h>
#include <OpenEXR/ImathBox.h>

#define D_PI 3.1415926535897932384626433832795
#define F_PI 3.1415926535897932384626433832795f
#define F_PI_DIV_180 0.01745329251994329576923690768489f
#define F_INV_PI 0.31830988618379067153776752674503f

namespace paprika {
namespace core {

typedef OSL::Vec2 Vec2;
typedef OSL::Vec3 Vec3;
typedef OSL::Matrix22 Matrix22;
typedef OSL::Matrix33 Matrix33;
typedef OSL::Matrix44 Matrix44;
typedef OSL::Matrix44 Matrix;
typedef Imath::Box<Vec3> BBox;
typedef OSL::Color3 Color3;

struct Ray
{
    Ray(float tnear = 1e-3f, float tfar = 1e30f) : tnear(tnear), tfar(tfar) {}
    Ray(const OSL::Dual2<Vec3>& o, const OSL::Dual2<Vec3>& d, float tnear = 1e-3f, float tfar = 1e30f) : o(o), d(d), tnear(tnear), tfar(tfar) {}

    Vec3 point(float t) const
	{
        return o.val() + d.val() * t;
    }

    OSL::Dual2<Vec3> point(const OSL::Dual2<float> &t) const
	{
        return o + d * t;
    }

    OSL::Dual2<Vec3> o, d;
    float tnear, tfar;
};

class Transform
{
public:
	Transform() {}

	Transform(const Matrix &m)
	{
		m_ = m;
		mInv_ = m_.inverse(false);
		mInvT_ = top(mInv_).transposed();
	}

	Transform(const Matrix &m, const Matrix &mInv)
	{
		m_ = m;
		mInv_ = mInv;
		mInvT_ = top(mInv_).transposed();
	}

	Transform(const float *m)
	{
		m_ = Matrix(m[0], m[1], m[2], m[3], 
					m[4], m[5], m[6], m[7],
					m[8], m[9], m[10], m[11],
					m[12], m[13], m[14], m[15]);

		mInv_ = m_.inverse(false);

		mInvT_ = top(mInv_).transposed();
	}

	Transform(const float *m, const float *mInv)
	{
		m_ = Matrix(m[0], m[1], m[2], m[3], 
					m[4], m[5], m[6], m[7],
					m[8], m[9], m[10], m[11],
					m[12], m[13], m[14], m[15]);

		mInv_ = Matrix(mInv[0], mInv[1], mInv[2], mInv[3], 
					   mInv[4], mInv[5], mInv[6], mInv[7],
					   mInv[8], mInv[9], mInv[10], mInv[11],
					   mInv[12], mInv[13], mInv[14], mInv[15]);

		mInvT_ = top(mInv_).transposed();
	}

	Vec3 transformPoint(const Vec3 &v) const
	{
		return v * m_;
	}

	Vec3 transformVector(const Vec3 &v) const
	{
		Vec3 v2;
		m_.multDirMatrix(v, v2);
		return v2;
	}

	Vec3 transformNormal(const Vec3 &v) const
	{
		return v * mInvT_;
	}

	OSL::Dual2<Vec3> transformPoint(const OSL::Dual2<Vec3> &v) const
	{
		OSL::Dual2<Vec3> v2;
		OSL::robust_multVecMatrix(m_, v, v2);
		return v2;
	}

	OSL::Dual2<Vec3> transformVector(const OSL::Dual2<Vec3> &v) const
	{
		OSL::Dual2<Vec3> v2;
		OSL::multDirMatrix(m_, v, v2);
		return v2;
	}

	OSL::Dual2<Vec3> transformNormal(const OSL::Dual2<Vec3> &v) const
	{
		OSL::Dual2<Vec3> v2;
		OSL::multMatrix(mInvT_, v, v2);
		return v2;
	}

	core::Ray transformRay(const core::Ray &ray) const
	{
		return core::Ray(transformPoint(ray.o), transformVector(ray.d), ray.tnear, ray.tfar);
	}

    const Matrix44 &matrix() const
    {
        return m_;
    }

    const Matrix44 &inverseMatrix() const
    {
        return mInv_;
    }

	Transform inverse() const
	{
		return Transform(mInv_, m_);
	}

	friend Transform operator * (const Transform &t1, const Transform &t2)
	{
		return Transform(t1.m_ * t2.m_, t2.mInv_ * t1.mInv_);
	}

	static Transform translate(float x, float y, float z)
	{
		Matrix m;
		m.setTranslation(Vec3(x, y, z));

		Matrix mInv;
		mInv.setTranslation(Vec3(-x, -y, -z));

		return Transform(m, mInv);
	}

	static Transform rotate(float angle, float x, float y, float z)
	{
		Vec3 a = Vec3(x, y, z).normalized();
        float s = sin(angle * F_PI_DIV_180);
        float c = cos(angle * F_PI_DIV_180);

		float m[4][4];

		m[0][0] = a.x * a.x + (1.f - a.x * a.x) * c;
		m[0][1] = a.x * a.y * (1.f - c) + a.z * s;
		m[0][2] = a.x * a.z * (1.f - c) - a.y * s;
		m[0][3] = 0;

		m[1][0] = a.x * a.y * (1.f - c) - a.z * s;
		m[1][1] = a.y * a.y + (1.f - a.y * a.y) * c;
		m[1][2] = a.y * a.z * (1.f - c) + a.x * s;
		m[1][3] = 0;

		m[2][0] = a.x * a.z * (1.f - c) + a.y * s;
		m[2][1] = a.y * a.z * (1.f - c) - a.x * s;
		m[2][2] = a.z * a.z + (1.f - a.z * a.z) * c;
		m[2][3] = 0;

		m[3][0] = 0;
		m[3][1] = 0;
		m[3][2] = 0;
		m[3][3] = 1;

		return Transform(Matrix(m), Matrix(m).transposed());
	}

	static Transform scale(float x, float y, float z)
	{
		Matrix m;
		m.setScale(Vec3(x, y, z));

		Matrix mInv;
		mInv.setScale(Vec3(1 / x, 1 / y, 1 / z));

		return Transform(m, mInv);
	}

	static Transform lookAt(float ex, float ey, float ez, 
							float lx, float ly, float lz,
							float ux, float uy, float uz)
	{
		Vec3 eye(ex, ey, ez);
		Vec3 look(lx, ly, lz);
		Vec3 up(ux, uy, uz);

		float m[4][4];

		m[3][0] = eye.x;
		m[3][1] = eye.y;
		m[3][2] = eye.z;
		m[3][3] = 1;

		Vec3 dir = (look - eye).normalized();
		Vec3 left = (up.cross(dir)).normalized();
		Vec3 newUp = dir.cross(left);

		m[0][0] = left.x;
		m[0][1] = left.y;
		m[0][2] = left.z;
		m[0][3] = 0;

		m[1][0] = newUp.x;
		m[1][1] = newUp.y;
		m[1][2] = newUp.z;
		m[1][3] = 0;

		m[2][0] = dir.x;
		m[2][1] = dir.y;
		m[2][2] = dir.z;
		m[2][3] = 0;

		return Transform(Matrix(m), Matrix(m).inverse());
	}

    static Transform perspective(float fov, float n, float f)
    {
	    // Perform projective divide
	    float inv_denom = 1.f / (f - n);
	    Matrix persp(1, 0, 0, 0,
                     0, 1, 0, 0,
                     0, 0, f * inv_denom, 1,
                     0, 0, -f * n * inv_denom, 0);

	    // Scale to canonical viewing volume
        float invTanAng = 1.f / tanf(fov * 0.5f * F_PI_DIV_180);
	    return Transform(persp) * scale(invTanAng, invTanAng, 1);
    }


private:
	Matrix33 top(const Matrix44 &m)
	{
		return Matrix33(m[0][0], m[0][1], m[0][2],
						m[1][0], m[1][1], m[1][2], 
						m[2][0], m[2][1], m[2][2]);
	}

	Matrix44 m_;
	Matrix44 mInv_;
	Matrix33 mInvT_;
};


}		// core
}		// paprika
#endif	// GEOMETRY_HPP
