#include "Frustum.h"
#include "SceneNode.h"
#include "Matrix4.h"

Frustum::Frustum()
{
}

Frustum::~Frustum()
{
}

void Frustum::FromMatrix(const Matrix4& pvMat)
{
	Vector3 xaxis = Vector3(pvMat.values[0], pvMat.values[4], pvMat.values[8]);
	Vector3 yaxis = Vector3(pvMat.values[1], pvMat.values[5], pvMat.values[9]);
	Vector3 zaxis = Vector3(pvMat.values[2], pvMat.values[6], pvMat.values[10]);
	Vector3 waxis = Vector3(pvMat.values[3], pvMat.values[7], pvMat.values[11]);

	//Right Plane
	planes[0] = Plane(waxis - xaxis, pvMat.values[15] - pvMat.values[12], true);

	//Left Plane
	planes[1] = Plane(waxis + xaxis, pvMat.values[15] + pvMat.values[12], true);

	//Bottom Plane
	planes[2] = Plane(waxis + yaxis, pvMat.values[15] + pvMat.values[13], true);

	//Top Plane
	planes[3] = Plane(waxis - yaxis, pvMat.values[15] - pvMat.values[13], true);

	//Near Plane
	planes[4] = Plane(waxis + zaxis, pvMat.values[15] + pvMat.values[14], true);

	//Far Plane
	planes[5] = Plane(waxis - zaxis, pvMat.values[15] - pvMat.values[14], true);
}

bool Frustum::InsideFrustum(SceneNode& node)
{
	for (int i = 0; i < 6; i++)
	{
		if (!planes[i].SphereInPlane(node.GetWorldTransform().GetPositionVector(), node.GetBoundingRadius())) { return false; }
	}
	return true;
}