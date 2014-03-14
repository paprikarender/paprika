#include "triangulate.hpp"
#include <core/debug.hpp>

namespace paprika {
namespace core {

float Triangulate::area(const Vec3* P, int nverts, int X, int Y)
{
	float A = 0.f;

	for (int p = nverts - 1, q = 0; q < nverts; p = q++)
		A += P[p][X] * P[q][Y] - P[q][X] * P[p][Y];

	return A * 0.5f;
}

bool Triangulate::snip(const Vec3* P, int u, int v, int w, int nverts, const int* V, bool swap) const
{
	const float EPSILON = 0.0000000001f;

	float Ax = P[V[u]][X];
	float Ay = P[V[u]][Y];

	float Bx = P[V[v]][X];
	float By = P[V[v]][Y];

	float Cx = P[V[w]][X];
	float Cy = P[V[w]][Y];

	if (swap)
	{
		std::swap(Ax, Cx);
		std::swap(Ay, Cy);
	}

	if (EPSILON > (((Bx - Ax) * (Cy - Ay)) - ((By - Ay) * (Cx - Ax))))
		return false;

	for (int p = 0; p < nverts; ++p)
	{
		if ((p == u) || (p == v) || (p == w))
			continue;

		float Px = P[V[p]][X];
		float Py = P[V[p]][Y];

		if (insideTriangle(Ax, Ay, Bx, By, Cx, Cy, Px, Py))
			return false;
	}

	return true;
}

bool Triangulate::insideTriangle(float Ax, float Ay, float Bx, float By, float Cx, float Cy, float Px, float Py)
{
	float ax = Cx - Bx;  float ay = Cy - By;
	float bx = Ax - Cx;  float by = Ay - Cy;
	float cx = Bx - Ax;  float cy = By - Ay;
	float apx = Px - Ax;  float apy = Py - Ay;
	float bpx = Px - Bx;  float bpy = Py - By;
	float cpx = Px - Cx;  float cpy = Py - Cy;

	float aCROSSbp = ax* bpy - ay* bpx;
	float cCROSSap = cx* apy - cy* apx;
	float bCROSScp = bx* cpy - by* cpx;

	return ((aCROSSbp >= 0.f) && (bCROSScp >= 0.f) && (cCROSSap >= 0.f));
}

// idea is from http://jgt.akpeters.com/papers/Sunday02
void Triangulate::findProjectionAxis(const Vec3* P, int nverts)
{
	float x = fabs(area(P, nverts, 1, 2));
	float y = fabs(area(P, nverts, 2, 0));
	float z = fabs(area(P, nverts, 0, 1));

	if (x > y && x > z)
	{
		X = 1;
		Y = 2;
	}
	else if (y > z)
	{
		X = 2;
		Y = 0;
	}
	else
	{
		X = 0;
		Y = 1;
	}
}

Triangulate::Triangulate(const Vec3* P, int nverts)
{
	if (nverts < 3)
		return;

	triangles.reserve((nverts - 2) * 3);

	// find projection axis (X and Y variables)
	findProjectionAxis(P, nverts);

	bool swap = 0.0f > area(P, nverts, X, Y);

	// allocate and initialize list of Vertices in polygon
	std::vector<int> V(nverts);
	for (int i = 0; i < nverts; i++)
		V[i] = i;

	int count = 2 * nverts;   // error detection

	int n = nverts;
	int v = 0;

	while (n > 2)
	{
		if (0 >= (count--))
		{
			core::Error("degeranete polygon");
			return;			// bad polygon
		}

		int v0 = v % n;
		int v1 = (v + 1) % n;
		int v2 = (v + 2) % n;

		if (snip(P, v0, v1, v2, n, &V[0], swap))
		{
			// output Triangle
			triangles.push_back(V[v0]);
			triangles.push_back(V[v1]);
			triangles.push_back(V[v2]);

			// remove v1 from remaining polygon
			V.erase(V.begin() + v1);		  
			n--;

			// resest error detection counter
			count = 2 * n;
		}
		else
		{
			// advance next polygon
			v++;
		}
	}
}

}		// geometry
}		// paprika
