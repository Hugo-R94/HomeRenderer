#include "../includes/HomeRenderer.hpp"

Vec3 rotatedVertex(const Mat3& M, const Vec3& v)
{
	Vec3 r;

	r.x = M.m[0][0]*v.x + M.m[0][1]*v.y + M.m[0][2]*v.z;
	r.y = M.m[1][0]*v.x + M.m[1][1]*v.y + M.m[1][2]*v.z;
	r.z = M.m[2][0]*v.x + M.m[2][1]*v.y + M.m[2][2]*v.z;

	return r;
}

Vec3 translatedVertex(const Vec3& MeshPos,const Vec3& Vpos)
{
	return {
		Vpos.x + MeshPos.x,
		Vpos.y + MeshPos.y,
		Vpos.z + MeshPos.z
	};
}


Vec2 toNDC(Vec2 p, float width, float height) {
    return {p.x / (width / 2.0f),
            p.y / (height / 2.0f)};
}

