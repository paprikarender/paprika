#ifndef TRIANGULATE_HPP
#define TRIANGULATE_HPP

#include <vector>
#include <core/geometry.hpp>

namespace paprika {
namespace core {

class Triangulate
{
public:
	Triangulate(const Vec3* P, int nverts);
	std::vector<int> triangles;

private:
	int X, Y;
	static float area(const Vec3* P, int nverts, int X, int Y);
	bool snip(const Vec3* P, int u, int v, int w, int nverts, const int* V, bool swapDirection) const;
	void findProjectionAxis(const Vec3* P, int nverts);

	static bool insideTriangle(float Ax, float Ay, float Bx, float By, float Cx, float Cy, float Px, float Py);
};

}		// geometry
}		// paprika

#endif

